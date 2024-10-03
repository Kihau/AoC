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


; UNUSED
; Calculates position from a given index.
; Input:
;     rdi - Board index.
; Output:
;     rax - Position on X axis. Returns -1 on error.
;     rdx - Position on Y axis. Returns -1 on error.
; index_to_pos:
;     xor rax, rax
;     ret
; UNSUED:
; Calculates index from a given position.
; Input:
;     rdi - Position on X axis.
;     rsi - Position on Y axis.
; Output:
;     rax - Board index.
; pos_to_index:
;     mov rax, rsi
;     imul rax, [board_width]
;     add rax, rdi
;     ret
; UNSUED:
; Find how many numbers are adjacent to a symbol at a given position.
; adjacent_numbers_count:
;     ret


; Calculates byte at a given position.
; Parameters:
;     [in] u64 - Position on X axis.
;     [in] u64 - Position on Y axis.
; Results:
;     [out] u64 - Byte at given position. Returns -1 on out of bounds.
byte_at_pos:
    cmp rax, [board_width]
    jge .out_of_bounds

    mov rdi, rbx
    imul rdi, [board_width]
    add rdi, rax

    cmp rdi, 0
    jl .out_of_bounds
    cmp rdi, [board_size]
    jge .out_of_bounds

    xor rdx, rdx
    mov rbx, [board]
    mov dl, [rbx + rdi]
    cmp dl, 0
    je .out_of_bounds
    cmp dl, 10
    je .out_of_bounds

    mov rdi, rdx
    ret

.out_of_bounds:
    mov rdi, -1
    ret


; Parse number that a given index points to.
; Steps back if index is not the middle of the number and parses it.
; Given starting index MUST point to a digit.
; Parameters:
;     [in] u64 - Position on X axis.
;     [in] u64 - Position on Y axis.
; Results:
;     [out] u64 - Parsed number.
parse_number_at_pos:
    push r15
    push r14
    push r13

    mov r15, 0   ; Parsed number.
    mov r14, rbx ; Position Y
    mov r13, rax ; Position X

.loop_find_start:
    mov rax, r13
    mov rbx, r14
    call byte_at_pos
    cmp rdi, '0'
    jl .parse_number
    cmp rdi, '9'
    jg .parse_number
    dec r13
    jmp .loop_find_start

.parse_number:
    inc r13
    mov rax, r13
    mov rbx, r14
    call byte_at_pos
    cmp rdi, '0'
    jl .return_number
    cmp rdi, '9'
    jg .return_number

    sub rdi, '0'
    imul r15, 10
    add r15, rdi
    jmp .parse_number

.return_number:
    mov rdi, r15

    pop r13
    pop r14
    pop r15
    ret


; Calculate sum of all number adjacent to symbol at a given position.
; Parameters:
;     [in] u64 - Position on X axis.
;     [in] u64 - Position on Y axis.
; Results:
;     [out] u64 - Calculated sum of adjacent numbers.
adjacent_numbers_sum:
    push r15
    push r14
    push r13
    push r12
    push r11

    xor r15, r15 ; Stores sum of adjacent numbers.
    mov r14, rbx ; Stores position Y
    mov r13, rax ; Stores position X

    ; Check if number is up - middle.
    mov r12, r14
    mov r11, r13
    dec r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    ; cmp rax, -1
    ; je .check_middle
    cmp rdi, '0'
    jl .check_up_left
    cmp rdi, '9'
    jg .check_up_left

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

    jmp .check_middle_left

.check_up_left:
    mov r11, r13
    mov r12, r14
    dec r11
    dec r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .check_up_right
    cmp rdi, '9'
    jg .check_up_right

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.check_up_right:
    mov r11, r13
    mov r12, r14
    inc r11 
    dec r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .check_middle_left
    cmp rdi, '9'
    jg .check_middle_left

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.check_middle_left:
    mov r11, r13
    mov r12, r14
    dec r11

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .check_middle_right
    cmp rdi, '9'
    jg .check_middle_right

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.check_middle_right:
    mov r11, r13
    mov r12, r14
    inc r11

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .check_down_middle
    cmp rdi, '9'
    jg .check_down_middle

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.check_down_middle:
    mov r11, r13
    mov r12, r14
    inc r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    ; cmp rax, -1
    ; je .return_result
    cmp rdi, '0'
    jl .check_down_left
    cmp rdi, '9'
    jg .check_down_left

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi
    jmp .return_result

.check_down_left:
    mov r11, r13
    mov r12, r14
    dec r11
    inc r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .check_down_right
    cmp rdi, '9'
    jg .check_down_right

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.check_down_right:
    mov r11, r13
    mov r12, r14
    inc r11
    inc r12

    mov rax, r11
    mov rbx, r12
    call byte_at_pos
    cmp rdi, '0'
    jl .return_result
    cmp rdi, '9'
    jg .return_result

    mov rax, r11
    mov rbx, r12
    call parse_number_at_pos
    add r15, rdi

.return_result:
    mov rdi, r15

    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    ret


; NOTE: Could be merged with the function above. 
;       Function could return two arguments instead of one -
;       - One for sum of all and one for multiple of two.
;
; Calculate multiple of two adjacent numbers to symbol at given position.
; If there are more than two adjacent numbers - returns 0.
; Parameters:
;     [in] u64 - Position on X axis.
;     [in] u64 - Position on Y axis.
; Results:
;     [out] u64 - Calculated multiple of the two adjacent numbers.
adjacent_numbers_mul:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10

    xor r15, r15 ; Store how many numbers have been parsed.
    mov r14, 1   ; Stores multiple of adjacent two numbers.
    mov r13, rbx ; Stores position Y.
    mov r12, rax ; Stores position X.

    ; Check if number is up - middle.
    mov r10, r12
    mov r11, r13
    dec r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_up_left
    cmp rdi, '9'
    jg .check_up_left

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    jmp .check_middle_left

.check_up_left:
    mov r10, r12
    mov r11, r13
    dec r10
    dec r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_up_right
    cmp rdi, '9'
    jg .check_up_right

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    cmp r15, 2
    je .return_result

.check_up_right:
    mov r10, r12
    mov r11, r13
    inc r10 
    dec r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_middle_left
    cmp rdi, '9'
    jg .check_middle_left

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    cmp r15, 2
    je .return_result

.check_middle_left:
    mov r10, r12
    mov r11, r13
    dec r10

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_middle_right
    cmp rdi, '9'
    jg .check_middle_right

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    cmp r15, 2
    je .return_result

.check_middle_right:
    mov r10, r12
    mov r11, r13
    inc r10

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_down_middle
    cmp rdi, '9'
    jg .check_down_middle

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    cmp r15, 2
    je .return_result

.check_down_middle:
    mov r10, r12
    mov r11, r13
    inc r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_down_left
    cmp rdi, '9'
    jg .check_down_left

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    jmp .return_result

.check_down_left:
    mov r10, r12
    mov r11, r13
    dec r10
    inc r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .check_down_right
    cmp rdi, '9'
    jg .check_down_right

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15
    cmp r15, 2
    je .return_result

.check_down_right:
    mov r10, r12
    mov r11, r13
    inc r10
    inc r11

    mov rax, r10
    mov rbx, r11
    call byte_at_pos
    cmp rdi, '0'
    jl .return_result
    cmp rdi, '9'
    jg .return_result

    mov rax, r10
    mov rbx, r11
    call parse_number_at_pos
    imul r14, rdi
    inc r15

.return_result:
    cmp r15, 2
    jne .return_zero_instead
    mov rdi, r14
    jmp .pop_everything
.return_zero_instead:
    xor rdi, rdi

.pop_everything:
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    ret


; Calculates width of the board from input for day 3
; Parameters:
;     [in] u8* - Pointer to the input buffer.
; Results:
;     [out] u64 - Calculated board width.
find_board_width:
    push r15
    mov r15, rax
    xor rax, rax

.not_new_line:
    mov dl, [r15 + rax]
    inc rax,
    cmp dl, 0
    je .found_newline
    cmp dl, 10
    je .found_newline
    jmp .not_new_line

.found_newline:
    mov rdi, rax
    pop r15
    ret


; Solves AoC day 3.
; Parameters:
;     [in] Arena* - Pointer to the arena allocator.
;     [in] u8*    - Pointer to the input buffer.
;     [in] u64    - Input buffer size.
; Results:
;     TODO: [out] u64 - Part 1 solution.
;     TODO: [out] u64 - Part 2 solution.
solve_day3:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10

    mov [board], rbx
    mov [board_size], rcx

    mov rax, [board]
    call find_board_width
    mov [board_width], rdi

    xor r15, r15 ; Current position X.
    xor r14, r14 ; Current position Y.
    xor r13, r13 ; Current index offset
    xor r12, r12 ; Sum of all numbers adjacent to all symbol (part 1).
    xor r11, r11 ; Sum of all calculated values for the '*' symbol (part 2).
    xor r10, r10 ; Holds current board byte.
    
.board_loop:
    mov rax, [board]
    mov r10b, [rax + r13]

    cmp r10b, 0
    je .board_end

    cmp r10b, 10
    je .board_advance_down

    cmp r10b, '.'
    je .board_advance_right

    cmp r10b, '0'
    jge .check_if_digit
.not_a_digit:

    ; Here we found an actual symbol, it's time to calculate sum of all adjacent numbers.
    mov rax, r15
    mov rbx, r14
    call adjacent_numbers_sum
    add r12, rdi

    cmp r10b, '*'
    jne .board_advance_right
    mov rax, r15
    mov rbx, r14
    call adjacent_numbers_mul
    add r11, rdi
    jmp .board_advance_right

.check_if_digit:
    cmp r10b, '9'
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
    call print_newline
    mov rax, header
    mov rbx, header_len
    call print_output

    mov rax, solved_part1
    mov rbx, solved_part1_len
    call print_output
    mov rax, r12
    call print_number
    call print_newline

    mov rax, solved_part2 
    mov rbx, solved_part2_len
    call print_output
    mov rax, r11
    call print_number
    call print_newline

    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    ret
