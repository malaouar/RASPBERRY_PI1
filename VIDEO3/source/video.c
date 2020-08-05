//This example displays random pixels on screen 1024x768  8bits colors


extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );


//=========================================================================
/* 
Address 	Size/Bytes 	Name 		Description 			R/W
2000B880 	4 			Read 		Receiving mail. 		R
2000B890 	4 			Poll 		Receive without retrieving. 	R
2000B894 	4 			Sender 		Sender information. 	R
2000B898 	4 			Status 		Information. 			R
2000B89C 	4 			Config	 	Settings. 				RW
2000B8A0 	4 			Write 		Sending mail. 			W 

To read from a mailbox:
1. Read the status register until the empty flag is not set
2. Read data from the read register
3. If the lower four bits do not match the channel number desired then repeat from 1
4. The upper 28 bits are the returned data

To write to a mailbox:
1. Read the status register until the full flag is not set
2. Write the data (shifted into the upper 28 bits) combined with the channel (in the lower four bits) to the write register
  */
//=================================================================================
unsigned int MailboxWrite ( unsigned int fbinfo_addr, unsigned int channel )
{

// mailbox0 base address
    unsigned int mailbox;
    mailbox=0x2000B880;

    while(1) //wait until the Status field has a 0 in the top bit.
    {
        if((GET32(mailbox+0x18)&0x80000000)==0) break; // MAILBOX FULL:Status b[31]=0.
    }
    PUT32(mailbox+0x20,fbinfo_addr+channel);
    return(0);
}

//===================================================================================
unsigned int MailboxRead ( unsigned int channel )
{
    unsigned int ra;

// mailbox0 base address
    unsigned int mailbox;
    mailbox=0x2000B880;

   while(1)
    {
        while(1)  //wait until the Status field has a 0 in the 30th bit.
        {
            ra=GET32(mailbox+0x18); //read Status reg
            if((ra&0x40000000)==0) break; // MAILBOX EMPTY: Status b[30]=0.
        }
       
        ra=GET32(mailbox+0x00); // read the "Read" register
        
        if((ra&0xF)==channel) break; //Check the channel is the one we want, if not go back.
    }
    return(ra);
}


//------------------------------------------------------------------------
int kmain( void )
{
    unsigned int ra,rb,rc,rd,re;
	unsigned int ry,rx;

	// Request for a frame buffer from GPU: 
	// We describe the framebuffer we would like and we get a response (pointer + size) 
	// we put it at address 0x40040000 (any address with low 4bits=0)
    PUT32(0x40040000, 1024); /*  Physical Width */
    PUT32(0x40040004, 768); /*  Physical Height */
    PUT32(0x40040008, 1024); /*  Virtual Width */
    PUT32(0x4004000C, 768); /*  Virtual Height */
    PUT32(0x40040010, 0); /*  GPU - Pitch */
    PUT32(0x40040014, 8); /*  Bit Depth */
    PUT32(0x40040018, 0); /* X */
    PUT32(0x4004001C, 0); /*  Y */
    PUT32(0x40040020, 0); /*  framebuffer Pointer */
    PUT32(0x40040024, 0); /*  framebuffer Size */

	
	// the address we send to GPU =  pointer to Frame Buffer Info 
    MailboxWrite(0x40040000,1); // Write to mailbox 1.

	//  read response from mailbox0, channel1
	// if GPU  agrees to our request it sends us back a 0
    MailboxRead(1);

	// get address of frame buffer defined by GPU
    rb=GET32(0x40040020);

	// get size of frame buffer
	rc =GET32(0x40040024);

	// clear buffer
	for(ra=0; ra<=rc;ra++) PUT8(ra+rb, 0);

//display random pixels on screen
 

   ra=0;
	while(1) { // forever

		while(1) { 
    ra = rand(ra); // generate a random 32bits number
 
	// let the low 16bits be the row of the pixel
	rx = ra & 0xFFFF;
	// and the high 16bits  the culumn
	ry = (ra & 0xFFFF0000)>>16;  // 
    if ((rx<=1024) & (ry<=768)) break; // if not in range, try other number
			}

	// compute the position of pixel in buffer
   rc = rb + (ry * 1024) + rx;

	// generate new random 32bits number
   ra = rand(ra); 

	// draw the pixel (put value in buffer)
	PUT8(rc,ra); // the low byte of random = pixel's color

	// wait a moment
	for(rd=0;rd<1500;rd++) dummy(rd);

	}
 
    return(0);
}

