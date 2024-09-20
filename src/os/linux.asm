;  linux.asm  Operating System specific implementations.

%include "os/linux.inc"

bits 64
default rel

section .text
    global exit_program
    global print_output
    global print_error
    global map_memory_pages
    global unmap_memory_pages
    global open_file
    global close_file
    global get_file_size
    global read_file


; Exit the program
; Input:
;     None.
; Output:
;     None.
exit_program:
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
    ud2


; Prints a string to standard output.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     None.
print_output:
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, STDOUT
    mov rax, SYS_WRITE
    syscall
    ret


; Prints a string to standard error.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     None.
print_error:
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, STDERR
    mov rax, SYS_WRITE
    syscall
    ret


; Map virtual memory pages.
; Input:
;     rdi - Minimium number of bytes.
; Output:
;     rax - Pointer to the mapped pages.
map_memory_pages:
    ; Should rcx be aligned to the page size?
    mov rcx, rdi

    mov rax, SYS_MMAP                     ; syscall code
    mov rdi, 0                            ; address, 0 for "random" one
    mov rsi, rcx                          ; number of bytes to allocate
    mov rdx, PROT_READ | PROT_WRITE       ; protection level
    mov r10, MAP_PRIVATE | MAP_ANONYMOUS  ; flags
    mov r8, -1                            ; file descriptor
    mov r9,  0                            ; offset
    syscall

    ; TODO: Check if mmap failed?
    ret


; Unmap virtual memory pages.
; Input:
;     rdi - Pointer to mapped virtual memory.
;     rsi - Size of the mapped memory.
; Output:
;     None.
unmap_memory_pages:
    mov rax, SYS_MUNMAP
    syscall
    ret


; Open file on a filesystem.
; Input:
;     rdi - Path to a file.
; Output:
;     rax - File handle.
open_file:
    mov rax, SYS_OPEN
    mov rdi, rdi
    mov rsi, O_RDONLY
    syscall
    ret 


; Close previously opened file.
; Input:
;     rdi - File handle.
; Output:
;     None.
close_file:
    mov rax, SYS_CLOSE
    mov rdi, rdi
    syscall
    ret


; Get size of the file.
; Input:
;     rdi - File handle.
; Output:
;     rax - Size of the file
get_file_size:
    push rbp
    mov rbp, rsp
    sub rsp, stat_size
    mov rax, SYS_FSTAT
    mov rdi, rdi
    mov rsi, rsp
    syscall
    mov rax, [rsp + stat.st_size]
    mov rsp, rbp
    pop rbp
    ret


; Read data from a file.
; Input:
;     rdi - File handle.
;     rsi - Pointer to a buffer to write to.
;     rdx - Number of bytes to read.
; Output:
;     rsi - Pointer to buffer where the bytes were written.
read_file: 
    mov rax, SYS_READ
    mov rdi, rdi
    mov rsi, rsi
    mov rdx, rdx
    syscall
    ret
