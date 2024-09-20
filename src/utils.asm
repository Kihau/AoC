;  utils.asm  System I/O and utility functions

%include "utils.inc"

bits 64
default rel

section .rodata
    newline_char: db 10

section .text
    global read_entire_file
    global print_number
    global print_newline
    global string_starts_with
    global number_to_string
    global arena_create
    global arena_destroy
    global arena_reset
    global arena_allocate
    global arena_clear
    global max

    extern print_output
    extern open_file
    extern get_file_size
    extern read_file
    extern close_file
    extern map_memory_pages
    extern unmap_memory_pages


; Read an entire file to a memory.
; Input:
;     rdi - Pointer to the arena allocator.
;     rsi - Path to a file.
; Output:
;     rax - Output buffer.
read_entire_file:
    push r15 ; Stores arena.
    push r14 ; Stores file handle.
    push r13 ; File size.

    mov r15, rdi

    mov rdi, rsi
    call open_file
    mov r14, rax

    mov rdi, r14
    call get_file_size
    mov r13, rax

    mov rdi, r15
    mov rsi, r13
    call arena_allocate

    mov rdi, r14
    mov rsi, rax
    mov rdx, r13
    call read_file

    ; Arena won't be needed now - swap it with the pointer to the buffer
    mov r15, rsi

    mov rdi, r14
    call close_file

    mov rax, r15
    mov rdx, r13

    pop r13
    pop r14
    pop r15
    ret

; Compare two strings and see if the first string starts with the seconds string.
; Input:
;     rdi - Pointer to string 1.
;     rsi - Pointer to string 2.
; Output:
;     rax - 0 when no match, 1 when match was found.
string_starts_with:
    ; If we iterated to the end ('\0') of the string 2, that means that we found a match.
    ; If we iterated to the end ('\0') of the string 1, that means that there is no match.
    ; If any of the compared characters don't match, that means that there is no match.

    mov r8, rdi
    mov r9, rsi

    ; Iteration counter, used as string character lookup offset.
    mov r10, 0

.substring_loop:
    ; Clear for debugging purposes.
    mov rax, 0
    mov al, [r9 + r10]
    cmp al, 0
    je .strings_matching

    ; Clear for debugging purposes.
    mov rdx, 0
    mov dl, [r8 + r10]
    cmp dl, 0
    je .strings_not_matching

    cmp al, dl
    jne .strings_not_matching

    inc r10
    jmp .substring_loop
    
.strings_not_matching:
    mov rax, 0
    ret

.strings_matching:
    mov rax, 1
    ret


; Compare two number and return bigger one.
; Input:
;     rdi - First number.
;     rsi - Second number.
; Output:
;     rax - Bigger number.
max:
    cmp rdi, rsi
    jl .second_larger
    mov rax, rdi
    ret
.second_larger:
    mov rax, rsi
    ret


; Create new arena allocator.
; Input:
;     rdi - Pointer to uninitialized Arena struct.
; Output:
;     rax - Initialized arena allocator.
arena_create:
    push r12

    %define ARENA_SIZE 2 * 1024 * 1024 * 1024
    mov rcx, ARENA_SIZE

    mov r12, rdi
    mov QWORD [r12 + Arena.total_size], rcx
    mov QWORD [r12 + Arena.position],   0

    mov rdi, ARENA_SIZE
    call map_memory_pages
    mov [r12 + Arena.base_ptr], rax

    mov rax, r12
    pop r12
    ret


; Destroy an arena allocator. Unmap all virtual memory pages.
; Input:
;     rdi - Pointer to arena allocator.
; Output:
;     None.
arena_destroy:
    push r12
    mov r12, rdi

    mov rdi, [r12 + Arena.base_ptr]
    mov rsi, [r12 + Arena.total_size]
    call unmap_memory_pages

    mov QWORD [r12 + Arena.base_ptr], 0
    mov QWORD [r12 + Arena.total_size], 0
    mov QWORD [r12 + Arena.position], 0

    pop r12
    ret


; Reset the arena. Unmapping all allocated pages and map new ones.
; Input:
;     rdi - Pointer to an arena allocator.
; Output:
;     None.
arena_reset:
    push rbx
    mov rbx, rdi ; Preserve
    call arena_destroy
    mov rdi, rbx
    call arena_create
    pop rbx
    ret


; Allocate rdi number of bytes.
; Input:
;     rdi - Pointer to an arena allocator.
;     rsi - Size in bytes to allocate.
; Output:
;     rax - Pointer to allocated data. Set to 0 if the allocation failed.
arena_allocate:
    mov r9,  [rdi + Arena.position]
    mov r10, [rdi + Arena.base_ptr]
    add r10, r9
    add r9, rsi
    cmp r9, [rdi + Arena.total_size]
    jl .not_out_of_bounds
    xor rax, rax
    ret

.not_out_of_bounds:
    mov [rdi + Arena.position], r9
    mov rax, r10
    ret


; Clear the arena without unmapping any allocated memory.
; Input:
;     rdi - Pointer to an arena allocator.
; Output:
;     None.
arena_clear:
    mov QWORD [rdi + Arena.position], 0
    ret


; Convert input string to a number.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     rax - Converted number.
string_to_number:
    ret


; Convert zero-terminated input string to a number.
; Input:
;     rdi - Zero-terminated input string.
; Output:
;     rax - Converted number.
cstring_to_number:
    ret


; Convert input number to a string.
; Input:
;     rdi - The number to convert.
;     rsi - Pointer to Number_String struct. The output will be placed here.
; Output:
;     rsi - Parsed number into Number_String struct.
number_to_string:
    ; Current buffer write offset
    xor r9, r9

    mov rax, rdi ; Dividend.
    mov rcx, 10  ; Divisor.

.loop_number_convert:
    ; Clear the dividend.
    xor rdx, rdx
    div rcx
    ; rax - Result of the divsion.
    ; rdx - Divison reminder.

    add rdx, '0'
    mov [rsi + Number_String.buffer + r9], dl
    inc r9

    cmp rax, 0
    jne .loop_number_convert

    ; Set string length and add null terminator.
    lea rax, [r9]
    mov [rsi + Number_String.len], al
    mov BYTE [rsi + Number_String.buffer + rax], 0

    ; Clear the left iterator.
    xor r8, r8
    dec r9
.loop_string_inverse:
    mov al, [rsi + Number_String.buffer + r8]
    mov dl, [rsi + Number_String.buffer + r9]
    mov [rsi + Number_String.buffer + r8], dl
    mov [rsi + Number_String.buffer + r9], al

    inc r8
    dec r9
    
    cmp r8, r9
    jl .loop_string_inverse
    ret


; Prints 64 bit number into STDOUT.
; Input:
;     rdi - Number to be printed.
; Output:
;     None.
print_number:
    sub rsp, Number_String_size

    mov rsi, rsp
    call number_to_string
    lea r8,  [rsi + Number_String.buffer]
    xor r9, r9
    mov r9b, [rsi + Number_String.len]

    mov rdi, r8
    mov rsi, r9
    call print_output

    add rsp, Number_String_size
    ret


; Prints newline character into STDOUT.
; Input:
;     None.
; Output:
;     None.
print_newline:
    mov rdi, newline_char
    mov rsi, 1
    call print_output
    ret
