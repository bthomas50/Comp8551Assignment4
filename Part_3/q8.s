	.file	"q8.cpp"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	;s
	; operate()
	;
	call	_Z7operatev
	;
	; int q = operate_inline()
	;
	movl	$3, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-8(%rbp), %eax
	imull	-4(%rbp), %eax
	;
	;
	;
	movl	%eax, -12(%rbp)
	cmpl	$0, -12(%rbp)
	sete	%al
	movzbl	%al, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	_Z7operatev
	.type	_Z7operatev, @function
_Z7operatev:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$3, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-8(%rbp), %eax
	imull	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	_Z7operatev, .-_Z7operatev
	.ident	"GCC: (Ubuntu 4.9.1-16ubuntu6) 4.9.1"
	.section	.note.GNU-stack,"",@progbits
