//#include "gpio.h"
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void hexstring ( unsigned int );
int sendcmd(int cmd, int arg);
//====================================================
//sd send command
int sendcmd(int cmd, int arg)
{
	int i, r1=0;
	int retry=0;

  // CS=0
	PUT32(0x2020002C, 1<<21);  // GPCLR1
	
	tx(cmd | 0x40);	      //send command
	tx(arg>>24);      // send MS Byte first
	tx(arg>>16);
	tx(arg>>8);
	tx(arg);
	tx(0x95);   // send CRC
	
	while((r1 = rx()) == 0xff)	//wait response
		if(retry++ > 0xfe) break;					//time out error

	
	if (cmd==58) 	 for (i=0; i<4; i++) {
	  r1= rx();
	  hexstring(r1);
	} 

	//CS=1  
	PUT32(0x20200020, 1<<21); //GPSET1

	tx(0xff);				      // extra 8 CLK
  
	return r1;					  //return state
}


//==================================================================
//Init sd card 
int sd_init(void)
{
	int i;
	int retry;
	int r1=0;
	retry = 0;
	do
	{
		for(i=0;i<10;i++) tx(0xff);
		r1 = sendcmd(0, 0);	//send idle command 
		retry++;
		if(retry>0xfe) return 1;		//time out
	} while(r1 != 0x01);	

	retry = 0;
	do
	{
		r1 = sendcmd(1, 0);	//send active command
		retry++;
		if(retry>0xfe) return 1;		//time out
	} while(r1); // while r1 =/= 0
	
	sendcmd(58, 0); //read OCR
	
  
	r1 = sendcmd(16, 512);	//set sector size to 512
	
	return 0;		//normal return
}
