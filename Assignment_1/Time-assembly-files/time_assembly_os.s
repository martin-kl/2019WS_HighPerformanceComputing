	.file	"time.c"
	.text
	.globl	timespec_diff
	.type	timespec_diff, @function
timespec_diff:
.LFB0:
	.cfi_startproc
	movq	(%rsi), %rcx
	movq	8(%rsi), %rax
	subq	(%rdi), %rcx
	subq	8(%rdi), %rax
	jns	.L2
	decq	%rcx
	addq	$1000000000, %rax
	movq	%rcx, (%rdx)
	jmp	.L4
.L2:
	movq	%rcx, (%rdx)
.L4:
	movq	%rax, 8(%rdx)
	ret
	.cfi_endproc
.LFE0:
	.size	timespec_diff, .-timespec_diff
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Sum: %lu\n"
.LC1:
	.string	"Time needed: %lis and %lins\n"
	.section	.text.startup,"ax",@progbits
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
.L8:
	movl	$1, %esi
	movq	%rsp, %rdi
	call	timespec_get
	movl	$1000000000, %eax
.L7:
	decl	%eax
	jne	.L7
	movabsq	$499999999500000000, %rsi
	movl	$.LC0, %edi
	xorl	%eax, %eax
	call	printf
	leaq	16(%rsp), %rdi
	movl	$1, %esi
	call	timespec_get
	leaq	32(%rsp), %rdx
	leaq	16(%rsp), %rsi
	movq	%rsp, %rdi
	call	timespec_diff
	movq	40(%rsp), %rdx
	movq	32(%rsp), %rsi
	xorl	%eax, %eax
	movl	$.LC1, %edi
	call	printf
	jmp	.L8
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 9.1.1 20190503 (Red Hat 9.1.1-1)"
	.section	.note.GNU-stack,"",@progbits
