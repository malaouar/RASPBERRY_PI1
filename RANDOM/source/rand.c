
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

extern void uart_init ( void );
extern void uart_send ( unsigned int );

extern void hexstrings ( unsigned int );
extern void hexstring ( unsigned int );


//------------------------------------------------------------------------
int kmain()
{
    unsigned int ra;

    uart_init();
	hexstring(0xDEADBEEF);
	hexstring(0x01234567);
 
	// generate random numbers
   ra=0;
	while(1) {
    ra = rand(ra); // generate
	hexstring(ra); // convert to hex and send to us over UART
	
  }


    return(0);
}

