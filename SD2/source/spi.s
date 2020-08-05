.section .text
@===================================================
@ gpio49= MOSI, gpio50= MISO, gpio53= CS, gpio48= CLK
@==================================================
.globl spi_init
spi_init:
	push {r0,r1,lr}
	ldr r0, =0x20200000		  @ GPIO base address

	ldr r1, [r0, #16]		    @ Read GPFSEL4 (to change gpio 48, 49 without touch others)
  ldr r2, =~(7<<24|7<<27)	@ r2= NOT (......)
	and r1, r2				      @ 
	ldr r2, =(1<<24|1<<27)	@ r2=(..xxxxxx....)
	orr r1, r2				      @   gpio48,gpio49 outputs
	str r1, [r0, #16]		    @ save in GPFSEL4

	ldr r1, [r0, #20]		    @ Read GPFSEL5 (gpio 50, 51, 52, 53)
	ldr r2, =~(7<<0|7<<3|7<<6|7<<9)	  @ r2= NOT (......)
	and r1, r2				      @ 
	ldr r2, =1<<9     	    @ gpio53 output, gpio50, 51, 52 inputs
	orr r1, r2				      @   
	str r1, [r0, #20]		    @ save in GPFSEL5
	
	mov r1, #1<<21		      @ to set or clear gpio53 (CS)
	str r1, [r0, #32]       @  set CS (in GPSET1)
	
	pop {r0,r1,pc}

@============================================
@ send/receive (MSB first) a byte in r0
@============================================
.globl rx
rx:
	mov r0, #0xFF
.globl tx
tx:
	push {r1-r5, lr}	    @ save regs and return address
	ldr r2, =0x20200000	  @ GPIO base address
	mov r5, #1<<16		    @ to set or clear gpio48 (CLK)	
	mov r4, #1<<17		    @ to set or clear gpio49 (MOSI)
	mov r1, #8			      @ 8 bits counter
again:
	bl delay			        @ wait befor sending next bit
	tst r0, #0x80
  streq  r4, [r2, #44]  @ 7th bit=0 then clear MOSI gpio49 (in GPCLR1)
	strne  r4, [r2, #32]  @ 7th bit=1 then set MOSI gpio49 (in GPSET1)
	lsl r0, #1  		      @ one shift left
	
	@ read MISO befor pulsing CLK !!
	ldr r3, [r2, #0x38]   @ read MISO gpio50 (in GPLEV1)
	tst r3, #1<<18        @ is MISO Hi or Lo? 
	addne r0, #1		      @ if high, bit0 of r0=1 else let it 0

	@ Pulse CLK
  str  r5, [r2, #32]    @  set CLK
	bl delay
	str  r5, [r2, #44]    @  clear CLK
	
	subs r1, #1			      @ next bit: dec r1 and test
	bne again			        @ r1=0? No ,continue sending/receiving
	and r0, #0xFF          @ save only low byte
	pop {r1-r5, pc}		    @ Yes, restore regs and pc from lr
                        @ return with read value in r0
@===================================================	
delay:
	push {r0,lr}	
	mov r0, #1
wait1:
	@bl wait			@ wait ??us
	subs r0, #1
  bne wait1
	pop {r0,pc}
@====================================================	

