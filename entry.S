// entry point of friend task
.text
	.global _start
_start:

// sp = DEPLOY_PHYS_ADDR + 31MB (align 16)
#ifdef TASK1
	mov	x0, #0xbc70
	movk	x0, #0x0000, lsl #16
#elif defined TASK2
	mov	x0, #0xbe70
	movk	x0, #0x0000, lsl #16
#endif
	mov	sp, x0

	// x0 = friend_loader_buf (int *)
#ifdef TASK1
	mov	x0, #0x7000
	movk	x0, #0x117, lsl #16
#elif defined TASK2
	mov	x0, #0x7800
	movk	x0, #0x117, lsl #16
#endif

	// Memo of initial values of some registers

	// CurrentEL, Current Exception Level
	// [31:0] = 0x00000008
	// CurrentEL.EL = EL2

	// CNTFRQ_EL0, Counter-timer Frequency register
	// [31:0] = 0x001d4c00
	// Clock frequency = 1.92Mz

	// ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0
	// (cpu 7)
	// [63:32] = 0
	// [31:0] = 0x00101122
	// 4KB, 64KB, 16KB memory translation granule is supported
	// (but probably in cpu 3 (Cortex-A53) 16KB is not supported)
	// Physical address range: 40-bit / 1TB is supported
	// (it seems to be common to cpu 3)

	// ECTLR_EL1 (S3_1_C15_C2_1), Extended Control Register
	// [31:0] = 0x000005c0
	// ECTLR_EL1.{MMUPF, L2PF, L1PF} = 1
	// ECTLR_EL1.SMPEN = 1

	// friend_loader_buf[0] = 1
	// to show exectuion start
	mov	w1, #1
	str	w1, [x0]

	// set TCR_EL2
	// friend_loader_buf[1] = TCR_EL2[31:0]
	// TCR_EL2 is Translation Control Register, EL2
	// TCR_EL2.PS = 4GB
	// TCR_EL2.TG0 = 4KB
	// TCR_EL2.SH0 = Inner sharable
	// TCR_EL2.ORGN0 = Normal memory, 
	// Outer Write-Back Write-Allocate Cacheable
	// TCR_EL2.IRGN0 = Normal memory,
	// Inner Write-Back Write-Allocate Cacheable
	// TCR_EL2.T0SZ = 32
	ldr	x1, =0x80803520
	msr	tcr_el2, x1
	dsb	sy
	isb
	mrs	x1, tcr_el2
	str	w1, [x0, #4]

	// set MAIR_EL2
	// friend_loader_buf[2] = MAIR_EL2[31:0]
	// MAIR_EL2 is Memory Attribute Indirection Register
	// MAIR_EL2.Attr3 = 
	// Normal memory, Outer Write-Back Non-transient,
	// Read: Allocate, Write: Allocate
	// Normal memory, Inner Write-Back Non-transient,
	// Read: Allocate, Write: Allocate
	ldr	x1, =0xff000000
	msr	mair_el2, x1
	dsb 	sy
	isb
	mrs	x1, mair_el2
	str	w1, [x0, #8]
	
	// set TTBR0_EL2
	// friend_loader_buf[3] = TTBR0_EL2[31:0]
	// TTBR0_EL2 is Translation Table Base Register 0 (EL2)
	ldr	x1, = level1_table
	msr	ttbr0_el2, x1
	dsb	sy
	isb
	mrs	x1, ttbr0_el2
	str	w1, [x0, #12]

	// set SCTLR_EL2
	// friend_loader[4] = SCTLR_EL2 (0x30c51834)
	// SCTLR_EL2 is System Control Register, EL2
	// enable data cache and instruction cache
	mrs	x1, sctlr_el2
	orr	x1, x1, #(0x1 << 2)	// data cache
	orr	x1, x1, #(0x1 << 12)	// instruction cache
	// orr	x1, x1, #0x1		// MMU
	msr	sctlr_el2, x1
	dsb	sy
	isb
	mrs	x1, sctlr_el2
	str	w1, [x0, #16]

	// jump to startup (in fllib) to jump to main
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

// cpu power off to replug
// mimic of /arch/arm64/kernel/psci.c cpu_psci_cpu_die()
	// x0 = PSCI_0_2_FN_CPU_OFF
	mov	x0, #0x0002
	movk	x0, #0x8400, lsl #16
	// x1 = state = PSCI_POWER_STATE_TYPE_POWER_DOWN <<
	// 		PSCI_0_2_POWER_STATE_TYPE_SHIFT
	mov	x1, #0
	movk	x1, #1, lsl #16
	// x2, x3 = 0
	mov	x2, #0
	mov	x3, #0
	smc	#0

loop:
	wfi
	b	loop

// Page Tables

.macro PUT_64B		high, low
	.word \low
	.word \high
.endm

// 0x2 means "descriptor type is table"
// 0x1 means "descriptor is valid"
.macro TABLE_ENTRY	PA, ATTR
	PUT_64B	\ATTR, (\PA) + 0x3
.endm

// 0x1 means "block"
.macro BLOCK_2MB	PA, ATTR_HI, ATTR_LO
	PUT_64B	\ATTR_HI, ((\PA & 0xFFE00000) | \ATTR_LO | 0x1)
.endm

// 12 for 4KB granule
.align 12
level1_table:
	// ATTR = 0 means
	// NSTable = 0, APTable = 0, XNTable = 0, PXNTable = 0
	TABLE_ENTRY	level2_table0, 0
	TABLE_ENTRY	level2_table0, 0
	TABLE_ENTRY	level2_table2, 0
	TABLE_ENTRY	level2_table2, 0

.align 12
level2_table0:
	.set		ADDR, 0x0
.rept 0x200
	// ATTR_HI = 0 means XN = 0
	// ATTR_LO = 0x74c means
	// AF = 1
	// SH = Inner sharable
	// AP = R/W from higher level and R/W from EL0
	// NS = non-secure
	// AttrIndx = 3
	BLOCK_2MB	(ADDR << 20), 0, 0x74c	
	.set		ADDR, ADDR+2
.endr

.align 12
level2_table2:
	.set		ADDR, 0x0
.rept 0x200
	// ATTR_HI = 0 means XN = 0
	// ATTR_LO = 0x74c means
	// AF = 1
	// SH = Inner sharable
	// AP = R/W from higher level and R/W from EL0
	// NS = non-secure
	// AttrIndx = 3
	BLOCK_2MB	(ADDR << 20), 0, 0x74c	
	.set		ADDR, ADDR+2
.endr

