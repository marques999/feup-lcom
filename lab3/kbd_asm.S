#include "i8042.h"

.data

print_makecode:
	.asciz "\tMakecode: 0x%X\n"
print_breakcode:
	.asciz "\tBreakcode: 0x%X\n"

.text

.global _kbd_int_handler_asm

_kbd_int_handler_asm:

	xor %edx, %edx

read_status:
	xor %eax, %eax
	# get buffer status from 0x64
	inb $STAT_REG
	# check whether OUT_BUF is full
	test $OBF, %al
	# OUT_BUF is empty, wait for valid data input
	jz read_status

get_scancode:
	# get scancode from 0x60
	inb $OUT_BUF
	# has user pressed the ESC key?
	movb %al, %dl
	cmp $KEY_ESC, %dl
	# return "KEY_ESC"
	je exit
	# has user entered a special key?
	cmp $TWO_BYTE, %dl
	jne process

	# process special key
	# prepare register for the LSB of the 2-byte scancode
	shl $8, %dx
	# read the LSB of the scancode
	jmp read_status

# normal key pressed

process:
	pushl %edx
	test $BREAK_BIT, %edx
	jnz breakcode
	lea print_makecode, %eax
	pushl %eax
	jmp print

breakcode:
	lea print_breakcode, %eax
	pushl %eax

print:
	call _printf
	add $8, %esp

exit:
	mov %edx, %eax
	ret

.end
