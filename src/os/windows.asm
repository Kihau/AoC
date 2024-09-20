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


; Exit the program
; Input:
;     rdi - Exit error code
; Output:
;     None.
exit_program:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, rdi ; [in] UINT uExitCode
    call ExitProcess
    mov rsp, rbp
    pop rbp
    ud2


; Prints a string to standard output.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     None.
print_output:
    push rbp
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov rcx, STD_OUTPUT_HANDLE ; [in] DWORD nStdHandle
    call GetStdHandle
    mov rcx, rax            ; [in]             HANDLE  hConsoleOutput,
    mov rdx, rdi            ; [in]       const VOID    *lpBuffer,
    mov r8, rsi             ; [in]             DWORD   nNumberOfCharsToWrite,
    mov r9, 0               ; [out, optional]  LPDWORD lpNumberOfCharsWritten,
    mov QWORD [rsp + 32], 0 ; [reserved]       LPVOID  lpReserved
    call WriteConsoleA
    mov rsp, rbp
    pop rbp
    ret


; Prints a string to standard error.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     None.
print_error:
    push rbp
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov rcx, STD_ERROR_HANDLE ; [in] DWORD nStdHandle
    call GetStdHandle
    mov rcx, rax            ; [in]             HANDLE  hConsoleOutput,
    mov rdx, rdi            ; [in]       const VOID    *lpBuffer,
    mov r8, rsi             ; [in]             DWORD   nNumberOfCharsToWrite,
    mov r9, 0               ; [out, optional]  LPDWORD lpNumberOfCharsWritten,
    mov QWORD [rsp + 32], 0 ; [reserved]       LPVOID  lpReserved
    call WriteConsoleA
    mov rsp, rbp
    pop rbp
    ret


; Map virtual memory pages.
; Input:
;     rdi - Minimium number of bytes.
; Output:
;     rax - Pointer to the mapped pages.
map_memory_pages:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, 0                       ; [in, optional] LPVOID lpAddress,
    mov rdx, rdi                     ; [in]           SIZE_T dwSize,
    mov r8, MEM_RESERVE | MEM_COMMIT ; [in]           DWORD  flAllocationType,
    mov r9, PAGE_READWRITE           ; [in]           DWORD  flProtect
    call VirtualAlloc
    mov rsp, rbp
    pop rbp
    ret


; Unmap virtual memory pages.
; Input:
;     rdi - Pointer to mapped virtual memory.
;     rsi - Size of the mapped memory.
; Output:
;     None.
unmap_memory_pages:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, rdi        ; [in] LPVOID lpAddress,
    mov rdx, rsi        ; [in] SIZE_T dwSize,
    mov r8, MEM_RELEASE ; [in] DWORD  dwFreeType
    call VirtualFree
    mov rsp, rbp
    pop rbp
    ret


; Open file on a filesystem.
; Input:
;     rdi - Path to a file.
; Output:
;     rax - File handle. Returns -1 on error.
open_file:
    push rbp
    mov rbp, rsp
    sub rsp, 32 + 24 + 8
    mov rcx, rdi                                    ; [in]           LPCSTR                lpFileName,
    mov rdx, GENERIC_READ | GENERIC_WRITE           ; [in]           DWORD                 dwDesiredAccess,
    mov r8, FILE_SHARE_NONE                         ; [in]           DWORD                 dwShareMode,
    mov r9, 0                                       ; [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    mov QWORD [rsp + 32], OPEN_EXISTING             ; [in]           DWORD                 dwCreationDisposition,
    mov QWORD [rsp + 32 + 8], FILE_ATTRIBUTE_NORMAL ; [in]           DWORD                 dwFlagsAndAttributes,
    mov QWORD [rsp + 32 + 16], 0                    ; [in, optional] HANDLE                hTemplateFile
    call CreateFileA
    mov rsp, rbp
    pop rbp
    ret


; Close previously opened file.
; Input:
;     rdi - File handle.
; Output:
;     None.
close_file:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    mov rcx, rdi ; [in] HANDLE hObject
    call CloseHandle
    mov rsp, rbp
    pop rbp
    ret


; Get size of the file.
; Input:
;     rdi - File handle.
; Output:
;     rax - Size of the file
get_file_size:
    push rbp
    mov rbp, rsp
    sub rsp, 8 + 32
    mov rcx, rdi        ; [in]  HANDLE         hFile,
    lea rdx, [rsp + 32] ; [out] PLARGE_INTEGER lpFileSize
    call GetFileSizeEx
    mov rax, [rsp + 32]
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
    push rbp
    mov rbp, rsp
    sub rsp, 32 + 8 + 8
    mov rax, rdx
    mov rcx, rdi            ; [in]                HANDLE       hFile,
    mov rdx, rsi            ; [out]               LPVOID       lpBuffer,
    mov r8, rax             ; [in]                DWORD        nNumberOfBytesToRead,
    mov r9, 0               ; [out, optional]     LPDWORD      lpNumberOfBytesRead,
    mov QWORD [rsp + 32], 0 ; [in, out, optional] LPOVERLAPPED lpOverlapped
    call ReadFile
    mov rsp, rbp
    pop rbp
    ret
