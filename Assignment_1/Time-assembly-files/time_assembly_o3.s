	.file	"time.c"
	.text
	.p2align 4
	.globl	timespec_diff
	.type	timespec_diff, @function
timespec_diff:
.LFB11:
	.cfi_startproc
	movq	(%rsi), %rax
	movq	8(%rsi), %rcx
	subq	(%rdi), %rax
	subq	8(%rdi), %rcx
	jns	.L2
	subq	$1, %rax
	addq	$1000000000, %rcx
.L2:
	movq	%rax, %xmm0
	movq	%rcx, %xmm1
	punpcklqdq	%xmm1, %xmm0
	movups	%xmm0, (%rdx)
	ret
	.cfi_endproc
.LFE11:
	.size	timespec_diff, .-timespec_diff
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Sum: %lu\n"
.LC1:
	.string	"Time needed: %lis and %lins\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB12:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movabsq	$499999999500000000, %rbx
	subq	$32, %rsp
	.cfi_def_cfa_offset 48
	.p2align 4,,10
	.p2align 3
.L7:
	movl	$1, %esi
	movq	%rsp, %rdi
	call	timespec_get
	movq	%rbx, %rsi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	printf
	movl	$1, %esi
	leaq	16(%rsp), %rdi
	call	timespec_get
	movq	16(%rsp), %rsi
	movq	24(%rsp), %rdx
	subq	(%rsp), %rsi
	subq	8(%rsp), %rdx
	jns	.L6
	subq	$1, %rsi
	addq	$1000000000, %rdx
.L6:
	movl	$.LC1, %edi
	xorl	%eax, %eax
	call	printf
	jmp	.L7
	.cfi_endproc
.LFE12:
	.size	main, .-main
	.ident	"GCC: (GNU) 9.1.1 20190503 (Red Hat 9.1.1-1)"
	.section	.note.GNU-stack,"",@progbits
