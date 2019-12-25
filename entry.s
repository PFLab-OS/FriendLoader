// entry point of friend task
.text
	.global _start
_start:
	// friend_loader_buf[0] = 1 (as 4B)
	// to show exectuion start
	mov	w1, #1
	mov	x0, #0x7000
	movk	x0, #0x117, lsl #16
	str	w1, [x0]	

	// DEPLOY_PHYS_ADDR_END (align 16)
	mov	x0, #0xfff0
	movk	x0, #0xbf6b, lsl #16
	mov	sp, x0

	bl	main

	// after return from main()
	// friend_loader_buf[0] = 2 (as 4B)
	mov	w1, #2
	mov	x0, #0x7000
	movk	x0, #0x117, lsl #16
	str	w1, [x0]	
	// loop infinitely
loop:
	wfe
	b	loop
