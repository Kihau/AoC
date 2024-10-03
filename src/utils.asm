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


; WORK IN PROGRESS
; Copy two non overlapping buffer to another buffer
memory_copy:
    ud2

; rax, rbx, rcx, rdx, rsi, rdi

; Read an entire file to a memory.
; Parameters:
;     [in] u8* - Pointer to the arena allocator.
;     [in] u8* - Path to a file.
; Returns:
;     [out] u8* - Output buffer.
;     [out] u64 - Size of the buffer.
read_entire_file:
    push r15 ; Stores arena.
    push r14 ; Stores file handle.
    push r13 ; File size.

    mov r15, rax

    mov rax, rbx
    call open_file
    ; TODO: Check if succeeded
    mov r14, rdi

    mov rax, r14
    call get_file_size
    mov r13, rdi

    mov rax, r15
    mov rbx, r13
    call arena_allocate

    mov rax, r14
    mov rbx, rdi
    mov rcx, r13
    call read_file
    ; TODO: Check wheter entire files was read to the buffer.

    ; Arena won't be needed now - swap it with a pointer to the buffer
    mov r15, rbx

    mov rax, r14
    call close_file

    mov rdi, r15
    mov rsi, r13

    pop r13
    pop r14
    pop r15
    ret

; Compare two strings and see if the first string starts with the seconds string.
; Parameters:
;     [in] u8* - Pointer to string 1.
;     [in] u8* - Pointer to string 2.
; Returns:
;     [out] u64 - 0 when no match, 1 when match was found.
string_starts_with:
    push r15
    push r14
    push r13
    ; If we iterated to the end ('\0') of the string 2, that means that we found a match.
    ; If we iterated to the end ('\0') of the string 1, that means that there is no match.
    ; If any of the compared characters don't match, that means that there is no match.

    mov r13, rax ; Pointer to string 1.
    mov r14, rbx ; Pointer to string 2.

    ; Iteration counter, used as string character lookup offset.
    mov r15, 0

.substring_loop:
    ; Clear for debugging purposes.
    mov rax, 0
    mov al, [r14 + r15]
    cmp al, 0
    je .strings_matching

    ; Clear for debugging purposes.
    mov rdx, 0
    mov dl, [r13 + r15]
    cmp dl, 0
    je .strings_not_matching

    cmp al, dl
    jne .strings_not_matching

    inc r15
    jmp .substring_loop
    
.strings_not_matching:
    mov rdi, 0
    pop r13
    pop r14
    pop r15
    ret

.strings_matching:
    mov rdi, 1
    pop r13
    pop r14
    pop r15
    ret


; Compare two number and return bigger one.
; Parameters:
;     [in] u64 - First number.
;     [in] u64 - Second number.
; Returns:
;     [out] u64 - Bigger number.
max:
    cmp rax, rbx
    jl .second_larger
    mov rdi, rax
    ret
.second_larger:
    mov rdi, rbx
    ret


; Create new arena allocator.
; Parameters:
;     [out] Arena* - Pointer to an empty buffer where Arena struct will be placed.
; Returns:
;     None.
arena_create:
    push r15
    mov r15, rax ; Preserve

    %define ARENA_SIZE 2 * 1024 * 1024 * 1024
    mov rax, ARENA_SIZE
    mov QWORD [r15 + Arena.total_size], rax
    mov QWORD [r15 + Arena.position],   0
    call map_memory_pages
    mov [r15 + Arena.base_ptr], rdi

    pop r15
    ret


; Destroy an arena allocator. Unmap all virtual memory pages.
; Parameters:
;     [in, out] Arena* - Pointer to arena allocator.
; Returns:
;     None.
arena_destroy:
    push r15
    mov r15, rax

    mov rax, [r15 + Arena.base_ptr]
    mov rbx, [r15 + Arena.total_size]
    call unmap_memory_pages

    mov QWORD [r15 + Arena.base_ptr],   0
    mov QWORD [r15 + Arena.total_size], 0
    mov QWORD [r15 + Arena.position],   0

    pop r15
    ret


; Reset the arena. Unmapping all allocated pages and map new ones.
; Parameters:
;     [in, out] Arena* - Pointer to an arena allocator.
; Returns:
;     None.
arena_reset:
    push r15
    mov r15, rax ; Preserve
    call arena_destroy
    mov rax, r15
    call arena_create
    pop r15
    ret


; Allocate rdi number of bytes.
; Parameters:
;     [in, out] Arena* - Pointer to an arena allocator.
;     [in]      u64    - Size in bytes to allocate.
; Returns:
;     [out] - Pointer to allocated data. Set to 0 if the allocation failed.
arena_allocate:
    push r15
    push r14
    mov r14, [rax + Arena.position]
    mov r15, [rax + Arena.base_ptr]
    add r15, r14
    add r14, rbx
    cmp r14, [rax + Arena.total_size]
    jl .not_out_of_bounds
    xor rdi, rdi
    jmp .arena_allocate_exit

.not_out_of_bounds:
    mov [rax + Arena.position], r14
    mov rdi, r15

.arena_allocate_exit:
    pop r14
    pop r15
    ret


; Clear the arena without unmapping any allocated memory.
; Parameters:
;     [in, out] Arena* - Pointer to an arena allocator.
; Returns:
;     None.
arena_clear:
    mov QWORD [rax + Arena.position], 0
    ret


; Convert input string to a number.
; Parameters:
;     [in] u8* - Input string.
;     [in] u64 - Size of the string.
; Returns:
;     [out] u64 - Converted number.
string_to_number:
    ret


; Convert zero-terminated input string to a number.
; Parameters:
;     [in] u8* - Zero-terminated input string.
; Returns:
;     [out] u64 - Converted number.
cstring_to_number:
    ret


; Convert input number to a string.
; Parameters:
;     [in] u64             - The number to convert.
;     [out] Number_String* - Pointer to Number_String struct. The output will be placed here.
; Returns:
;     None.
number_to_string:
    push r15
    push r14
    push r13

    mov r15, rax

    ; Current buffer write offset
    xor r14, r14

    mov rax, r15 ; Dividend.
    mov rcx, 10  ; Divisor.

.loop_number_convert:
    ; Clear the dividend.
    xor rdx, rdx
    div rcx
    ; rax - Result of the divsion.
    ; rdx - Divison reminder.

    add rdx, '0'
    mov [rbx + Number_String.buffer + r14], dl
    inc r14

    cmp rax, 0
    jne .loop_number_convert

    ; Set string length and add null terminator.
    lea rax, [r14]
    mov [rbx + Number_String.len], al
    mov BYTE [rbx + Number_String.buffer + rax], 0

    ; Clear the left iterator.
    xor r13, r13
    dec r14
.loop_string_inverse:
    mov al, [rbx + Number_String.buffer + r13]
    mov dl, [rbx + Number_String.buffer + r14]
    mov [rbx + Number_String.buffer + r13], dl
    mov [rbx + Number_String.buffer + r14], al

    inc r13
    dec r14
    
    cmp r13, r14
    jl .loop_string_inverse
    pop r13
    pop r14
    pop r15
    ret


; Prints 64 bit number into STDOUT.
; Parameters:
;     [in] u64 - Number to be printed.
; Returns:
;     None.
print_number:
    push r15
    push r14
    sub rsp, Number_String_size

    mov rbx, rsp
    call number_to_string
    lea r14, [rbx + Number_String.buffer]
    xor r15, r15
    mov r15b, [rbx + Number_String.len]

    mov rax, r14
    mov rbx, r15
    call print_output

    add rsp, Number_String_size
    pop r14
    pop r15
    ret


; Prints newline character into STDOUT.
; Parameters:
;     None.
; Returns:
;     None.
print_newline:
    mov rax, newline_char
    mov rbx, 1
    call print_output
    ret
