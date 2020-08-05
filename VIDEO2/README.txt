
This example displays a photo on screen 640x480  32bits colors

Raspberry Pi video frame buffer basics.
simple version of dwelch video01

- we send the pointer of our request to GPU to give as a pointer to a framebuffer
- the GPU answers 
- we display the address of the given pointer in hex format using UART

we write our message to memory and then pass the memory address of our message to the GPU, via the mailbox.

Writing to the mailbox is done as follows:

    - Read the status register (0x2000B898).
    - Check if bit 31 (0x80000000) is set.
    - If not, go back to step 1 else proceed to next step.
    - The data to be written, has to be left shifted by 4 bits. This converts our address to 28 bits.
    The lower 4 bits will contain the mailbox channel number, combine this with 28bit address by performing a logical OR with the 28 bit address from step 4.
    - Write the data to the Write register (0x2000B8A0).

We read from the mailbox as follows:

    - Read the status (0x2000B898) register.
    - Check if bit 30 (0x40000000) is set.
    - If not, go back to step 1.
    - Read the  "Read" register (0x2000B880).
    The lower 4 bits will contain the mailbox channel number of the mailbox that responded. Check that this matches the mailbox we are interested in. If not, then go back to step 1.
    The upper 28 bit should be zero.

Channel 1 of mailbox 0 is used.

Request data: The 28 most significant bits of the address of a buffer
Response data: 0x0000000 (0x00000001 including the channel identifier)


