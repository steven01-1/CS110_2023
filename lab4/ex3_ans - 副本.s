.data
n: .word 3 
data: 
.word 0x12345678, 0x1E6A2C48
.word 0x71C924BF, 0xFD24938E
.word 0x19260817, 0xE8106498

.text


main:
    la t0, n
    lw s0, 0(t0)
    la s1, data
    main_loop: 
        beq s0, zero, main_loop_end
        lw a0, 0(s1)
        jal print_hex_and_space 
        lw a0, 4(s1)
        jal print_hex_and_space 
        lw a0, 0(s1)
        jal bitrev1
        jal print_hex_and_space 
        lw a0, 0(s1)
        jal bitrev2
        jal print_hex_and_space 
        jal print_newline
        addi s0, s0, -1
        addi s1, s1, 8
        j main_loop
    main_loop_end:
    li a0, 10
    ecall 

print_hex_and_space:
    mv a1, a0
    li a0, 34
    ecall
    li a1, ' '
    li a0, 11
    ecall
    ret

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    ret



bitrev1:
    
    li t0 32
    li t1 0
    li t2 0
    j loop
    loop: 
        andi t3 a0 1
        slli t2 t2 1
        add t2 t2 t3
        srli a0 a0 1
        addi t0 t0 -1
        bne t0 t1 loop
    addi a0 t2 0
    ret



bitrev2:
    
    li t0 0x0000ffff
    li t1 0xffff0000
    and t2 t0 a0
    slli t2 t2 16
    and t3 t1 a0
    srli t3 t3 16
    or a0 t2 t3
    li t0 0x00ff00ff
    li t1 0xff00ff00
    and t2 t0 a0
    slli t2 t2 8
    and t3 t1 a0
    srli t3 t3 8
    or a0 t2 t3
    li t0 0x0f0f0f0f
    li t1 0xf0f0f0f0
    and t2 t0 a0
    slli t2 t2 4
    and t3 t1 a0
    srli t3 t3 4
    or a0 t2 t3
    li t0 0x33333333
    li t1 0xcccccccc
    and t2 t0 a0
    slli t2 t2 2
    and t3 t1 a0
    srli t3 t3 2
    or a0 t2 t3
    li t0 0x55555555
    li t1 0xaaaaaaaa
    and t2 t0 a0
    slli t2 t2 1
    and t3 t1 a0
    srli t3 t3 1
    or a0 t2 t3
    ret
