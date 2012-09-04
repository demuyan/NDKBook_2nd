	.arch armv7-a
	.eabi_attribute 27, 3
	.fpu neon
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.file	"multi_neon.c"
	.text
	.align	2
	.global	multi_neon
	.type	multi_neon, %function
multi_neon:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r3, #0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
	ble	.L1
	ubfx	r9, r0, #2, #2
	mov	sl, r0
	rsb	r9, r9, #0
	and	r9, r9, #3
	cmp	r9, r3
	movcs	r9, r3
	cmp	r9, #0
	beq	.L8
	mov	r8, r1
	mov	r5, r2
	mov	ip, #0
	@
        @ (1)128bit境界前の演算を行う(32bit単位)
	@
.L4:
	ldr	r4, [sl], #4
	add	ip, ip, #1
	ldr	r6, [r8], #4
	cmp	ip, r9
	mov	r7, sl
	mul	r4, r6, r4
	mov	r6, r8
	str	r4, [r5], #4
	mov	r4, r5
	bcc	.L4
	cmp	r3, r9
	beq	.L1
.L3:
	rsb	fp, r9, r3
	mov	r8, fp, lsr #2
	movs	sl, r8, asl #2
	beq	.L7
	mov	r9, r9, asl #2
	mov	r5, #0
	add	r0, r0, r9
	add	r1, r1, r9
	add	r9, r2, r9
	@
        @ (2)128bit(Qレジスタ)のデータを一括して扱う
	@
.L6:
	vld1.32	{q9}, [r1]!
	vldmia	r0!, {d16-d17}
	vmul.i32	q8, q9, q8
	add	r5, r5, #1
	cmp	r5, r8
	vst1.32	{q8}, [r9]!
	bcc	.L6
	@
	cmp	fp, sl
	mov	r2, sl, asl #2
	add	r7, r7, r2
	add	r6, r6, r2
	add	r4, r4, r2
	add	ip, ip, sl
	beq	.L1
	@
        @ (3)128bit境界後のデータを扱う(32bit単位)
	@
.L7:
	ldr	r2, [r7], #4
	add	ip, ip, #1
	ldr	r1, [r6], #4
	cmp	r3, ip
	mul	r2, r1, r2
	str	r2, [r4], #4
	bgt	.L7
.L1:
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp}
	bx	lr
.L8:
	mov	r4, r2
	mov	r6, r1
	mov	r7, r0
	mov	ip, r9
	b	.L3
	.size	multi_neon, .-multi_neon
	.align	2
	.global	Java_com_example_neon_MainActivity_multineon
	.type	Java_com_example_neon_MainActivity_multineon, %function
Java_com_example_neon_MainActivity_multineon:
	@ args = 0, pretend = 0, frame = 360016
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
	sub	sp, sp, #356352
	sub	sp, sp, #3664
	sub	sp, sp, #4
	add	r0, sp, #16
	and	r5, r0, #15
	movs	r5, r5, lsr #2
	beq	.L20
	add	r1, sp, #119808
	mov	r2, #0
	add	r1, r1, #208
	mov	ip, #16
	mov	r3, r2
.L15:
	add	r3, r3, #1
	str	ip, [r0, r2]
	cmp	r5, r3
	rsb	r4, r3, #29952
	mov	lr, ip, asl #1
	add	r4, r4, #48
	str	lr, [r1, r2]
	add	ip, ip, #1
	add	r2, r2, #4
	bhi	.L15
.L14:
	rsb	r8, r5, #29952
	add	r8, r8, #48
	mov	r6, r8, lsr #2
	movs	r7, r6, asl #2
	beq	.L16
	mov	ip, r5, asl #2
	add	fp, sp, #356352
	add	r5, sp, #356352
	add	fp, fp, #3664
	add	r5, r5, #3664
	movw	r9, #33200
	movw	sl, #33204
	movt	r9, 65530
	movt	sl, 65530
	movw	lr, #33208
	movt	lr, 65530
	movw	r2, #33212
	vmov.i32	q11, #4  @ v4si
	movt	r2, 65530
	vmov.i32	q12, #16  @ v4si
	str	r3, [r5, r9]
	vmov.i32	q13, #2  @ v4si
	add	r5, r3, #1
	add	r9, r3, #2
	str	r5, [fp, sl]
	str	r9, [fp, lr]
	add	lr, r0, ip
	add	ip, r1, ip
	add	r5, r3, #3
	str	r5, [fp, r2]
	mov	r2, #0
	vldmia	sp, {d16-d17}
.L17:
	vadd.i32	q9, q8, q12
	add	r2, r2, #1
	cmp	r2, r6
	vstmia	lr!, {d18-d19}
	vmul.i32	q10, q9, q13
	vadd.i32	q8, q8, q11
	vst1.32	{q10}, [ip]!
	bcc	.L17
	cmp	r8, r7
	add	r3, r3, r7
	rsb	r4, r7, r4
	beq	.L18
.L16:
	add	r2, r3, #16
	mov	r4, r4, asl #2
	mov	r3, r3, asl #2
	mov	ip, r2, asl #1
	add	r5, r0, r3
	add	lr, r1, r3
	mov	r3, #0
.L19:
	str	r2, [r5, r3]
	add	r2, r2, #1
	str	ip, [lr, r3]
	add	r3, r3, #4
	cmp	r3, r4
	add	ip, ip, #2
	bne	.L19
.L18:
	add	lr, sp, #239616
	movw	r3, #30000
	add	lr, lr, #592
	sub	r2, lr, #192
	bl	multi(PLT)
	add	sp, sp, #596
	add	sp, sp, #97280
	add	sp, sp, #262144
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
.L20:
	add	r1, sp, #119808
	movw	r4, #30000
	add	r1, r1, #208
	mov	r3, r5
	b	.L14
	.size	Java_com_example_neon_MainActivity_multineon, .-Java_com_example_neon_MainActivity_multineon
	.ident	"GCC: (GNU) 4.6.x-google 20120106 (prerelease)"
	.section	.note.GNU-stack,"",%progbits
