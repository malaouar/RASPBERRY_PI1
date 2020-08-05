
.section .init
.globl _start
_start:
    ldr pc,reset_handler	@ load PC with contents of address "reset_handler"
				@ PC = [PC + offset]

reset_handler:      .word reset		@ contents of this address = address of code labeled "reset". (word=4bytes)

reset:		@ first we move vectors table from address 0x8000 to 0x0000
 
    @ stack pointer 
    mov sp,#0x00100000    

    @ call C code:
    bl kmain

	/* if main returns  */
halt:
	wfe		/* wfe put the CPU in a low-power state while waiting for event: IRQ, FIQ, Imprecise abort, or Debug entry request. */
	b halt		/*  loop forever */

@=============================================
/* some functions to use with asm or  C code */	

/* function to put contents of r1 in memory location pointed by r0 */
.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

/* function to read contents of memory location pointed by r0 in r0*/
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

/* a NOP equivalent ? */
.globl dummy
dummy:
    bx lr


