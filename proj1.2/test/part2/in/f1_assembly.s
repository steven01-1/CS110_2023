.data
n: .word 3 # number of test data
data: # input, answer
.word 0x12345678, 0x1E6A2C48
.word 0x71C924BF, 0xFD24938E
.word 0x19260817, 0xE8106498

.text
# === main and helper functions ===
# You don't need to understand these, but reading them may be useful
main:
    la t0, n
    lw s0, 0(t0)
    la s1, data
    main_loop: # run each of the n tests
        beq s0, zero, main_loop_end
        lw a0, 0(s1)
        jal print_hex_and_space # print input
        lw a0, 4(s1)
        jal print_hex_and_space # print answer
        lw a0, 0(s1)
        jal bitrev1
        jal print_hex_and_space # print result of bitrev1
        lw a0, 0(s1)
        jal bitrev2
        jal print_hex_and_space # print result of bitrev2
        jal print_newline
        addi s0, s0, -1
        addi s1, s1, 8
        j main_loop
    main_loop_end:
    li a0, 10
    ecall # exit

print_hex_and_space:
    mv a1, a0
    li a0, 1
    ecall
    li a1, ' '
    li a0, 11
    ecall
    jalr x0 x1 0

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    jalr x0 x1 0

# === The first version ===
# Reverse the bits in a0 with a loop
bitrev1:
    ### TODO: YOUR CODE HERE ###
    li t1 0
    li t2 0
    li t3 0
    li t4 31
loop:
    beq t4 t1 break
    andi t2 a0 1 #t2 last bit 0/1
    add t3 t2 t3
    slli t3 t3 1
    srli a0 a0 1
    addi t4 t4 -1
    j loop
break:    
    mv a0 t3
    li t1 0
    li t2 0
    li t3 0
    li t4 0
    jalr x0 x1 0 #ret
# === The second version ===
# Reverse the bits in a0. Only use li, and, or, slli, srli!
bitrev2:
    ### TODO: YOUR CODE HERE ###
    li t0, 0x55555555 # t0 = 0101-0101-0101-0101-0101-0101-0101-0101
    li t1, 0xaaaaaaaa # t1 = 1010-1010-1010-1010-1010-1010-1010-1010
    and t3 a0 t0
    slli t3 t3 1
    and t4 a0 t1
    srli t4 t4 1
    add a0 t3 t4
    #step3
    li t0, 0x33333333 # t0 = 0011-0011-0011-0011-0011-0011-0011-0011
    li t1, 0xcccccccc # t1 = 1100-1100-1100-1100-1100-1100-1100-1100
    and t3 a0 t0
    slli t3 t3 2
    and t4 a0 t1
    srli t4 t4 2
    add a0 t3 t4

    li t0, 0x0F0F0F0F # t0 = 0000-1111-0000-1111-0000-1111-0000-1111
    li t1, 0xF0F0F0F0 # t1 = 1111-0000-1111-0000-1111-0000-1111-0000
    and t3 a0 t0
    slli t3 t3 4
    and t4 a0 t1
    srli t4 t4 4
    add a0 t3 t4

    li t0, 0x00ff00ff 
    li t1, 0xff00ff00
    and t3 a0 t0
    slli t3 t3 8
    and t4 a0 t1
    srli t4 t4 8
    add a0 t3 t4

    li t0, 0x0000ffff 
    li t1, 0xffff0000 
    and t3 a0 t0
    slli t3 t3 16
    and t4 a0 t1
    srli t4 t4 16
    add a0 t3 t4
    jalr x0 x1 0