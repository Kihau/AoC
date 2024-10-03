; Exit the program
; Parameters:
;     [in] u64 - Exit error code
; Returns:
;     None.
export exit_program

; Prints a string to standard output.
; Parameters:
;     [in] u8* - Input string.
;     [in] u64 - Size of the string.
; Returns:
;     None.
export print_output

; Prints a string to standard error.
; Parameters:
;     [in] u8* - Input string.
;     [in] u64 - Size of the string.
; Returns:
;     None.
export print_error

; Map virtual memory pages.
; Parameters:
;     [in] u64 - Minimium number of bytes. The byte count will be aligned to the page size.
; Returns:
;     [out] u8* - Pointer to the mapped pages.
export map_memory_pages

; Unmap virtual memory pages.
; Parameters:
;     [in] u8* - Pointer to mapped virtual memory.
;     [in] u64 - Size of the mapped memory.
; Returns:
;     None.
export unmap_memory_pages

; Open file on a filesystem.
; Parameters:
;     [in] u8* - Path to a file.
; Returns:
;     [out] u64 - File handle.
export open_file

; Close previously opened file.
; Parameters:
;     [in] u64 - File handle.
; Returns:
;     None.
export close_file

; Get size of the file.
; Parameters:
;     [in] u64 - File handle.
; Returns:
;     [out] u64 - Size of the file
export get_file_size

; Read data from a file.
; Parameters:
;     [in]  u64 - File handle.
;     [out] u8* - Pointer to a buffer to write to.
;     [in]  u64 - Number of bytes to read.
; Returns:
;     TODO: [out] u64 - Number of bytes that was read.
export read_file
