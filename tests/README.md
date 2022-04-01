# Tests

## Linux:

###shellcode:
RevShellLinux.c -> RevShellLinux.nasm  

Get syscall code  
-> find syscall in /usr/include/asm/unistd_64.h  
-> or use ausyscall execve  

Find code for params  
-> python -c 'import socket; print socket.SOCK_STREAM'  
-> python -c 'import socket; print socket.AF_INET'  

Compile assembly   
-> nasm -f elf64 -o RevShellLinux.o RevShellLinux.nasm  
-> ld -o RevShellLinux RevShellLinux.o  

Test it with strace  
-> strace ./RevShellLinux  

// objdump -d -M intel RevShellLinux  

Get the shellcode  
-> objcopy -O binary RevShellLinux RevShellLinux.bin  
-> hexdump -v -e '"\\""x" 1/1 "%02x" ""' RevShellLinux.bin  

###exec-asemblyLinux:
Exec shellcode assembly in memory using mmap.

###injectLinux:
Inject shellcode assembly in an process with this pid using ptrace. (try to inject in dumy)

## Windows:

###RevShellWindows:
Use donut to creat shellcode from .exe.

###exec-asemblyWindows:
Exec shellcode assembly in memory using VirtualAllocEx.
Inject shellcode assembly in an process with this pid using CreateRemoteThread.


