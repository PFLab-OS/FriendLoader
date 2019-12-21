.text
	.align 2
	.global _start

// store 0xabcd as 4B to friend_loader_buf[0]
_start:
	mov	x0, #0x7000
	movk	x0, #0x0117, lsl #16
	movk	x0, #0x0000, lsl #48
	mov	w1, #0xabcd
	str	w1, [x0]
loop:
	wfe
	b loop
