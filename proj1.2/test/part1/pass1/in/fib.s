.data
n: .word 9

.text
main:   add     t0, x0, x0
        addi    t1, x0, 0xFFFFFFFF
        la      t3, n
        lw      t3, n
        lui t3 4099
        auipc t3 1
        li t3 -1
        li t3 -666666
        beqz t3 fib
        lw t3,n
        la ra fib
        jalr ra ra 6
        jalr ra
fib:    beq     t3, x0, finish
        j       fib
finish: addi    a0, x0, 1
        ecall
        ecall
        jal finish
        slti t1 t1 2023
        srai t1 t1 13
        sb t1 4(t1)
        sh t1 -4(t1)
        sw t1 -2048(t1)
