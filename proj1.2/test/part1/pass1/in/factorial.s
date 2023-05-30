.data
test_input: .word 1 2 3 4 5 6 7 8 9 10 11 12

.text
main:
	add t0, x0, x0, x1
	addi t1, x0, 12
	la t2, test_input
main_loop:
	beq t0, t1, main_exit
	slli t3, t0, 2
	add t4, t2, t3
	lw a0, 0(t4)

	addi sp, sp, -20
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw t3, 12(sp)
	sw t4, 16(sp)

	jal ra, factorial

	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw t3, 12(sp)
	lw t4, 16(sp)
	addi sp, sp, 20

	addi a1, a0, 0
	addi a0, x0, 1
	ecall # Print Result
	addi a1, x0, 10
	addi a0, x0, 11
	ecall
	
	addi t0, t0, 1
	jal x0, main_loop
main_exit:  
	addi a0, x0, 10
	ecall # Exit

factorial:
    add t0 x0 a0 #t0=n
    addi a1 x0 1 #a1 is the total
f_loop:
    beq t0 x0 return
    addi sp sp -8
    sw t0 4(sp)
	sw x1 0(sp) 
	add a0 x0 t0 #pass parameter a0 and a1 to multiply
	add a1 x0 a1 #a0 is n-i, a1 is the total
	jal ra, multiply #call the multiply function to mul a0, a1
	lw t0 4(sp)
	lw x1 0(sp)
	addi t0 t0 -1 #t0=n-i (i++)
	addi sp sp 8  
	jal x0, f_loop

return:
	add a0 x0 a1 # Transfer a1 to a0, which will be printed in the main
    jalr x0 x1 0 # return to the main function
    #jr ra

multiply:
	addi t0 x0 1 #t0 is the bit to mul
	add t1 x0 x0 #t1 is the total
	add t2 x0 x0 #t2 records the number of bits
	addi t5 x0 32 #t5 is the size of int
mul_loop:
	bgeu t2 t5 mul_return #if i>=31, return
	and t3 a0 t0 #t3 is the t0-th bit of a0
	beq t3 x0 mul_not_add
	sll t4 a1 t2 #t4 is 2^(t0-1) * a1
	add t1 t1 t4 #t1 is the total
mul_not_add:
	slli t0 t0 1 #move to the next bit
	addi t2 t2 1 #add the bit counter by one
	jal x0, mul_loop
mul_return:
	add a1 x0 t1 #store the sum into a1
	jr ra		#retrun to the fracorial func
