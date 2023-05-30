.section .text
.globl circle
.type circle, @function
circle:

addi sp, sp, -4;
sw ra, 0(sp);

li a0, 110; # x0
li a1, 50; # y0
li a2, 10; # r
li a3, 0xffff; # color
jal  LCD_DrawCircle;

lw ra, 0(sp);
addi sp, sp, 4;
ret