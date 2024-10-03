;  day4.asm  Advent of Code Day 4
;
; Assemble: nasm -f elf64 day4.asm
; Link:     ld -o aoc day4.o ...

%include "utils.inc"

bits 64
default rel

section .rodata
    header: db "----- DAY 4 -----", 10, 0
    header_len: equ $ - header

    solved_part1:     db "Part 1 result: ", 0
    solved_part1_len: equ $ - solved_part1

    solved_part2:     db "Part 2 result: ", 0
    solved_part2_len: equ $ - solved_part2


section .text
    global solve_day4

    extern print_output
    extern print_newline
    extern print_number


; Clears the winning number map with zeros.
; Parameters:
;     [in, out] u8* - Pointer to the map.
;     [in] u64      - Size of the map (in bytes).
; Results:
;     None.
zero_clear_map:
    mov rdx, 0

.map_clear_loop:
    cmp rdx, rbx
    jge .map_cleared
    mov BYTE [rax + rdx], 0
    inc rdx
    jmp .map_clear_loop

.map_cleared:
    ret


; Sets all elements within the cards map to one.
; Parameters:
;     [in, out] u8* - Pointer to the map.
;     [in] u64      - Number of elements in the map (4 bytes per element).
; Results:
;     None.
set_one_map:
    mov rdx, 0

.map_clear_loop:
    cmp rdx, rbx
    jge .map_cleared
    mov DWORD [rax + rdx * 4], 1
    inc rdx
    jmp .map_clear_loop

.map_cleared:
    ret


; Solves AoC day 4.
; Parameters:
;     [in] Arena* - Pointer to the arena allocator.
;     [in] u8*    - Pointer to the input buffer.
;     [in] u64    - Input buffer size.
; Results:
;     TODO: [out] u64 - Part 1 solution.
;     TODO: [out] u64 - Part 2 solution.
solve_day4:
    push rbp
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9

    %define cards_map_start 0
    %define cards_map_count 300
    %define cards_map_size  cards_map_count * 4

    %define winning_map_start cards_map_size
    %define winning_map_size  200

    %define total_size winning_map_size + cards_map_size

    mov rbp, rsp
    sub rsp, total_size

    mov r15, rbx ; Input buffer.
    mov r14, rcx ; Input buffer size.
    mov r13, 0   ; Current buffer offset.
    mov r12, 0   ; Player score for a given card.
    mov r11, 0   ; Winning numbers count for a given card.
    mov r10, 0   ; Current card number.
    mov r9,  0   ; Total number of points.

    lea rax, [rsp + winning_map_start]
    mov rbx, winning_map_size
    call zero_clear_map

    lea rax, [rsp + cards_map_start]
    mov rbx, cards_map_count
    call set_one_map

    %define PARSING_CARD_NAME    0
    %define PARSING_WINNING_HAND 1
    %define PARSING_PLAYER_HAND  2

    mov rdi, PARSING_CARD_NAME ; Parsing state.
    mov rsi, 0                 ; Parsed number.

.input_loop:
    cmp r13, r14
    jge .input_loop_exit

    mov rax, 0 ; DEBUG
    mov al, [r15 + r13]
    inc r13

    cmp al, 10
    je .end_of_line

    cmp al, ':'
    je .end_of_card_name

    cmp rdi, PARSING_CARD_NAME
    je .input_loop

    cmp al, '|'
    je .end_of_winning_cards

    cmp al, ' '
    je .skip_whitespace

    cmp al, '0'
    jl .input_loop ; TOOD: Error case
    cmp al, '9'
    jg .input_loop ; TODO: Error case

    mov rcx, 0
    mov cl, al
    sub cl, '0'
    imul rsi, 10
    add rsi, rcx
    jmp .input_loop


    ; TODO: Check for state changes, where the only allowed changes are:
    ;       PARSING_CARD_NAME -> PARSING_WINNING_HAND -> PARSING_PLAYER_HAND (per one line)
.skip_whitespace:
    cmp rsi, 0
    je .input_loop

    cmp rdi, PARSING_WINNING_HAND
    je .handle_winning_number

    cmp rdi, PARSING_PLAYER_HAND
    je .handle_player_number

    ; TODO: Assert false, unknown program state

.handle_winning_number:
    ; TODO: Check if rsi is out of the map boundaries.
    ; Mark number as encounterd in the "winning_map".
    inc BYTE [rsp + winning_map_start + rsi]
    mov rsi, 0 
    jmp .input_loop

.handle_player_number:
    cmp BYTE [rsp + winning_map_start + rsi], 0
    je .not_a_winning_number

    inc r11

    shl r12, 1
    mov rax, 1
    test r12, r12
    cmove r12, rax

.not_a_winning_number:
    mov rsi, 0 
    jmp .input_loop


.end_of_winning_cards:
    mov rdi, PARSING_PLAYER_HAND
    mov rsi, 0 
    jmp .input_loop


.end_of_card_name:
    mov rdi, PARSING_WINNING_HAND
    jmp .input_loop


.end_of_line:
    cmp BYTE [rsp + winning_map_start + rsi], 0
    je .not_a_winning_number2

    inc r11

    shl r12, 1
    mov rax, 1
    test r12, r12
    cmove r12, rax
.not_a_winning_number2:

    mov edx, [rsp + cards_map_start + r10 * 4]
    mov rax, 0
.iterate_won_numbers_loop:
    cmp rax, r11
    jge .iterate_won_exit_loop
    lea rcx, [r10 + rax + 1]
    add DWORD [rsp + cards_map_start + rcx * 4], edx
    inc rax
    jmp .iterate_won_numbers_loop
.iterate_won_exit_loop:

    ; Reset number map
    lea rax, [rsp + winning_map_start]
    mov rbx, winning_map_size
    call zero_clear_map

    add r9, r12
    mov r12, 0
    mov r11, 0
    inc r10
    mov rdi, PARSING_CARD_NAME
    mov rsi, 0 
    jmp .input_loop

.input_loop_exit:
    mov rax, 0
    mov rdx, 0
.iterate_cards_map_loop:
    cmp rax, r10
    jge .iterate_cards_map_exit
    add edx, DWORD [rsp + cards_map_start + rax * 4]
    inc rax
    jmp .iterate_cards_map_loop
.iterate_cards_map_exit:

    mov r12, rdx

    call print_newline
    mov rax, header
    mov rbx, header_len
    call print_output

    mov rax, solved_part1 
    mov rbx, solved_part1_len
    call print_output
    mov rax, r9
    call print_number
    call print_newline

    mov rax, solved_part2 
    mov rbx, solved_part2_len
    call print_output
    mov rax, r12
    call print_number
    call print_newline

    mov rsp, rbp

    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    pop rbp
    ret
