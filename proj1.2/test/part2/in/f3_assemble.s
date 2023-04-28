#test sb-type
.text
	L1:
		addi a1, x0, 0x134
		addi a2, a1, 0x134
		addi a3, zero, 0x130
		beq a1, a3, L2
		bne a1, a2, L4
continue:
		blt a2, a3, L2
		bge a3, a2, L2
		bltu a2, a3, L2
		bgeu a3, a2, L3 
        beq a1 a2 L3
	L2:
		addi a0, x0, 10
		ecall
	L4:
    	addi a4, a4, 0x134
		sub a2, a2, a4
        jal x0 continue
	L3:
		addi a0, x0, 10
		ecall