// this example displays the informations echanged between ARM and GPU

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

extern void uart_init ( void );
extern void uart_send ( unsigned int );
extern unsigned int uart_recv ( void );
extern void hexstring ( unsigned int );
extern void hexstrings ( unsigned int );


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
    unsigned int ra,rb;
    unsigned int ry,rx;

    uart_init();

	//  display 12345678 (just to tell we are ok !)
    hexstring(0x12345678);


	// Request for a frame buffer from GPU: 
	// We describe the framebuffer we would like and we get a response (pointer + size) 
	// we put it at address 0x40040000 (any address with low 4bits=0)
    PUT32(0x40040000, 1024); /*  Physical Width */
    PUT32(0x40040004, 768); /*  Physical Height */
    PUT32(0x40040008, 1024); /*  Virtual Width */
    PUT32(0x4004000C, 768); /*  Virtual Height */
    PUT32(0x40040010, 0); /* GPU - Pitch */
    PUT32(0x40040014, 8); /*  Bit Depth */
    PUT32(0x40040018, 0); /*  X */
    PUT32(0x4004001C, 0); /*  Y */
    PUT32(0x40040020, 0); /* buffer Pointer: GPU response will be here */
    PUT32(0x40040024, 0); /* buffer Size: GPU response will be here */

	// display response after sending the ponter to our request
    hexstring(MailboxWrite(0x40040000,1)); // write  to mailbox0, channel1

	// display response after reading from mailbox0, channel1
	// if GPU either agrees to our request it sends us back a 0x00000001
    hexstring(MailboxRead(1));

	// display GPU pitch: number of bytes by line
    rb=GET32(0x40040010);
    hexstring(rb);

	// display the address of framebuffer defined by GPU
    rb=GET32(0x40040020);
    hexstring(rb);

	// display size of framebuffer
    rb=GET32(0x40040024);
    hexstring(rb);

 
 
    return(0);
}

