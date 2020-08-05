
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

#define GPFSEL1         (0x20200004)
#define GPSET0          (0x2020001C)
#define GPCLR0          (0x20200028)
#define GPPUD           (0x20200094)
#define GPPUDCLK0       (0x20200098)

#define AUX_ENABLES     (0x20215004)
#define AUX_MU_IO_REG   (0x20215040)
#define AUX_MU_IER_REG  (0x20215044)
#define AUX_MU_IIR_REG  (0x20215048)
#define AUX_MU_LCR_REG  (0x2021504C)
#define AUX_MU_MCR_REG  (0x20215050)
#define AUX_MU_LSR_REG  (0x20215054)
#define AUX_MU_MSR_REG  (0x20215058)
#define AUX_MU_SCRATCH  (0x2021505C)
#define AUX_MU_CNTL_REG (0x20215060)
#define AUX_MU_STAT_REG (0x20215064)
#define AUX_MU_BAUD_REG (0x20215068)

//------------------------------------------------------------------------
void puts (char *s) // send a string one char at a time to uart
{
int i;
for (i=0; s[i] != 0; i++) uart_send(s [i]);
}

//------------------------------------------------------------------------
void uart_send ( unsigned int c )
{
    while(1) // if SET then transmiter empty so break else continue testing 
    {
        if(GET32(AUX_MU_LSR_REG)&0x20) break; //(1<<5 = 0x20)
    }
    PUT32(AUX_MU_IO_REG,c);
}

//------------------------------------------------------------------------
void hexstrings ( unsigned int d )
{
    //unsigned int ra;
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
void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}
//------------------------------------------------------------------------
void uart_init ( void )
{
    unsigned int ra;

    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    PUT32(AUX_MU_IER_REG,0);
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


