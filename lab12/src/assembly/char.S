.section .text
.globl d_char
.type d_char, @function
d_char:

addi sp, sp, -4;
sw ra, 0(sp);

li a0, 30; # x
li a1, 10; # y
li a2, 67; # C
li a3, 1; # mode
li a4, 0xffff;
jal LCD_ShowChar;

li a0, 40; # x
li a1, 10; # y
li a2, 83; # S
li a3, 1; # mode
li a4, 0xffff;
jal LCD_ShowChar;

li a0, 50; # x
li a1, 10; # y
li a2, 49; # 1
li a3, 1; # mode
li a4, 0xffff;
jal LCD_ShowChar;

li a0, 60; # x
li a1, 10; # y
li a2, 49; # 1
li a3, 1; # mode
li a4, 0xffff;
jal LCD_ShowChar;

li a0, 70; # x
li a1, 10; # y
li a2, 48; # 0
li a3, 1; # mode
li a4, 0xffff;
jal LCD_ShowChar;

lw ra, 0(sp);
addi sp, sp, 4;
ret;
 