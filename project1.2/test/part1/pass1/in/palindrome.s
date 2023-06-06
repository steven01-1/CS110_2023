
.data
str:
    # .string "abbddbbb"
    .byte 97 98 98 100 100 98 98 98 0


.text
main:
	# load the address of a0 into str
	la a0 str
	# call your function
   	jal LPS
	# print the result
	jal print_int
	jal print_line
	jal print_line

exit:
    li a0 10    #
    ecall       #


LPS:

    addi sp sp -48
        sw ra +0 sp         # save the return address
        sw a0 +4 sp          # save the old_str
        jal print_str
        jal print_line

        lw a0 +4 sp          # old_str
        jal strlen                  # strlen(old_str)
        sw a0 +8 sp       # old_strlen = strlen(old_str)
        jal print_int
        jal print_line

        lw a0 +8 sp       # old_strlen
        slli a0 a0 1                # old_strlen * 2
        addi a0 a0 3                # old_strlen * 2 + 3
        jal malloc                  # malloc(old_strlen * 2 + 3)
        sw a0 +12 sp          # new_str = malloc(old_strlen * 2 + 3)
        jal print_hex
        jal print_line



        lw a0 +12 sp          # new_str
        lw a1 +28 sp                # j
        li a2 +36          # begin_char
        jal char_index_set                 # new_str[j] = begin_char

        lw a1 +28 sp                # j
        addi a1 a1 1                # j + 1
        sw a1 +28 sp                # j = j + 1

        lw a0 +12 sp          # new_str
        li a2 +35            # sep_char
        jal char_index_set                 # new_str[j] = sep_char
        
        lw a1 +28 sp                # j
        addi a1 a1 1                # j + 1
        sw a1 +28 sp                # j = j + 1

        LPS_set_new_str_loop_begin:             
            lw a0 +4 sp                  # old_str
            lw a1 +24 sp                        # i
            jal char_index_get                          # old_str[i]
            beqz a0 LPS_set_new_str_loop_end    # if po[0] == 0: break
            
            mv a2 a0                            # old_str[i]   
            lw a0 +12 sp                  # new_str
            lw a1 +28 sp                        # j
            jal char_index_set                         # new_str[j] = old_str[i]

            lw a0 +24 sp                        # i
            addi a0 a0 1                        # i + 1
            sw a0 +24 sp                        # i = i + 1

            lw a1 +28 sp                        # j
            addi a1 a1 1                        # j + 1

            lw a0 +12 sp                  # new_str
            li a2 +35                    # sep_char
            jal char_index_set                         # new_str[j+1] = sep_char

            addi a1 a1 1                        # j + 2
            sw a1 +28 sp                        # j = j + 2

            j LPS_set_new_str_loop_begin        
        LPS_set_new_str_loop_end:   

        lw a0 +12 sp          # new_str
        lw a1 +28 sp                # j
        mv a2 zero                  # '\0'
        jal char_index_set                 # new_str[j] = '\0'

        lw a0 +12 sp                    
        jal print_str
        jal print_line
        

        lw a0 +12 sp          # new_str
        jal strlen                  # strlen(new_str)
        sw a0 +16 sp       # new_strlen = strlen(new_str)
        jal print_int
        jal print_line

        lw a0 +16 sp           # new_strlen
        li a1 4                         # sizeof(int)
        jal calloc                      # calloc(new_strlen, sizeof(int))
        sw a0 +20 sp   # palindromelen_list = calloc(new_strlen, sizeof(int))
        jal print_hex
        jal print_line


        li a0 1                             # 1
        sw a0 +24 sp                        # i = 1
        sw zero +32 sp                      # c = 0
        sw zero +36 sp                      # r = 0
        sw zero +40 sp      # max_len = 0

        LPS_scan_palindrome_begin:
            lw a0 +16 sp               # new_strlen
            addi a0 a0 -1                       # new_strlen - 1
            lw a1 +24 sp                        # i
            bge a1 a0 LPS_scan_palindrome_end   # while i < new_strlen - 1

            lw a0 +24 sp                        # i
            addi a0 a0 1                        # i + 1
            sw a0 +24 sp                        # i = i + 1

            jal print_int
            jal print_tab


            lw a0 +36 sp                        # max_right
            lw a1 +24 sp                        # i
            bge a1 a0 LPS_scan_palindrome_if1   # if i < max_right

                lw a1 +24 sp                    # i
                lw a0 +32 sp                    # c
                slli a0 a0 1                    # 2 * c
                sub a1 a0 a1                    # 2 * c - i
                sw a1 +28 sp                    # j = 2 * c - i

                lw a0 +20 sp   # p_list
                jal int_index_get                       # p_list[j]

                lw a1 +24 sp                    # i
                lw a2 +36 sp                    # max_right
                sub a1 a2 s1                    # max_right - i
                jal min                         # min(p_list[j], max_right - i)
                mv a2 a0                        
                lw a0 +20 sp   # p_list
                lw a1 +24 sp                    # i
                jal int_index_set                      # p_list[i] = min(p_list[j], max_right - i)

            LPS_scan_palindrome_if1:


            LPS_scan_palindrome_while_begin:

                lw a0 +20 sp       # p_list
                lw a1 +24 sp                        # i
                jal int_index_get                           # p_list[i]
                addi a3 a0 1                        # p_list[i] + 1
                lw a0 +24 sp                        # i
                
                lw a0 +12 sp                  # new_str
                lw a1 +24 sp                        # i
                sub a1 a1 a3                        # i - p_list[i] - 1
                jal char_index_get                          # new_str[i - p_list[i] - 1]
                mv a4 a0                            # new_str[i - p_list[i] - 1]

                lw a0 +12 sp                  # new_str
                lw a1 +24 sp                        # i
                add a1 a1 a3                        # i + p_list[i] + 1
                jal char_index_get                          # new_str[i + p_list[i] + 1]

            bne a4 a0 LPS_scan_palindrome_while_end # while new_str[i - p_list[i] - 1] == new_str[i + p_list[i] + 1]

                lw a0 +20 sp       # p_list
                lw a1 +24 sp                        # i
                jal int_index_get                           # p_list[i]
                addi a2 a0 1                        # p_list[i] + 1
                lw a1 +24 sp                        # i
                lw a0 +20 sp       # p_list
                jal int_index_set                          # p_list[i] = p_list[i] + 1

            j LPS_scan_palindrome_while_begin
            LPS_scan_palindrome_while_end:

            lw a0 +20 sp       # p_list
            lw a1 +24 sp                        # i
            jal int_index_get                           # p_list[i]
            lw a1 +24 sp                        # i
            add a0 a1 a0                        # p_list[i] + i
            lw a1 +36 sp                        # max_right
            ble a0 a1 LPS_scan_palindrome_if2   # if p_list[i] + i > max_right
                sw a0 +36 sp                    # max_right = p_list[i] + i
                lw a0 +24 sp                    # i
                sw a0 +32 sp                    # c = i
            LPS_scan_palindrome_if2:

            lw a0 +20 sp       # p_list
            lw a1 +24 sp                        # i
            jal int_index_get                           # p_list[i]
            lw a1 +40 sp        # max_len
            jal max                             # max(max_len, p_list[i])
            sw a0 +40 sp        # max_len = max(max_len, p_list[i])

            lw a1 +16 sp               
            lw a0 +20 sp       
            jal print_list
            jal print_tab
            lw a0 +40 sp
            jal print_int
            jal print_line

        j LPS_scan_palindrome_begin
        LPS_scan_palindrome_end:
        jal print_line


        lw a0 +12 sp                  # new_str
        jal free                            # free(new_str)
        lw a0 +20 sp       # p_list
        jal free                            # free(p_list)

        lw ra +0 sp                 # restore return address
        lw a0 +40 sp        # max_len
    addi sp sp +48
    ret                                     # return max_len


# Array indexing 

int_index_get:
    slli a1 a1 2        # a1 * sizeof(int)
    add a0 a0 a1        # a0 + a1 * sizeof(int)
    lw a0 0(a0)         # return a0[a1]
    ret                 

int_index_set:
    slli a1 a1 2        # a1 * sizeof(int)
    add a0 a0 a1        # a0 + a1 * sizeof(int)
    sw a2 0(a0)         # a0[a1] = a2
    ret                  

char_index_get:
    add a0 a0 a1        # a0 + a1 * sizeof(char)
    lb a0 0(a0)         # return a0[a1]
    ret

char_index_set:
    add a0 a0 a1        # a0 + a1 * sizeof(char)
    sb a2 0(a0)         # a0[a1] = a2
    ret

# String functions

strlen:
    # return the length of the string
    mv a1 a0            # str
    li a0 0             # len
    strlen_loop:      
        lb a2 0(a1)     # *str
        beqz a2 return  # if str = '\0' return 
        addi a0 a0 1    # len++
        addi a1 a1 1    # str++
        j strlen_loop

# Print functions


print_str:
    mv a1 a0    # a0
    li a0 4     # print_str
    ecall       # print_str(a0)
    ret
print_int:
    mv a1 a0    # a0
    li a0 1     # print_int
    ecall       # print_int(a0)
    ret
print_char:
    mv a1 a0    # a0
    li a0 11    # print_char
    ecall       # print_char(a0)
    ret
print_hex:
    mv a1 a0    # a0
    li a0 34    # print_hex
    ecall       # print_hex(a0)
    ret         
print_line:
    li a0 11    # '\n'
    li a1 10    # print_char
    ecall       # print_char('\n')
    ret
print_space:
    li a1 32    # ' '
    li a0 11    # print_char
    ecall       # print_char(' ')
    ret
print_tab:
    li a1 9     # '\t'
    li a0 11    # print_char
    ecall       # print_char('\t')
    ret
print_list:
    # print a0[0:a1]
    mv a2 a0    # a
    mv a3 a1    # i = n
    print_list_loop:
        blez a3 return      # while i > 0
        lw a1 0(a2)         # a[i]
        li a0 1             # print_int
        ecall               # print_int(a[i])
        li a1 32            # ' '
        li a0 11            # print_int
        ecall               # print_int(' ')
        addi a2 a2 4        # 
        addi a3 a3 -1       # i--
        j print_list_loop

# Memory functions


malloc: 
    mv a1 a0        # a0
    li a0 0x3CC     # 
    li a6 1         # malloc
    ecall           # malloc(a0)
    ret             # return malloc(a0)
calloc:
    mv a2 a1        # a1
    mv a1 a0        # a0
    li a0 0x3CC     #
    li a6 2         # calloc
    ecall           # calloc(a0, a1)
    ret             # return calloc(a0, a1)
realloc:
    mv a2 a1        # a1
    mv a1 a0        # a0
    li a0 0x3CC     #
    li a6 3         # realloc
    ecall           # realloc(a0, a1)
    ret             # return realloc(a0, a1)
free:
    mv a1 a0        # a0
    li a0 0x3CC     #
    li a6 4         # free
    ecall           # free(a0)
    ret

# Math functions


max:
    bge a0 a1 return    # if a0 >= a1 return a1
    mv a0 a1            # else return a0
    ret     
min:
    ble a0 a1 return    # if a0 <= a1 return a1
    mv a0 a1            # return a0
    
    
# Iterative functions


map:
    # map(head, tail, func)
    addi sp sp -16     
    sw ra +0 sp         # save return address
    sw a0 +4 sp             # save head
    sw a1 +8 sp             # save tail
    sw a2 +12 sp             # save func


    map_loop_begin:
        lw a0 +4 sp         # head
        lw a1 +8 sp         # tail
    bge a0 a1 map_loop_end          # while head < tail

        lw a2 +12 sp         # func
        lw a1 0(a0)                 # *head
        jalr a2                     # func(*head)

        lw a1 +4 sp         # head
        sw a0 0(a1)                 # *head = func(*head)
        addi a1 a1 4                # head + 1
        sw a1 +4 sp         # head = head + 1

    j map_loop_begin
    map_loop_end:

    lw ra +0 sp         # restore return address
    addi sp sp +16     # restore stack
    ret

return:
    ret