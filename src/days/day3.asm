;  day3.asm  Advent of Code Day 3
;
; Assemble: nasm -f elf64 day3.asm
; Link:     ld -o aoc day3.o ...

%include "utils.inc"

bits 64
default rel

section .rodata
    header: db "----- DAY 3 -----", 10, 0
    header_len: equ $ - header

    solved_part1:     db "Part 1 result: ", 0
    solved_part1_len: equ $ - solved_part1

    solved_part2:     db "Part 2 result: ", 0
    solved_part2_len: equ $ - solved_part2

    debug:     db "debug print: ", 0
    debug_len: equ $ - debug


section .bss
    ; TODO: Place those on da stack.
    board: resq 1
    board_width: resq 1
    board_size: resq 1


section .text
    global solve_day3

    extern print_output
    extern print_number
    extern print_newline


; Calculates position from a given index.
; Input:
;     rdi - Board index.
; Output:
;     rax - Position on X axis. Returns -1 on error.
;     rdx - Position on Y axis. Returns -1 on error.
index_to_pos:
    xor rax, rax
    ret


; Parse number that a given index points to.
; Steps back if index is not the middle of the number and parses it.
; Given starting index MUST point to a digit.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Parsed number.
parse_number_at_pos:
    push r12
    push r13
    push r14

    ; Position X
    mov r12, rdi
    ; Position Y
    mov r13, rsi
    ; Parsed number.
    mov r14, 0

.loop_find_start:
    mov rdi, r12
    mov rsi, r13
    call byte_at_pos
    cmp rax, '0'
    jl .parse_number
    cmp rax, '9'
    jg .parse_number
    dec r12
    jmp .loop_find_start

.parse_number:
    inc r12
    mov rdi, r12
    mov rsi, r13
    call byte_at_pos
    cmp rax, '0'
    jl .return_number
    cmp rax, '9'
    jg .return_number

    sub rax, '0'
    imul r14, 10
    add r14, rax
    jmp .parse_number

.return_number:
    mov rax, r14

    pop r14
    pop r13
    pop r12
    ret

; Calculates index from a given position.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Board index.
pos_to_index:
    mov rax, rsi
    imul rax, [board_width]
    add rax, rdi
    ret

; Find how many numbers are adjacent to a symbol at a given position.
adjacent_numbers_count:
    ret

; Calculate sum of all number adjacent to symbol at given position.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Calculated sum of adjacent numbers.
adjacent_numbers_sum:
    push r12
    push r13
    push r14

    ; Stores position X
    mov r12, rdi

    ; Stores position Y
    mov r13, rsi

    ; Stores sum of adjacent numbers.
    xor r14, r14

    ; Check if number is up - middle.
    mov r9,  r12
    mov r10, r13
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    ; cmp rax, -1
    ; je .check_middle
    cmp rax, '0'
    jl .check_up_left
    cmp rax, '9'
    jg .check_up_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

    jmp .check_middle_left

.check_up_left:
    mov r9,  r12
    mov r10, r13
    dec r9
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_up_right
    cmp rax, '9'
    jg .check_up_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.check_up_right:
    mov r9,  r12
    mov r10, r13
    inc r9 
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_middle_left
    cmp rax, '9'
    jg .check_middle_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.check_middle_left:
    mov r9,  r12
    mov r10, r13
    dec r9

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_middle_right
    cmp rax, '9'
    jg .check_middle_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.check_middle_right:
    mov r9,  r12
    mov r10, r13
    inc r9

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_down_middle
    cmp rax, '9'
    jg .check_down_middle

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.check_down_middle:
    mov r9,  r12
    mov r10, r13
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    ; cmp rax, -1
    ; je .return_result
    cmp rax, '0'
    jl .check_down_left
    cmp rax, '9'
    jg .check_down_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax
    jmp .return_result

.check_down_left:
    mov r9,  r12
    mov r10, r13
    dec r9
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_down_right
    cmp rax, '9'
    jg .check_down_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.check_down_right:
    mov r9,  r12
    mov r10, r13
    inc r9
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .return_result
    cmp rax, '9'
    jg .return_result

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    add r14, rax

.return_result:
    mov rax, r14
    pop r14
    pop r13
    pop r12
    ret

; NOTE: Could be merged with the function above. 
;       Function could return two arguments instead of one -
;       - rax for sum of all and rdx for multiple of two.
;
; Calculate multiple of two adjacent numbers to symbol at given position.
; If there are more than two adjacent numbers - returns 0.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Calculated multiple of the two adjacent numbers.
adjacent_numbers_mul:
    push r12
    push r13
    push r14
    push r15

    ; Stores position X
    mov r12, rdi

    ; Stores position Y
    mov r13, rsi

    ; Stores multiple of adjacent two numbers.
    mov r14, 1

    ; Store how many numbers have been parsed
    xor r15, r15

    ; Check if number is up - middle.
    mov r9,  r12
    mov r10, r13
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    ; cmp rax, -1
    ; je .check_middle
    cmp rax, '0'
    jl .check_up_left
    cmp rax, '9'
    jg .check_up_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15

    jmp .check_middle_left

.check_up_left:
    mov r9,  r12
    mov r10, r13
    dec r9
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_up_right
    cmp rax, '9'
    jg .check_up_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    cmp r15, 2
    je .return_result

.check_up_right:
    mov r9,  r12
    mov r10, r13
    inc r9 
    dec r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_middle_left
    cmp rax, '9'
    jg .check_middle_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    cmp r15, 2
    je .return_result

.check_middle_left:
    mov r9,  r12
    mov r10, r13
    dec r9

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_middle_right
    cmp rax, '9'
    jg .check_middle_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    cmp r15, 2
    je .return_result

.check_middle_right:
    mov r9,  r12
    mov r10, r13
    inc r9

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_down_middle
    cmp rax, '9'
    jg .check_down_middle

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    cmp r15, 2
    je .return_result

.check_down_middle:
    mov r9,  r12
    mov r10, r13
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    ; cmp rax, -1
    ; je .return_result
    cmp rax, '0'
    jl .check_down_left
    cmp rax, '9'
    jg .check_down_left

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    jmp .return_result

.check_down_left:
    mov r9,  r12
    mov r10, r13
    dec r9
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .check_down_right
    cmp rax, '9'
    jg .check_down_right

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15
    cmp r15, 2
    je .return_result

.check_down_right:
    mov r9,  r12
    mov r10, r13
    inc r9
    inc r10

    mov rdi, r9
    mov rsi, r10
    call byte_at_pos
    cmp rax, '0'
    jl .return_result
    cmp rax, '9'
    jg .return_result

    mov rdi, r9
    mov rsi, r10
    call parse_number_at_pos
    imul r14, rax
    inc r15

.return_result:
    cmp r15, 2
    jne .return_zero_instead
    mov rax, r14
    jmp .pop_everything
.return_zero_instead:
    xor rax, rax

.pop_everything:
    pop r15
    pop r14
    pop r13
    pop r12
    ret

; Calculates byte at a given position.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Byte at given position. Returns -1 on out of bounds.
byte_at_pos:
    cmp rdi, [board_width]
    jge .out_of_bounds

    mov rax, rsi
    imul rax, [board_width]
    add rax, rdi

    cmp rax, 0
    jl .out_of_bounds
    cmp rax, [board_size]
    jge .out_of_bounds

    xor rdx, rdx
    mov rsi, [board]
    mov dl, [rsi + rax]
    cmp dl, 0
    je .out_of_bounds
    cmp dl, 10
    je .out_of_bounds

    mov rax, rdx
    ret

.out_of_bounds:
    mov rax, -1
    ret


; Calculates width of the board from input for day 3
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     rax - Calculated board width.
find_board_width:
    xor rax, rax

.not_new_line:
    mov dl, [rdi + rax]
    inc rax,
    cmp dl, 0
    je .found_newline
    cmp dl, 10
    je .found_newline
    jmp .not_new_line

.found_newline:
    ret

; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
;     rsi - Size of the input.
; Output:
;     None.
solve_day3:
    push r15
    push r14
    push r13
    push r12

    mov [board], rdi
    mov [board_size], rsi

    mov rdi, [board]
    call find_board_width
    mov [board_width], rax

    ; mov rdi, [board_width]
    ; call print_number
    ; call print_newline

    ; Current position X.
    xor r15, r15

    ; Current position Y.
    xor r14, r14

    ; Current index offset
    xor r13, r13

    ; Sum of all numbers adjacent to all symbol (part 1).
    xor r12, r12

    ; Sum of all calculated values for the '*' symbol (part 2).
    xor r8, r8
    push r8
    
.board_loop:
    ; PRINTLN_WITH_NUMBER debug, debug_len, r15
    ; PRINTLN_WITH_NUMBER debug, debug_len, r14
    ; PRINTLN_WITH_NUMBER debug, debug_len, r13
    ; PRINTLN_WITH_NUMBER debug, debug_len, r12
    ; call print_newline

    mov rax, [board]
    xor rbx, rbx
    mov bl, [rax + r13]

    cmp bl, 0
    je .board_end

    cmp bl, 10
    je .board_advance_down

    cmp bl, '.'
    je .board_advance_right

    cmp bl, '0'
    jge .check_if_digit
.not_a_digit:

    ; Here we found an actual symbol, it's time to calculate sum of all adjacent numbers.
    mov rdi, r15
    mov rsi, r14
    call adjacent_numbers_sum
    add r12, rax

    cmp bl, '*'
    jne .board_advance_right
    mov rdi, r15
    mov rsi, r14
    call adjacent_numbers_mul
    pop r8
    add r8, rax
    push r8

    ; DEBUG:
    ; mov r8, rax
    ; PRINTLN_WITH_NUMBER debug, debug_len, r8
    
    jmp .board_advance_right

.check_if_digit:
    cmp bl, '9'
    jle .board_advance_right
    jmp .not_a_digit
    
.board_advance_right:
    inc r13
    inc r15
    jmp .board_loop
    
.board_advance_down:
    inc r13
    inc r14
    xor r15, r15
    jmp .board_loop
    
.board_end:
    ; mov rdi, 141
    ; mov rsi, 0
    ; call byte_at_pos
    ; mov rdi, rax
    ; call print_number
    ; call print_newline

    ; mov rdi, r12
    ; call print_number
    ; call print_newline

    call print_newline
    mov rdi, header
    mov rsi, header_len
    call print_output

    mov rdi, solved_part1
    mov rsi, solved_part1_len
    call print_output
    mov rdi, r12
    call print_number
    call print_newline

    mov rdi, solved_part2 
    mov rsi, solved_part2_len
    call print_output
    pop r8
    mov rdi, r8
    call print_number
    call print_newline

    pop r12
    pop r13
    pop r14
    pop r15
    ret
