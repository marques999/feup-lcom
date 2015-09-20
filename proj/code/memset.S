/**
 * KBC status
 */
#define OBF 1
#define IBF 2
#define AUX 64

/**
 * KBC registers
 */
#define STAT_REG 0x64
#define OUT_BUF 0x60
#define TWO_BYTE 0xe0

.text

.global _memset16

_memset16:
	pushl %ebp
	movl %esp, %ebp
	subl $12, %esp
	movl %ebx, (%esp)
	movl 8(%ebp), %edx
	movl %esi, 4(%esp)
	movzwl 12(%ebp), %ebx
	movl %edi, 8(%esp)
	movl 16(%ebp), %esi
	movl %esi, %ecx
	movw %bx, %ax
	movl %edx, %edi
	rep stosw
	movl (%esp), %ebx
	movl 4(%esp), %esi
	movl 8(%esp), %edi
	movl %ebp, %esp
	popl %ebp
	ret
	.p2align 4,,15

.global _kbd_read

_kbd_read:

	xor %edx, %edx

init_cycle:

	pushl %ecx
	movl $8, %ecx

read_status:

	xor %eax, %eax
	inb $STAT_REG
	test $OBF, %al
	loopz read_status
	test $AUX, %al
	loopnz read_status

get_scancode:

	popl %ecx
	inb $OUT_BUF
	ret

.end
