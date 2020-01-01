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

	// friend_loader_buf[1] = current EL (expected to be 8 (means EL2))
	mrs	x1, currentel
	str	w1, [x0, #4]

	// friend_loader_buf[2] = frequency (expected to be 1.92Mz)
	// In arm reference manual,
	// value of cntfrq_el0 is undefined and should be set,
	// but 1.92MHz resembles 19.2Mz (SoC manual's clock value)
	// and in all execution the value is same
	// so I suppose that frequency is 1.92Mz
	mrs	x1, cntfrq_el0
	str	w1, [x0, #8]

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
