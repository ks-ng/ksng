section .bss
    buffer resb 128  ; Reserve 128 bytes for the input buffer

section .data
    prompt db "Enter something: ", 0
    prompt_len equ $ - prompt

section .text
    global _start

_start:
    ; Print the prompt
    mov eax, 4           ; sys_write
    mov ebx, 1           ; file descriptor 1 (stdout)
    mov ecx, prompt      ; pointer to the prompt string
    mov edx, prompt_len  ; length of the prompt string
    int 0x80

    ; Read user input
    mov eax, 3           ; sys_read
    mov ebx, 0           ; file descriptor 0 (stdin)
    mov ecx, buffer      ; pointer to the buffer
    mov edx, 128         ; number of bytes to read
    int 0x80

    ; Store the number of bytes read in a register
    mov esi, eax         ; esi will now hold the number of bytes read

    ; Print the user input
    mov eax, 4           ; sys_write
    mov ebx, 1           ; file descriptor 1 (stdout)
    mov ecx, buffer      ; pointer to the buffer
    mov edx, esi         ; number of bytes to write (same as the number of bytes read)
    int 0x80

    ; Exit the program
    mov eax, 1           ; sys_exit
    xor ebx, ebx         ; exit code 0
    int 0x80
