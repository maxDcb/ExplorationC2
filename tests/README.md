# Tests

## Linux:

### shellcode:
https://infosecwriteups.com/expdev-reverse-tcp-shell-227e94d1d6ee  
https://silviavali.github.io/blog/2019-01-25-blog-SLAE2/  
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

### exec-assemblyLinux:
https://github.com/Frodox/execute-machine-code-from-memory/blob/master/mem-test-exec.c  
Exec shellcode assembly in memory using mmap.

### injectLinux:
https://medium.com/@jain.sm/code-injection-in-running-process-using-ptrace-d3ea7191a4be#:~:text=Shell%20code%20is%20binary%20code,privileges%20to%20run%20ptrace%20though.  
Inject shellcode assembly in an process with this pid using ptrace. (try to inject in dumy)

## Windows:

### RevShellWindows:
https://github.com/TheWover/donut  
Use donut to creat shellcode from .exe.

### exec-assemblyWindows:
https://github.com/VirtualAlllocEx/Shellcode-Downloader-CreateThread-Execution  
Exec shellcode assembly in memory using VirtualAllocEx.   
Inject shellcode assembly in an process with this pid using CreateRemoteThread.


