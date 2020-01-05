// entry point of friend task
.text
	.global _start
_start:
	// x0 = friend_loader_buf (int *)
#ifdef TASK1
	mov	x0, #0x7000
	movk	x0, #0x117, lsl #16
#elif defined TASK2
	mov	x0, #0x7800
	movk	x0, #0x117, lsl #16
#endif

	// friend_loader_buf[0] = 1
	// to show exectuion start
	mov	w1, #1
	str	w1, [x0]

	// friend_loader_buf[1] = current EL (expected to be 0x8 (means EL2))
	mrs	x1, currentel
	str	w1, [x0, #4]

	// friend_loader_buf[2] = frequency
	// (expected to be 0x1d4c00 (means 1.92Mz))
	// In arm reference manual,
	// value of cntfrq_el0 is undefined and should be set,
	// but 1.92MHz resembles 19.2Mz (SoC manual's clock value)
	// and in all execution the value is same
	// so I suppose that frequency is 1.92Mz
	mrs	x1, cntfrq_el0
	str	w1, [x0, #8]

	// friend_loader[3] = ECTRL_EL1[31:0] (expected to be 0x5c0)
	// SMP is already enabled (why?), so I don't touch this register
	mrs	x1, s3_1_c15_c2_1
	str	w1, [x0, #12]

	// friend_loader[4] = System Control Register (EL2) [31:0]
	// expected to be 0x30c50830
	// which means that data cache and instruction cache are disabled
	mrs	x1, sctlr_el2
	str	w1, [x0, #16]
	// Enable data cache and instruction cache
	orr	x1, x1, #(0x1 << 2)	// data cache
	orr	x1, x1, #(0x1 << 12)	// instruction cache
	msr	sctlr_el2, x1

	// friend_loader[5] = System Control Register (EL2)
	// expected to be 0x30c51834
	// which means that data cache and instruction cache are enabled
	mrs	x1, sctlr_el2
	str	w1, [x0, #20]

	// sp = DEPLOY_PHYS_ADDR + 31MB (align 16)
#ifdef TASK1
	mov	x0, #0xbc70
	movk	x0, #0x0000, lsl #16
#elif defined TASK2
	mov	x0, #0xbe70
	movk	x0, #0x0000, lsl #16
#endif
	mov	sp, x0

	// jump to startup (in fllib) to jump main
	bl	startup

	// friend_loader_buf[0] = 2 (as 4B)
	// to show execution end 
	mov	w1, #2
#ifdef TASK1
	mov	x0, #0x7000
	movk	x0, #0x117, lsl #16
#else
	mov	x0, #0x7800
	movk	x0, #0x117, lsl #16
#endif
	str	w1, [x0]	

	// loop infinitely
loop:
	wfi
	b	loop
