//#include "gpio.h"

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

extern void uart_init ( void );
extern void uart_send ( unsigned int );
extern void hexstring ( unsigned int );
void puts (char *s);
void  spi_init();  
int sd_init();

//------------------------------------------------------------------------
int kmain()
{
     int ra;
    
    char *str1, *str2, *str3;
    str1 ="hello sebseb\n";
    str2 ="WOW Goooooooood !!!\n";
    str3 ="Ooooops !!!\n";
    
   
    uart_init();
	hexstring(0xDEADBEEF);
	hexstring(0x01234567);
	hexstring(0xDEADBEEF);

  puts(str1); // display string
	
  spi_init();
  
  if((ra =sd_init()) == 0)   puts(str2);
  else puts(str3);
  
    return(0);
}

