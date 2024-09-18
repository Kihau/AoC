;
; Registers by width
; +--------------------------------+----------------+--------+--------+
; |             EAX                |       AX       |   AH   |   AL   |  <- 64 bit RAX
; +--------------------------------+----------------+--------+--------+
; |             EBX                |       BX       |   BH   |   BL   |  <- 64 bit RBX
; +--------------------------------+----------------+--------+--------+
; |             ECX                |       CX       |   CH   |   CL   |  <- 64 bit RCX
; +--------------------------------+----------------+--------+--------+
; |             EDX                |       DX       |   DH   |   DL   |  <- 64 bit RDX
; +--------------------------------+----------------+--------+--------+

; +--------------------------------+----------------+--------+--------+
; |             EDI                |       DI       |        |   DIL  |  <- 64 bit RDI
; +--------------------------------+----------------+--------+--------+
; |             ESI                |       SI       |        |   SIL  |  <- 64 bit RSI
; +--------------------------------+----------------+--------+--------+

; +--------------------------------+----------------+--------+--------+
; |             R(9-15)D           |     R(9-15)W   |        |R(9-15)B|  <- 64 bit R(9-15)
; +--------------------------------+----------------+--------+--------+

; C calling conventions:
;
; Function paramater register order:
;     rdi, rsi, rdx, rcx, r8, r9
; Return registers:
;     rax, rdx
; Scratch registers:
;     rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
; Preserved registers:
;     rbx, r12, r13, r14, r15, rsp, rbp

%define breakpoint int3

struc Arena
    .base_ptr:   resq 1
    .position:   resq 1
    .total_size: resq 1
endstruc

struc Number_String
    ; 31 bytes is enough to fit 64 bit values.
    ; 2^64 - 1 = "18446744073709551615" + '\0' = 21 bytes
    .buffer: resb 31
    .len: resb 1
endstruc

; Macros below require exporting:
;     print_newline, print_number
;
;%macro print_string2 2
;    mov rdi, %1
;    mov rsi, %2
;    call print_string
;%endmacro
;
;%macro PRINTLN 2
;    mov rdi, %1
;    mov rsi, %2
;    call print_string
;
;    call print_newline
;%endmacro
;
;%macro PRINTLN_WITH_NUMBER 3
;    mov rax, SYS_WRITE
;    mov rdi, STDOUT
;    mov rsi, %1
;    mov rdx, %2
;    syscall
;
;    mov rdi, %3
;    call print_number
;    call print_newline
;%endmacro
