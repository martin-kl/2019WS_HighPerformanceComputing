	.file	"time.c"
	.text
	.globl	timespec_diff
	.type	timespec_diff, @function
timespec_diff:
.LFB11:
	.cfi_startproc
	movq	8(%rdi), %rax
	cmpq	%rax, 8(%rsi)
	js	.L4
	movq	(%rsi), %rax
	subq	(%rdi), %rax
	movq	%rax, (%rdx)
	movq	8(%rsi), %rax
	subq	8(%rdi), %rax
	movq	%rax, 8(%rdx)
	ret
.L4:
	movq	(%rsi), %rax
	subq	(%rdi), %rax
	subq	$1, %rax
	movq	%rax, (%rdx)
	movq	8(%rsi), %rax
	subq	8(%rdi), %rax
	addq	$1000000000, %rax
	movq	%rax, 8(%rdx)
	ret
	.cfi_endproc
.LFE11:
	.size	timespec_diff, .-timespec_diff
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Sum: %lu\n"
.LC1:
	.string	"Time needed: %lis and %lins\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB12:
	.cfi_startproc
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
	jmp	.L7
.L10:
	movabsq	$499999999500000000, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$1, %esi
	leaq	16(%rsp), %rdi
	call	timespec_get
	movq	%rsp, %rdx
	leaq	16(%rsp), %rsi
	leaq	32(%rsp), %rdi
	call	timespec_diff
	movq	8(%rsp), %rdx
	movq	(%rsp), %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
.L7:
	movl	$1, %esi
	leaq	32(%rsp), %rdi
	call	timespec_get
	movl	$1000000000, %eax
.L6:
	subl	$1, %eax
	jne	.L6
	jmp	.L10
	.cfi_endproc
.LFE12:
	.size	main, .-main
	.ident	"GCC: (GNU) 9.1.1 20190503 (Red Hat 9.1.1-1)"
	.section	.note.GNU-stack,"",@progbits
