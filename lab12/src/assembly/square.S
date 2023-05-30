.section .text
.globl square
.type square, @function
square:
	# The above lines declares a function that can be accessed from C code.
	# When you're implementing your own function, you make a new file in
	# this folder, copy the above 4 lines, and replace all
	# "your_function_name" to your function's name.

	# Then you can add code after the label on line 4.

	# The rest of things work the same as in Venus, except that the directive
	# ".asciiz" is removed. Use ".string" instead.

	# Also, please make sure that each assembly function has a filename with 
	# extension name ".S", not ".s". Otherwise it will not be recognized
	# by the riscv-nuclei-elf-as and will throw an error.

	# You're free to remove this function after you've read the above.
	addi sp, sp, -4;
sw ra, 0(sp);
	li a0, 70;
	li a1, 40;
	li a2, 90;
	li a3, 60;
	li a4, 0xff00;
	jal LCD_DrawRectangle
lw ra, 0(sp);
addi sp, sp, 4;
ret;