; Linux system calls:
;
; Call register:
;     rax
; Argument registers order:
;     rdi, rsi, rdx, r10, r8, r9
; Return registers:
;     rdx, rdx

; C calling conventions:
;
; Function paramater register order:
;     rdi, rsi, rdx, rcx, r8, r9
; Return registers:
;     rax, rdx
; Scratch registers:
;     rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
; Preserved registers:
;     rbx, rsp, rbp, r12, r13, r14, r15

; Passed to the rax register

%include "defines.inc"

BITS 64

section .bss
    string_buffer_inv: resb 32
    string_buffer:     resb 32
    string_len:        resb 2

    statbuf: resb stat_size
    statbuf_size: equ $-stat

section .data
    newline: db 10

    debug_msg: db "DEBUG HIT", 10
    debug_len: equ $-debug_msg

    ; input_path: db "./inputs/kihau/day1.txt", 0
    ; input_len:  equ $-input_path

    heap_ptr: dq 0

    struc timespec 
        .tv_sec: resb 16
        .tv_nsec: resb 16
    endstruc

    struc stat
        .st_dev:     resb 8
        .st_ino:     resb 8
        .st_nlink:   resb 8

        .st_mode:    resb 4
        .st_uid:     resb 4
        .st_gid:     resb 4
        .pad0:       resb 4

        .st_rdev:    resb 8
        .st_size:    resb 8
        .st_blksize: resb 8
        .st_blocks:  resb 8

        ; .st_atim:    equ $-timespec_size
        ; .st_mtim:    equ $-timespec_size
        ; .st_ctim:    equ $-timespec_size
        .st_atim:    resb 16
        .st_mtim:    resb 16
        .st_ctim:    resb 16

        .pad1:       resb 24
    endstruc

section .text
    global print_number
    global print_newline
    global read_to_string

; Read an entire input file to string buffer.
; Input:
;     rax - File input path.
; Output:
;     rax - Null terminated input buffer. Set to 0 if the read failed.
read_to_string:
    ; Preserve
    push r12
    push r13

    mov r12, rax

    ; Open the input file here.
    mov rax, SYS_OPEN
    mov rdi, r12
    mov rsi, O_RDONLY
    syscall

    ; Storing the file descriptor to not overwrite it.
    mov r12, rax

    ; Get exact size of the input size (in bytes)
    mov rax, SYS_FSTAT
    mov rdi, r12
    mov rsi, statbuf ; I could put it on the stack
    syscall

    mov rax, [statbuf + stat.st_size]
    inc rax ; Add 1 to rax in order to fit string null terminator.
    call bump_allocate

    ; Save pointer to the heap.
    mov r13, rax

    ; Now we read an entire input file into memory
    mov rax, SYS_READ
    mov rdi, r12
    mov rsi, r13
    mov rdx, [statbuf + stat.st_size]
    syscall

    ; Should I null terminate the string or is this fine?

    ; TODO: Check if expected byte count was read from a file.

    ; All data is ready. Input file can be closed now.
    mov rax, SYS_CLOSE
    mov rdi, r12
    syscall

    mov rax, r13

    pop r13
    pop r12
    ret


; Map virtual memory pages.
; Input:
;     rax - Minimium number of bytes.
; Output:
;     rax - Pointer to the mapped page.
map_virtual_page:
    ; Should rcx be aligned to the page size?
    mov rcx, rax

    mov rax, SYS_MMAP  ; syscall code
    mov rdi, 0         ; address, 0 for "random" one
    mov rsi, rcx       ; number of bytes to allocate
    mov rdx, 3         ; protection level, here PROT_READ | PROT_WRITE
    mov r10, 34        ; flags, here MAP_PRIVATE | MAP_ANONYMOUS
    mov r8, -1         ; file descriptor
    mov r9,  0         ; offset
    syscall
    ret


; Allocates N number of bytes. Increases heap memory region size.
; Input:
;     rax - Number of bytes to allocate.
; Output:
;     rax - Pointer to allocated data.
bump_allocate:
    mov rsi, rax
    mov rdx, [heap_ptr]

    cmp rdx, 0
    jne .heap_initialized
    mov rax, SYS_BRK
    xor rdi, rdi
    syscall
    mov [heap_ptr], rax
.heap_initialized:

    mov rdx, [heap_ptr]
    add [heap_ptr], rsi

    mov rax, SYS_BRK
    mov rdi, [heap_ptr]
    syscall

    ; Check if the syscall failed.
    cmp rax, -1
    je .brk_failed
    mov rax, rdx
.brk_failed:
    ret


debug_hit_print:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, debug_msg
    mov rdx, debug_len
    syscall
    ret


; Convert input number to a string.
; Input:
;     rax - The number to convert.
; Output:
;     string_buffer - Buffer with converted string.
;     string_len    - Length of the converted string.
number_to_string:
    ; Current buffer write offset
    mov r8, 0

    ; mov rdx, rax
    mov rcx, 10

.loop_convert:
    mov rdx, 0
    div rcx

    add rdx, 48
    mov [string_buffer_inv + r8], rdx
    inc r8

    cmp rax, 0
    jne .loop_convert

    ; Now we set up decrement register
    mov rax, r8

    ; Reset buffer write offset
    mov r8, 0

.loop_reverse:
    dec rax

    mov rdx, [string_buffer_inv + rax]
    mov [string_buffer + r8], rdx
    inc r8

    cmp rax, 0
    jne .loop_reverse

    ; null terminate the string
    mov rdx, 0
    mov [string_buffer + r8], rdx
    inc r8
    mov [string_len], r8
    ret


; Prints 64 bit number into STDOUT.
; Input:
;     rax - Number to be printed.
; Output:
;     none
print_number:
    call number_to_string
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, string_buffer
    mov rdx, [string_len]
    syscall
    ret


; Prints newline character into STDOUT.
; Input:
;     none
; Output:
;     none
print_newline:
    call number_to_string
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, newline
    mov rdx, 1
    syscall
    ret


; Testing some stuff here
debug_testing:
    BREAKPOINT
    ret
