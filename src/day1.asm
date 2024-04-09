;  day1.asm  Advent of Code Day 1
;
; Assemble: nasm -f elf64 day1.asm
; Link:     ld -o aoc day1.o ...

%include "defines.inc"

BITS 64

section .rodata
    header: db "----- DAY 1 -----", 10, 0
    header_len: equ $ - header

    solved_part1:     db "Part 1 result: ", 0
    solved_part1_len: equ $ - solved_part1

    solved_part2:     db "Part 2 result: ", 0
    solved_part2_len: equ $ - solved_part2

    string_one:   db "one",   0
    string_two:   db "two",   0
    string_three: db "three", 0
    string_four:  db "four",  0
    string_five:  db "five",  0
    string_six:   db "six",   0
    string_seven: db "seven", 0
    string_eight: db "eight", 0
    string_nine:  db "nine",  0

    digit_strings_array: dq                        \
        string_one,   string_two,   string_three,  \
        string_four,  string_five,  string_six,    \
        string_seven, string_eight, string_nine,
    digit_strings_len: equ $ - digit_strings_array


section .text
    global solve_day1

    extern print_number
    extern print_newline
    extern string_starts_with


; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part1:
    push rbx
    push r12
    push r13
    push r14
    push r15
    push rbp

    ; Set up the stack
    mov rbp, rsp
    sub rsp, 2 * 8

    %define TOTAL_VALUE rsp + 0 * 8
    %define FIRST_DIGIT rsp + 1 * 8
    %define LAST_DIGIT  rsp + 2 * 8
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rdi

    ; Iteration counter
    mov r14, 0

.loop_read_input:
    ; Preserve current character at index r14
    xor rax, rax
    mov al, [r13 + r14]
    mov r15, rax

    cmp r15, 0
    je .end_of_input

    cmp r15, 10
    jne .not_newline

    ; Compute the value for the current line
    mov rbx, [FIRST_DIGIT]
    mov rdi, [LAST_DIGIT]
    imul rbx, 10
    add rbx, rdi

    ; Add computed value to total value and clear the digits
    add [TOTAL_VALUE], rbx
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT], 0
.not_newline:

    ; Check if the character is an ASCII digit (codes 48-57 are digits)
    cmp r15, 48 
    jl .not_a_digit
    cmp r15, 57
    jg .not_a_digit
    mov rax, r15
    sub rax, 48
    mov [LAST_DIGIT], rax

    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found
    mov [FIRST_DIGIT], rax
.first_number_found:
.not_a_digit:

    inc r14
    jmp .loop_read_input
.end_of_input:
    mov rax, QWORD [TOTAL_VALUE]

    mov rsp, rbp

    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    ret


; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part2:
    push rbx
    push r12
    push r13
    push r14
    push r15
    push rbp

    ; Set up the stack
    mov rbp, rsp
    sub rsp, 16

    %define TOTAL_VALUE rsp + 0
    %define FIRST_DIGIT rsp + 8
    %define LAST_DIGIT  rsp + 16
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rdi

    ; Iteration counter
    mov r14, 0

.loop_read_input:
    ; Preserve current character at index r14
    xor rax, rax
    mov al, [r13 + r14]
    mov r15, rax

    cmp r15, 0
    je .end_of_input

    cmp r15, 10
    jne .not_newline

    ; Compute the value for the current line
    mov rbx, [FIRST_DIGIT]
    mov rdi, [LAST_DIGIT]
    imul rbx, 10
    add rbx, rdi

    ; Add computed value to total value and clear the digits
    add [TOTAL_VALUE], rbx
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT], 0
.not_newline:

    ; Check if the character is an ASCII digit (codes 48-57 are digits)
    cmp r15, 48 
    jl .not_a_digit
    cmp r15, 57
    jg .not_a_digit
    mov rax, r15
    sub rax, 48
    mov [LAST_DIGIT], rax

    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found
    mov [FIRST_DIGIT], rax
.first_number_found:
.not_a_digit:

    ; Iterate over the array of string pointers.
    ; mov r12, [digit_strings_len]
    mov r12, 0
.loop_digit_string_array:
    lea rdi, [r13 + r14]
    mov rsi, [digit_strings_array + r12 * 8]
    inc r12

    call string_starts_with
    cmp rax, 0
    je .not_a_digit_string
    mov rax, r12
    mov QWORD [LAST_DIGIT], rax
    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found2
    mov QWORD [FIRST_DIGIT], rax
.first_number_found2:
.not_a_digit_string:

    cmp r12, 9
    jne .loop_digit_string_array

    inc r14
    jmp .loop_read_input
.end_of_input:
    mov rax, QWORD [TOTAL_VALUE]

    mov rsp, rbp

    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    ret


; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     None.
solve_day1:
    push rbx
    push r12

    mov r12, rdi

    call print_newline

    PRINT header, header_len

    ; PART 1
    mov rdi, r12
    call solve_part1
    mov rbx, rax

    PRINTLN_WITH_NUMBER solved_part1, solved_part1_len, rbx

    ; PART 2
    mov rdi, r12
    call solve_part2
    mov rbx, rax

    PRINTLN_WITH_NUMBER solved_part2, solved_part2_len, rbx

    pop r12
    pop rbx
    ret
