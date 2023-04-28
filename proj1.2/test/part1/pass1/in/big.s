# Revised RISC-V code example

.data # data segment
    bytes: .byte 96 97 98
    array: .word 1, 2, 3, 4 # an array of four words
    size: .word 4 # the size of the array
    halfs: .half 0x1234, 0x5678 # two halfwords

.text # text segment
    main: # main label
        li x1, 10 # load immediate value to x1
        la x2, msg # load address of msg to x2
        mv x3, x1 # move value from x1 to x3
        
        addi x4, zero, 5 # add immediate value to zero and store in x4
        add x5, x1, x4 # add values from x1 and x4 and store in x5
        
        addi sp ,sp ,-16# subtract immediate value from stack pointer and store in sp (allocate stack space)
        sw ra ,12(sp) # store word from return address to stack (save return address)
        
        jal ra ,func1# jump and link to func1 label (call func1)
        
        lw ra ,12(sp) # load word from stack to return address (restore return address)
        addi sp ,sp ,16# add immediate value to stack pointer and store in sp (deallocate stack space)
        
        jalr zero ,ra ,0# jump and link register to return address with offset zero (return from main)

    func1: 
        lui t0 ,0x12345# load upper immediate value to t0 
        auipc t1 ,0x67890# add upper immediate PC-relative value to t1
        
        la a7 ,bytes# load address of bytes to a7 
        lb t2 ,(a7)# load byte from bytes with offset zero and sign-extend to t2 
        lbu t3 ,2(a7)# load byte from bytes with offset two and zero-extend to t3
        
        la a6,halfs# load address of halfs to a6 
        lh t4 ,(a6)# load halfword from halfs with offset zero and sign-extend to t4 
        lhu t5 ,2(a6)# load halfword from halfs with offset two and zero-extend to t5
        
        la s0,array# load address of arrayto s0 
        lw s1,(s0)# load word from arraywith offset zeroto s1 
        lw s2,size(zero)# load word from sizewith offset zeroto s2 
        
       L2:
           mul a0,s0,s2# multiply values from s0and s2and store lower32 bits in a0 
           div a1,s1,s2# divide valuesfrom s1by s2and store quotientin a1 
           rem a2,s0,s2# divide valuesfrom s0by s2and store remainderin a2 
           divu a3,s1,s2# divide unsigned valuesfrom s1by s2and store quotientin a3 
           remu a4,s0,s2# divide unsigned valuesfrom s0by s2and store remainderin a4 
        

           csrrw a3,mstatus,a3# read/write CSR mstatus using a3 as source/destination 

           ecall# environment call 

       L3:
           bne a3,a7,L4# branch if not equal between a3 and a7to L4 label 

       L4:
           ret# return from function (same as jalr ra,x0,x31) 

       L5:
           j L6# jump unconditionallyto L6 label 

       L6:
           jr ra# jump register unconditionallyto ra (same as jalr ra,x31,x31) 

       L7:
           blt a6,a7,L8# branch if less than betweena6 anda7toL8label

       L8:
           bgeu t6,t7,L9# branch if greater than or equal unsigned betweent6and t

        # Continued RISC-V code example

       L9:
           slt t8,a6,a7# set less than between a6 and a7 and store in t8 
           sltu t9,t6,t7# set less than unsigned between t6 and t7 and store in t9 

           slli s3,s1,2# shift left logical immediate value from s1 by 2 bits and store in s3 
           srli s4,s2,1# shift right logical immediate value from s2 by 1 bit and store in s4 
           srai s5,s0,-1# shift right arithmetic immediate value from s0 by -1 bit and store in s5 

           xor a4,a0,a1# exclusive or values from a0 and a1 and store in a4 
           xori a5,a2,-10# exclusive or immediate value from a2 by -10 and store in a5 

           sb t2,array+8(s0)# store byte from t2 to array with offset 8 plus value of s0 
           sh t4,array+12(s0)# store halfword from t4 to array with offset 12 plus value of s0 
           sw ra,array+16(s0)# store word from ra to array with offset 16 plus value of s0 

       L10:
           beqz x31,L11# branch if equal to zero between x31 and zero to L11 label 

       L11:
           bnez x31,L12# branch if not equal to zero between x31 and zero to L12 label 

       L12:
           fence.i # instruction fence
           
       L13:
            nop # no operation (same as addi x0,x0,0)

       L14:
            li gp ,100 # load immediate value 100 to gp
            li tp ,200 # load immediate value 200 to tp

        end: # end label
            j end # jump unconditionallyto end label (infinite loop)
