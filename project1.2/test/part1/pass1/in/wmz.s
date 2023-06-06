#SB-type
addi a1, x0, 0x134
addi a2, a1, 0x134
addi a3, zero, 0x130
beq a1, a3, L2
bne a1, a2, L4
blt a2, a3, L2
bge a3, a2, L2
bltu a2, a3, L2
bgeu a3, a2, L3 
L2:
L3:
L4:
#jal
jal ra, label
label: