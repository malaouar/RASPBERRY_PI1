
.section .init
.globl _start
_start:
    ldr pc,reset_handler	@ load PC with contents of address "reset_handler"
				@ PC = [PC + offset]
    ldr pc,undefined_handler
    ldr pc,swi_handler
    ldr pc,prefetch_handler		
    ldr pc,data_handler	
    ldr pc,unused_handler
    ldr pc,irq_handler
    ldr pc,fiq_handler
reset_handler:      .word reset		@ contents of this address = address of code labeled "reset". (word=4bytes)
undefined_handler:  .word halt
swi_handler:        .word halt
prefetch_handler:   .word halt
data_handler:       .word halt
unused_handler:     .word halt
irq_handler:        .word irq
fiq_handler:        .word halt

reset:		@ first we move vectors table from address 0x8000 to 0x0000
    mov r0,#0x8000
    mov r1,#0x0000
    ldmia r0!,{r2-r9}		@ load multiple (IA= increment after): load the 8 registers r2 to R9 with contents of adresses
				@ pointed by r0.  {r2,r3,r4,r5,r6,r7,r8,r9} = {r2-r9}
    stmia r1!,{r2-r9}		@ store multiple : store the 8 registers r2 to R9 in adresses pointed by r1
    ldmia r0!,{r2-r9}		@ repeat: cause we have 16 words between labels " _start" and "reset".
    stmia r1!,{r2-r9}

/* the exclamation point after the register modifies that register to point to the next word after the last one we loaded so we read 8 words or 32 bytes 
   at address 0x8000 so the last thing it does (increment after the load) is add 0x20 and save so r0 is now 0x8020. */


	/* Set the stack pointer for each mode */

    ;@ IRQ_MODE:
    mov r0,#0xD2	@ IRQ_MODE , IRQ and FIQ disabled
    msr cpsr_c,r0
    mov sp,#0x8000

    ;@ FIQ_MODE:
    mov r0,#0xD1	@ FIQ_MODE , IRQ and FIQ disabled
    msr cpsr_c,r0
    mov sp,#0x4000

    ;@ we start in SVC_MODE:
    mov r0,#0xD3	@ SVC_MODE , IRQ and FIQ disabled
    msr cpsr_c,r0
    mov sp,#0x8000000


    @ call C code:
    bl kmain

	/* if main returns  */
halt:
	wfe		/* wfe put the CPU in a low-power state while waiting for event: IRQ, FIQ, Imprecise abort, or Debug entry request. */
	b halt		/*  loop forever */


/* ------------------------------------------------ */
.globl enable_irq	@ clear Irq control bit (bit7) in cpsr 
enable_irq:
    cpsie i	@ Change processor state: Enable specified interrupts (i = Irq, f= Fiq)
    bx lr

    /* cpsie is equivalent to this code wich  clears Irq control bit (bit7) in cpsr :
    @mrs r0,cpsr		@ read cpsr
    @bic r0,r0,#0x80	@Bit Clear ( Rd := Rn AND NOT Operand2)
    @msr cpsr_c,r0	@ save new value in cpsr*/


/* ------------------------------------------------ */
.globl enable_fiq	@ clear Frq control bit (bit8) in cpsr 
enable_fiq:
    cpsie f
    bx lr

/* ------------------------------------------------ */
.globl enable_int	@ enable irq and fiq 
enable_int:
    cpsie if
    bx lr

/* ------------------------------------------------ */
.globl disable_irq	@ set Irq control bit (bit7) in cpsr 
disable_irq:
    cpsid i	
    bx lr
/* ------------------------------------------------ */
.globl disable_fiq	@ set Fiq control bit (bit8) in cpsr 
disable_fiq:
    cpsid i	
    bx lr
/* ------------------------------------------------ */
.globl disable_int	@ disable irq and fiq 
disable_int:
    cpsid i	
    bx lr


/* ------------------------------------------------ */
/* two things that must be done by the software handler, are:
    - The CPSR must be restored from SPSR_<mode>.
    - The PC must be restored from LR_<mode>.
  where <mode> is the exception mode in which the processor currently operates.

 FIQ interrupts are automatically disabled by the processor upon entry to FIQ mode. The software could enable them again, but this is not typical 
   practice. 
   servicing an FIQ disables IRQ interrupts. Once the FIQ handler exits, the IRQ can then be serviced. */ 
      
@=============================================
@ IRQ interrupt handler:
irq:

	push {r0-r12, lr}  @ save r0 to r12 and lr in stack: {r0-r12, lr} = {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr})

	/*  NOT working yet !!
        @ first read "irq2 pending" register to see if gpio_int[0] is fired
	@ldr     r1, =0x2000B000
	@ldr     r0, [r1, #0x208]	@ IRQ2_PENDG
	@tst     r0, #1<<17		@ gpio_int[0] set? (tst -> Rn AND Operand2)
	@bne     exit			@ no, exit */

	@ bl wait 			@ anti rebonds

	@ else clear interrupt by writing a 1 in "Pin Event Detect Status" register: 
	ldr r0, =0x20200000		@ gpio base
	mov r1, #1<<18
	str r1, [r0, #0x40]		@ GPEDS0: GPIO Pin Event Detect Status
	
	@ call C routine to  blink the led:
	@bl ...

exit:
	pop {r0-r12, lr}	@ restore regs
	subs	pc, lr, #4	@ pc = lr - 4 (befor entering IRQ or FIQ handler, PC points to 2 words in pipeline)
				@ The ‘s’ at the end of the opcode means special instruction when the destination is the pc
				@ It sets the PC and restores the CPSR from SPSR in one instruction. 

@=============================================
/* some functions to use with asm or  C code */

/* simple delay  */
.globl wait
wait:
	@ push r2
	mov r2,#0x5F0000
wait1:
	sub r2,#1
	cmp r2,#0
	bne wait1
	@ pop r2
	mov pc, lr	@ return
	

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
