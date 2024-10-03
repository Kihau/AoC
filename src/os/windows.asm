;  windows.asm  Operating System specific implementations.

%include "os/windows.inc"

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

    extern ExitProcess
    extern GetStdHandle
    extern WriteConsoleA
    extern VirtualAlloc
    extern VirtualFree
    extern CreateFileA
    extern CloseHandle
    extern GetFileSizeEx
    extern ReadFile


exit_program:
    sub rsp, 32
    mov rcx, rax ; [in] UINT uExitCode
    call ExitProcess
    ud2

; push r11
; push r10
; push r9
; push r8
; pop r8
; pop r9
; pop r10
; pop r11


print_output:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov r8, rax
    mov r9, rbx
    mov rcx, STD_OUTPUT_HANDLE ; [in] DWORD nStdHandle
    call GetStdHandle
    mov rcx, rax            ; [in]             HANDLE  hConsoleOutput,
    mov rdx, r8             ; [in]       const VOID    *lpBuffer,
    mov r8, r9              ; [in]             DWORD   nNumberOfCharsToWrite,
    mov r9, 0               ; [out, optional]  LPDWORD lpNumberOfCharsWritten,
    mov QWORD [rsp + 32], 0 ; [reserved]       LPVOID  lpReserved
    call WriteConsoleA
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


print_error:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov r8, rax
    mov r9, rbx
    mov rcx, STD_ERROR_HANDLE ; [in] DWORD nStdHandle
    call GetStdHandle
    mov rcx, rax            ; [in]             HANDLE  hConsoleOutput,
    mov rdx, r8             ; [in]       const VOID    *lpBuffer,
    mov r8, r9              ; [in]             DWORD   nNumberOfCharsToWrite,
    mov r9, 0               ; [out, optional]  LPDWORD lpNumberOfCharsWritten,
    mov QWORD [rsp + 32], 0 ; [reserved]       LPVOID  lpReserved
    call WriteConsoleA
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


map_memory_pages:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 32
    mov r9, PAGE_READWRITE           ; [in]           DWORD  flProtect
    mov r8, MEM_RESERVE | MEM_COMMIT ; [in]           DWORD  flAllocationType,
    mov rdx, rax                     ; [in]           SIZE_T dwSize,
    mov rcx, 0                       ; [in, optional] LPVOID lpAddress,
    call VirtualAlloc
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


unmap_memory_pages:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov r8, MEM_RELEASE ; [in] DWORD  dwFreeType
    mov rdx, rbx        ; [in] SIZE_T dwSize,
    mov rcx, rax        ; [in] LPVOID lpAddress,
    call VirtualFree
    mov rsp, rbp
    pop rbp
    ret


open_file:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 32 + 24 + 8
    mov QWORD [rsp + 32 + 16], 0                    ; [in, optional] HANDLE                hTemplateFile
    mov QWORD [rsp + 32 + 8], FILE_ATTRIBUTE_NORMAL ; [in]           DWORD                 dwFlagsAndAttributes,
    mov QWORD [rsp + 32], OPEN_EXISTING             ; [in]           DWORD                 dwCreationDisposition,
    mov r9, 0                                       ; [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    mov r8, FILE_SHARE_NONE                         ; [in]           DWORD                 dwShareMode,
    mov rdx, GENERIC_READ | GENERIC_WRITE           ; [in]           DWORD                 dwDesiredAccess,
    mov rcx, rax                                    ; [in]           LPCSTR                lpFileName,
    call CreateFileA
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


close_file:
    push r11
    push r10
    push r9
    push r8
    sub rsp, 32
    mov rcx, rax ; [in] HANDLE hObject
    call CloseHandle
    add rsp, 32
    pop r8
    pop r9
    pop r10
    pop r11
    ret


get_file_size:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 8 + 32
    lea rdx, [rsp + 32] ; [out] PLARGE_INTEGER lpFileSize
    mov rcx, rax        ; [in]  HANDLE         hFile,
    call GetFileSizeEx
    mov rdi, [rsp + 32]
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


read_file:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov QWORD [rsp + 32], 0 ; [in, out, optional] LPOVERLAPPED lpOverlapped
    mov r9, 0               ; [out, optional]     LPDWORD      lpNumberOfBytesRead,
    mov r8, rcx             ; [in]                DWORD        nNumberOfBytesToRead,
    mov rdx, rbx            ; [out]               LPVOID       lpBuffer,
    mov rcx, rax            ; [in]                HANDLE       hFile,
    call ReadFile
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret
