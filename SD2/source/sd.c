
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void hexstring ( unsigned int );
int sendcmd(int cmd, int arg);
char sector[512];
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
 
	r1 = sendcmd(16, 512);	//set sector size to 512
	
	return 0;		//normal return
}

//===============================================
		//write one sector 

int writesector(int no_secteur)
{
	int r1;
	int i;
	int retry=0;

	r1 = sendcmd(24, no_secteur<<9);	//send command
	if(r1 != 0x00)
		return r1;

	PUT32(0x2020002C, 1<<21);  //CS=0
	
	tx(0xff);
	tx(0xff);
	tx(0xff);

	tx(0xfe);			//send start byte "token"
	
	for(i=0; i<512; i++)		//send 512 bytes data
	{
		tx(sector[i]);
	}
	
	tx(0xff);			//dummy crc
	tx(0xff);
	
	r1 = tx(0xff);
	
	if( (r1&0x1f) != 0x05)	//judge if it successful
	{
		PUT32(0x20200020, 1<<21); //CS=1
		return r1;
	}
			//wait no busy
	while(!tx(0xff))if(retry++ > 0xfffe){PUT32(0x20200020, 1<<21); //CS=1
                  return 1;}

	PUT32(0x20200020, 1<<21); //CS=1
	tx(0xff);// extra 8 CLK

	return 0;
}
