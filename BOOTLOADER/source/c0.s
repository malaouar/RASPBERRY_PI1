
.section .init
.globl _start
_start:

    ldr pc,reset
    ldr pc,undefined
    ldr pc,swi
    ldr pc,prefetch
    ldr pc,data
    ldr pc,unused
    ldr pc,irq
    ldr pc,fiq
reset:      .word reset_handler
undefined:  .word halt
swi:        .word halt
prefetch:   .word halt
data:       .word halt
unused:     .word halt
irq:        .word irq_handler
fiq:        .word halt

reset_handler:
    mov r0,#0x8000
    mov r1,#0x0000
    ldmia r0!,{r2-r9}
    stmia r1!,{r2-r9}
    ldmia r0,{r2-r9}
    stmia r1,{r2-r9}


    ;@ (IRQ_MODE|FIQ_DIS|IRQ_DIS)
    msr cpsr_c,#0xD2
    mov sp,#0x4000


    ;@ (SVC_MODE|FIQ_DIS|IRQ_DIS)
    msr cpsr_c,#0xD3
    mov sp,#0x8000

    b begin

 .space 0x200000-4,0  

begin:
	@ call C code
    bl kmain
@==============================
.globl halt
halt:
	wfe
	b halt
@========================================
irq_handler:
	msr cpsr_c,#0xD3	@ (SVC_MODE|FIQ_DIS|IRQ_DIS)
	bl xmodem

@==================================
.globl enable_irq
enable_irq:
	cpsie i
	bx lr

@===========================

.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl PUT16
PUT16:
    strh r1,[r0]
    bx lr

.globl PUT8
PUT8:
    strb r1,[r0]
    bx lr
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.globl dummy
dummy:
    bx lr

.globl BRANCHTO
BRANCHTO:
    bx r0


@============

