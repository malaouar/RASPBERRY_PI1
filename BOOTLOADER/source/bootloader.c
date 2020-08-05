
// The raspberry pi firmware at the time this was written defaults
// loading at address 0x8000.  Although this bootloader could easily
// load at 0x0000, it loads at 0x8000 so that the same binaries built
// for the SD card work with this bootloader.  Change the ARMBASE
// below to use a different location.

#define ARMBASE 0x8000

extern void PUT32 ( unsigned int, unsigned int );
extern void PUT16 ( unsigned int, unsigned int );
extern void PUT8 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void BRANCHTO ( unsigned int );
extern void dummy ( unsigned int );

/* Start of memory-mapped peripherals address space  */
#define PERIPHERALS_BASE 0x20000000
/* Power management / watchdog timer  */
#define PM_REGS_BASE           (PERIPHERALS_BASE + 0x100000)

#define PM_RSTC                       (PM_REGS_BASE + 0x1c)
#define PM_WDOG                       (PM_REGS_BASE + 0x24)

#define PM_PASSWORD                   0x5a000000
#define PM_RSTC_WRCFG_CLR             0xffffffcf
#define PM_RSTC_WRCFG_FULL_RESET      0x00000020

#define PM_WDOG_UNITS_PER_SECOND      (1 << 16)
#define PM_WDOG_UNITS_PER_MILLISECOND (PM_WDOG_UNITS_PER_SECOND / 1000)

#define ARM_TIMER_CTL 0x2000B408
#define ARM_TIMER_CNT 0x2000B420

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

#define IRQ_BASIC 0x2000B200
#define IRQ_PEND1 0x2000B204
#define IRQ_PEND2 0x2000B208
#define IRQ_FIQ_CONTROL 0x2000B210
#define IRQ_ENABLE1 0x2000B210
#define IRQ_ENABLE2 0x2000B214
#define IRQ_ENABLE_BASIC 0x2000B218
#define IRQ_DISABLE1 0x2000B21C
#define IRQ_DISABLE2 0x2000B220
#define IRQ_DISABLE_BASIC 0x2000B224


//------------------------------------------------------------------------
unsigned int uart_recv ( void ){
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x01) break;
    }
    return(GET32(AUX_MU_IO_REG)&0xFF);
}
//------------------------------------------------------------------------
void uart_send ( unsigned int c ){
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x20) break;
    }
    PUT32(AUX_MU_IO_REG,c);
}
//------------------------------------------------------------------------
// send a string one char at a time to uart
void puts (char *s) {
int i;
for (i=0; s[i] != 0; i++) uart_send(s[i]);
}
//------------------------------------------------------------------------
void hexstrings ( unsigned int d ){
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}
//------------------------------------------------------------------------
void hexstring ( unsigned int d ){
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}
//------------------------------------------------------
void uart_init(){ 
	unsigned int ra;
    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    PUT32(AUX_MU_IER_REG,0x05); //enable rx interrupts
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);

    ra=GET32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    PUT32(GPFSEL1,ra);
    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,(1<<14)|(1<<15));
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,0);
    PUT32(AUX_MU_CNTL_REG,3);
}
//----------------------------------------
/*
 * Argument msecs: Milliseconds to wait until the system resets itself.
 */
void watchdogset(unsigned int msecs){
    *(volatile unsigned int*)PM_WDOG = PM_PASSWORD | (msecs * PM_WDOG_UNITS_PER_MILLISECOND);
    unsigned int cur = *(volatile unsigned int*)PM_RSTC;
    *(volatile unsigned int*)PM_RSTC = PM_PASSWORD | (cur & PM_RSTC_WRCFG_CLR) |
                               PM_RSTC_WRCFG_FULL_RESET;
}

//------------------------------------------------------------------------
unsigned char xstring[256];
//------------------------------------------------------------------------
int kmain ( void ){
	char *str= " \r\nLM BootLoader..!!\r\n";
	char *str2= "Please send any character when you are Ready........\r\n";
 	unsigned int a;
    
	PUT32(IRQ_DISABLE1,1<<29);	// disable uart1 receive irq

	uart_init();
    
	puts(str);
	puts(str2);

	PUT32(IRQ_ENABLE1,1<<29);  // enable uart1 receive irq
	enable_irq();			// enable irq on CPU

	halt();  // wait for a character to weakup from sleep mode

    return(0);
}

//===============================
void xmodem(){
    unsigned int ra, rb;
    unsigned int rx;
    unsigned int addr;
    unsigned int block;
	unsigned int crc;
	char *str= " \r\nLoad your file using XMODEM protocol....\r\n";
	puts(str);

//SOH 0x01  start of transfer
//ACK 0x06
//NAK 0x15
//EOT 0x04  end of transfer

//there is 132 byte in each packet:
//1st byte --> SOH
//2nd byte --> block number (block numbers start with 1)
//3rd byte --> 2s complement of 2nd byte (255-block number)
//a block of 128 bytes of data
//132th byte --> checksum  (sum of previous 131 bytes)

//when trasfer done the host send a single EOT byte, send an ACK on it too

	 //  clear the receive FIFO 
	ra=GET32(AUX_MU_IO_REG); //read 
	ra=GET32(AUX_MU_IO_REG); //read 
	ra=GET32(AUX_MU_IO_REG); //read 


	PUT32(ARM_TIMER_CTL,0x00F90000);
    PUT32(ARM_TIMER_CTL,0x00F90200);

   rx=GET32(ARM_TIMER_CNT);
    while(1)
    {
        ra=GET32(ARM_TIMER_CNT);
        if((ra-rx)>=4000000)
        {
            uart_send(0x15);
            rx+=4000000;
        }
        if(GET32(AUX_MU_LSR_REG)&0x01) break;
    }
    block=1;
    addr=ARMBASE;
    while(1)
    {
        xstring[0]=uart_recv();
        if(xstring[0]==0x04)
        {
            uart_send(0x06);
            break;
        }
        if(xstring[0]!=0x01) break;
        crc=0x01;
        for(ra=1;ra<132;ra++)
        {
            xstring[ra]=uart_recv();
            crc+=xstring[ra];
        }
        if(xstring[2]!=(255-xstring[1])) break;
        crc-=xstring[131];
        crc&=0xFF;
        if(xstring[131]!=crc)
        {
            uart_send(0x15);
        }
        for(ra=0;ra<128;ra++)
        {
            PUT8(addr++,xstring[ra+3]);
        }
        if(addr>0x200000)
        {
            uart_send(0x15);
            break;
        }
        uart_send(0x06);
        block=(block+1)&0xFF;
    }
    if(xstring[0]==0x04)
    {
		puts("\r\nTransfer DONE. \r\nLaunching your program ...");
        BRANCHTO(ARMBASE);
    }
 		//if some thing goes wrong reset system
	puts("Transfer PROBLEM !! reseting system now...");
	watchdogset(1000); // reset after 1000ms=1s 
	halt();
}


