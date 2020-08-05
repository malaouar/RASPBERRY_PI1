//#include "gpio.h"

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

extern void uart_init ( void );
extern void uart_send ( unsigned int );
extern void hexstring ( unsigned int );
void puts (char *s);
void  spi_init();  
int sd_init();
int writesector(int no_secteur);
 char sector[512];
//===================================================

void fillram(void)	 { // fill RAM sector with ASCII characters
	int i,c;
	char mystring[14] = "mahmoud LAOUAR";
	c = 0;
	for (i=0;i<=512;i++) {
		sector[i] = mystring[c];
		c++;
		if (c > 13) c = 0;
	}
}

//==================
int kmain()
{
     int ra;
    
    char *str1, *str2, *str3, *str4;
    str1 ="hello sebseb\n";
    str2 ="WOW Goooooooood !!!\n";
    str3 ="Ooooops !!!\n";
    str4 ="Write OK !!!\n";
   
    uart_init();
	hexstring(0xDEADBEEF);
	hexstring(0x01234567);
	hexstring(0xDEADBEEF);

  puts(str1); // display string
	
  spi_init();
  
  if((ra =sd_init()) == 0)   puts(str2);
  else puts(str3);
	
	// fill buffer with  text
	 fillram();
	
	// write biffer to sector 3
	  if((ra= writesector(3))==0) puts(str4);
	 else puts(str3);
  
    return(0);
}

