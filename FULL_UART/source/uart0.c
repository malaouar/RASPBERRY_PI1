
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define GPPUD       0x20200094	// Controls actuation of pull up/down to ALL GPIO pins.
#define GPPUDCLK0   0x20200098	// Controls actuation of pull up/down for specific GPIO pin.


// UART0 (PL011) Page 175 
#define UART0_BASE   0x20201000   		// The base address for UART.

// The offsets for reach register for the UART.
#define UART0_DR     (UART0_BASE+0x00)  // Data Register
#define UART0_RSRECR (UART0_BASE+0x04)	// receive status register/error clear register
#define UART0_FR     (UART0_BASE+0x18)	// Flag register
#define UART0_ILPR   (UART0_BASE+0x20)	// Not used
#define UART0_IBRD   (UART0_BASE+0x24)	// Integer Baud rate divisor
#define UART0_FBRD   (UART0_BASE+0x28)	// Fractional Baud rate divisor
#define UART0_LCRH   (UART0_BASE+0x2C)	//
#define UART0_CR     (UART0_BASE+0x30)  // Control register
#define UART0_IFLS   (UART0_BASE+0x34)
#define UART0_IMSC   (UART0_BASE+0x38)
#define UART0_RIS    (UART0_BASE+0x3C)
#define UART0_MIS    (UART0_BASE+0x40)
#define UART0_ICR    (UART0_BASE+0x44)
#define UART0_DMACR  (UART0_BASE+0x48)
#define UART0_ITCR   (UART0_BASE+0x80)
#define UART0_ITIP   (UART0_BASE+0x84)
#define UART0_ITOP   (UART0_BASE+0x88)
#define UART0_TDR    (UART0_BASE+0x8C)

//------------------------------------------------------------------------
void uart_putc(unsigned int c)  // send a character
{
    while(1) // wait for UART to become ready to transmit: bit5 TXFF= 0 ( if FIFO enabled ?)
    {
        if((GET32(UART0_FR)&0x20)==0) break; // if  transmiter empty so break else continue testing (1<<5 = 0x20)
       // or: if (!(GET32(UART0_FR) & (1 << 5)))    0x20=1<<5=100000
    }
    PUT32(UART0_DR,c);
}
//------------------------------------------------------------------------
unsigned int uart_getc(void)  // receive character
{
    while(1) // wait for a character to be received: bit4 RXFE=0 (if FIFO enabled ?)
    {
        if((GET32(UART0_FR)&0x10)==0) break; // if char received so break else continue testing (1<<4 = 0x10 = 10000)
    }
    return(GET32(UART0_DR));
}
//------------------------------------------------------------------------
void puts(char *s) // send a zero terminated string one char at a time to uart
{
int i;
for (i=0; s[i] != 0; i++) uart_putc(s [i]);
}
//------------------------------------------------------------------------
void uart_init(void)
{
    unsigned int ra;

    // Disable UART0.
    PUT32(UART0_CR,0);

//GPIO14  TXD0 and TXD1
//GPIO15  RXD0 and RXD1
//alt function 5 for uart1
//alt function 0 for uart0

    // change gpio14/15 function to alt0:
    ra=GET32(GPFSEL1);  // read current vlaue of GPFSEL1
    ra&=~(7<<12); //gpio14
    ra|=4<<12;    //alt0
    ra&=~(7<<15); //gpio15
    ra|=4<<15;    //alt0
    PUT32(GPFSEL1,ra);  // save new vlaue of GPFSEL1

    // Disable pull up/down for all GPIO pins & delay for 150 cycles. 
    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);

   // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    PUT32(GPPUDCLK0,(1<<14)|(1<<15));
    for(ra=0;ra<150;ra++) dummy(ra);

    // Write 0 to GPPUDCLK0 to make it take effect.
    PUT32(GPPUDCLK0,0);

    // Clear pending interrupts.
    PUT32(UART0_ICR,0x7FF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.
    
    // Divider = 3000000/(16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.  
    PUT32(UART0_IBRD,1);
    PUT32(UART0_FBRD,40);

    // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
    PUT32(UART0_LCRH,0x70);  // 0x70 = (1 << 4) | (1 << 5) | (1 << 6) = 1110000

/*
   // Mask all interrupts.
    PUT32(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) |
               (1 << 6) | (1 << 7) | (1 << 8) |
               (1 << 9) | (1 << 10));
*/

    // Enable UART0, receive & transfer part of UART.
    PUT32(UART0_CR,0x301);  // 0x301 = (1 << 0) | (1 << 8) | (1 << 9) = 1100000001
}

//------------------------------------------------------------------------
int kmain(void)
{
    unsigned int ra;
    char *str;
    str = "Hello Sebseb\n";

    uart_init();

    puts(str); // display string


    while(1)  // forever
    {
        ra=uart_getc();   // get character
        if(ra==0x0D) uart_putc(0x0A);
	puts(str);
        uart_putc(ra);  // and eho it back
    }


    return(0);
}

/* To enable transmission, the TXE bit and UARTEN bit must be set to 1.
Similarly, to enable reception, the RXE bit and UARTEN bit, must be set to 1.
NOTE:
Program the control registers as follows:
1. Disable the UART.
2. Wait for the end of transmission or reception of the current character.
3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control
Register, UART_LCRH.
4. Reprogram the Control Register, UART_CR.
5. Enable the UART.  */
