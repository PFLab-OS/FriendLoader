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

	// sp = 0x1177800 (middle of friend_loader_buf)
	mov	x0, #0x7800
	movk	x0, #0x117, lsl #16
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
