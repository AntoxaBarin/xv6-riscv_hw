.data
fmt: .asciz "%d\n"
error_ii: .asciz "Incorrect input!\n"
input: .space 23

.text
.global main

main:
    la a0, input                       # arg1: string pointer
    li a1, 23                          # arg2: max length
    call gets                          # call gets with args 1, 2

    la t0, input                       # t0 - address of current symbol
    li t1, 0                           # first number
    li t2, 0                           # second number

fst_num_loop:
    lbu t3, 0(t0)                      # push symbol input[t0] to t3
    beqz t1, check_fst_digit_fst_num   # check if first symbol == '0'

fst_num_fst_digit_checked:             # first symbol != '0' 
    mv t6, t3                        
    addi t6, t6, -10                   # check if symbol == '\n'  
    beqz t6, error_incorrect_input     # if true -> print an error   

    mv t6, t3                        
    addi t6, t6, -32                   # check if symbol == ' '  
    beqz t6, done_fst_num              # if true -> start parse second number

    mv t6, t3
    addi t6, t6, -48                   # check if symbol < '0'
    bltz t6, error_incorrect_input     # if true -> print an error (non numeric symbol in number)

    mv t6, t3
    addi t6, t6, -57                   # check if symbol > '9' 
    bgtz t6, error_incorrect_input     # if true -> print an error (non numeric symbol in number)

    addi t3, t3, -48                   # convert ASCII code into number
    li t5, 10                        
    mul t1, t1, t5                     # multiply result number by 10
    add t1, t1, t3                     # add new digit to result number

    addi t0, t0, 1                     # go to next symbol of input
    j fst_num_loop                     # next iteration of loop

done_fst_num:
    addi t0, t0, 1                     # skip space symbol between numbers
    li t3, 0                          

snd_num_loop:
    lbu t4, 0(t0)                      # push symbol input[t0] to t4

    beqz t2, check_fst_digit_snd_num   # check if first symbol == '0'

snd_num_fst_digit_checked:             # first symbol != '0' 

    mv t6, t4                        
    addi t6, t6, -10                   # check if symbol == '\n'
    beqz t6, done_parse                # if true -> we read all digits of second number

    mv t6, t4
    addi t6, t6, -48                   # check if symbol < '0'
    bltz t6, error_incorrect_input     # if true -> print an error (non numeric symbol in number)

    mv t6, t4
    addi t6, t6, -57                   # check if symbol > '9'
    bgtz t6, error_incorrect_input     # if true -> print an error (non numeric symbol in number)

    beqz t4, done_parse                # if symbol == '\0'   
 
    addi t4, t4, -48                   # convert ASCII code into number    
    li t5, 10                         
    mul t2, t2, t5                     # multiply result number by 10
    add t2, t2, t4                     # add new digit to result number

    addi t0, t0, 1                     # go to next symbol
    j snd_num_loop                     # new iteration of loop

done_parse:
    add a1, t1, t2                     # arg2: sum of entered numbers 
    la a0, fmt                         # arg1: format string "%d\n"
    call printf                        # call printf to output sum 
    j exit                             # junp to exit lable 

check_fst_digit_fst_num:             
    mv t6, t3
    addi t6, t6, -48                   
    bltz t6, error_incorrect_input     # check if first symbol < '0'
    bnez t6, fst_num_fst_digit_checked # check if first symbol != '0'
                                       # (checking if symbol < '9' is on line 37) 

    addi t0, t0, 1                     # go to next symbol
    lbu t3, 0(t0)                      # load symbol agter '0' 

    addi t3, t3, -32                   
    beqz t3, done_fst_num              # if space symbol after '0' -> first number is 0 
    j error_incorrect_input            # else - incorrect number

check_fst_digit_snd_num:
    mv t6, t4
    addi t6, t6, -48                 
    bltz t6, error_incorrect_input     # check if first symbol < '0'
    bnez t6, snd_num_fst_digit_checked # check if first symbol != '0'

    addi t0, t0, 1                     # go to next symbol
    lbu t3, 0(t0)                      # load symbol agter '0' 

    addi t3, t3, -10                   
    beqz t3, done_parse                # if '\n' symbol after '0' -> second number is 0
    j error_incorrect_input            # else - incorrect number    

error_incorrect_input:
    li a0, 2                           # stderr - file descriptror
    la a1, error_ii                    # error message                             
    li a2, 18                          # length of error message
    li a7, 16                          # write system call
    ecall                              # Syscall         
    
exit:
    li a7, 2                           # Exit system call
    li a0, 0                           # Exit status
    ecall                              # Syscall

