	beq s0 s0 start #0 00840663
bad-loop: 
	addi sp sp -1 #1   FFF10113
	beq x0 x0 bad-loop #2FE000EE3

start:
	addi s1 s1 10#3  00A48493
	blt s0 s1 label1#4 00944A63
	beq x0 x0 bad-loop#5 FE0008E3

label2: 
	addi s1 s1 -20#6   FEC48493
	bltu s0 s1 end#7  00946A63
    beq x0 x0 bad-loop#8  FE0002E3

label1: 
	addi s0 s0 20#9  01440413
	blt s1 s0 label2#10  FE84C8E3
    beq x0 x0 bad-loop#11  FC000CE3

end:
	add a0 x0 x0#12  00000533

#0,3,4,9,10,6,7,12