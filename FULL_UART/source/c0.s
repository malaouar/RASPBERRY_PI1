
.section .init
.globl _start
_start:

	mov sp,#0x8000	/* Set the stack pointer to 0x8000 */

	bl kmain	 	/* call C code "kernel main" */

	/* if main returns  */
halt:
	wfe		/* wfe put the CPU in a low-power state while waiting for event: IRQ, FIQ, Imprecise abort, or Debug entry request. */
	b halt		/* if main returns loop forever */

/* ------------------------------------------------ */
/* some functions to use with asm or  C code */

/* function to put contents of r1 in memory location pointed by r0 */
.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

/* function to put contents of memory location pointed by r0 in r0*/
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

/* a NOP equivalent */
.globl dummy
dummy:
    bx lr
