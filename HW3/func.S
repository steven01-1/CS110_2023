# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS

LPS:
# YOUR CODE HERE
	# a0 is the string address
	# t0 is the returned value, the longest par
	# t1 is head, t2 is tail
	# t3 is the original string's length
	# t4 is the for-loop counter
	
	addi sp sp -16
	sw s0 0(sp)
	sw s1 4(sp)
	sw s2 8(sp)
	sw s3 12(sp)
	sw ra 16(sp)

	# zero-length string
	lb s0 0(a0)
    li t0 0
	beq s0 x0 Out_loop_end

	# detect the lengh of the original string
	li t3 0
	add s0 x0 a0 # a temporary ptr for string
	addi sp sp -4
	sw a0 0(sp)
	jal ra Detect_length # call the function
	add t3 a0 x0
	lw a0 0(sp)
	addi sp sp 4

	# main loop 
	li t0 1 # the return result
	li t4 0 # the for-loop counter
	Out_loop:
		# for loop to detect the longest palindromic substring
		addi t4 t4 1 # counter + 1
		bge t4 t3 Out_loop_end # if counter >= length
		# even substring  ##########
		# initialize the head and tail
		addi t1 t4 -1 # head = counter - 1
		addi t2 t4 0 # tail = counter
		jal ra Loop_even # loop even
		sub t1 t2 t1 # head = tail - head
		addi t1 t1 -1 # head - 1
		jal ra Updata # not a function. It is in the loop.

		# odd substring  ##########
		# initialize the head and tail
		addi t1 t4 -1 # head = counter - 1
		addi t2 t4 1 # tail = counter	
		jal ra Loop_odd # loop odd
		sub t1 t2 t1 # head = tail - head
		addi t1 t1 -1 # head - 1		
		jal ra Updata
		j Out_loop
	Out_loop_end:
		add a0 x0 t0
		lw s0 0(sp)
		lw s1 4(sp)
		lw s2 8(sp)
		lw s3 12(sp)
		lw ra 16(sp)
		addi sp sp 16
		ret

Loop_even:
	add s0 t1 a0
	add s1 t2 a0 # get address
	lb s0 0(s0) # get char
	lb s1 0(s1)
	bne s0 s1 Loop_even_end # if str_head != str_tail
	blt t1 x0 Loop_even_end # if head < 0
	bge t2 t3 Loop_even_end # if tail >= length
	addi t1 t1 -1 # head - 1
	addi t2 t2 1 # tail + 1
	j Loop_even # loop
Loop_even_end:
	ret

Loop_odd:
	add s0 t1 a0
	add s1 t2 a0 # get address	
	lb s0 0(s0) # get char
	lb s1 0(s1)
	bne s0 s1 Loop_odd_end # if str_head != str_tail
	blt t1 x0 Loop_odd_end # if head < 0
	bge t2 t3 Loop_odd_end # if tail >= length
	addi t1 t1 -1 # head - 1
	addi t2 t2 1 # tail + 1
	j Loop_odd # loop
Loop_odd_end:
	ret

Detect_length:
	addi sp sp -8
	sw s0 0(sp)
	sw s1 4(sp)
	li a1 0 # the lenth of the original string
	add s0 a0 x0 # a temporary ptr for string
Detect_length_loop:
	# for loop to detect length
	addi s0 s0 1 # str ptr
	addi a1 a1 1 # length
	lb s1 0(s0) # temporary str[i]
	blt x0 s1 Detect_length_loop # if str[i] > 0
	add a0 x0 a1 # return the length
	lw s0 0(sp)
	lw s1 4(sp)
	addi sp sp 8
	ret

Updata: 
	blt t0 t1 Up_t0 # max < tail - head -1
	ret
Up_t0:
    add t0 x0 t1
	ret