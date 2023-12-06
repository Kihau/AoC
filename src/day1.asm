;  day1.asm  Advent of Code Day 1 
;
; Assemble: nasm -f elf64 day1.asm
; Link:     ld -o day1 day1.o

%include "defines.inc"

BITS 64

section .data
    message: db "Hello, day 1!", 10
    msg_len: equ $-message

    solved_part1:     db "Part 1 result: ", 0
    solved_part1_len: equ $-solved_part1

    solved_part2:     db "Part 2 result: ", 0
    solved_part2_len: equ $-solved_part2

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
    digit_strings_len: equ $-digit_strings_array


section .text
    global solve_day1

    extern print_number
    extern print_newline


; Compare two strings and see if the first string starts with the seconds string.
; Input:
;     rax - pointer to string 1, day1 input buffer
;     rdi - pointer to string 2, the digit name 
; Output:
;     rax - 0 when no match, 1 when match was found.
substring_match:
    ; If we iterated to the end ('\0') of the string 2 that means that we found a match.
    ; If we either reached end of string 1 or we found new line character that means there is no match.

    mov r8, rax
    mov r9, rdi

    ; Iteration counter, used as string character lookup offset.
    mov r10, 0

.substring_loop:
    ; Clear for debugging purposes.
    mov rax, 0
    mov al, [r9 + r10]
    cmp al, 0
    je .strings_matching

    ; Clear for debugging purposes.
    mov rbx, 0
    mov bl, [r8 + r10]
    cmp bl, 0
    je .strings_not_matching
    cmp bl, 10
    je .strings_not_matching

    cmp al, bl
    jne .strings_not_matching

    inc r10
    jmp .substring_loop
    
.strings_not_matching:
    mov rax, 0
    ret

.strings_matching:
    mov rax, 1
    ret


; Does what it says.
; Input:
;     rax - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part1:
    ; Set up the stack
    add rsp, 24

    ; push rbx
    ; push r12
    ; push r13
    ; push r14
    ; push r15

    %define TOTAL_VALUE rsp + 8
    %define FIRST_DIGIT rsp + 16
    %define LAST_DIGIT  rsp + 24
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rax

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
    mov rax, QWORD[TOTAL_VALUE]

    ; pop r15
    ; pop r14
    ; pop r13
    ; pop r12
    ; pop rbx

    ; Set up the stack
    sub rsp, 24

    ret


; Does what it says.
; Input:
;     rax - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part2:
    ; Set up the stack
    add rsp, 24

    %define TOTAL_VALUE rsp + 8
    %define FIRST_DIGIT rsp + 16
    %define LAST_DIGIT  rsp + 24
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rax

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
    lea rax, [r13 + r14]
    mov rdi, [digit_strings_array + r12 * 8]
    inc r12

    call substring_match
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
    mov rax, QWORD[TOTAL_VALUE]

    ; Set up the stack
    sub rsp, 24
    ret

; Does what it says.
; Input:
;     rax - Pointer to the input buffer.
; Output:
;     None
solve_day1:
    mov rbx, rax
    push rbx

    ; Write some dummy welcome message.
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, message
    mov rdx, msg_len
    syscall

    ; PART 1
    mov rax, rbx
    call solve_part1
    mov rbx, rax

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, solved_part1
    mov rdx, solved_part1_len
    syscall

    mov rax, rbx
    call print_number
    call print_newline

    pop rbx

    ; PART 2
    mov rax, rbx
    call solve_part2
    mov rbx, rax

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, solved_part2
    mov rdx, solved_part2_len
    syscall

    mov rax, rbx
    call print_number
    call print_newline
    ret
