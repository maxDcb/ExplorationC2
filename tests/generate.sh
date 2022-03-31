
gcc injectLinux.c -o injectLinux
gcc exec-asemblyLinux.c -o exec-asemblyLinux
gcc dumy.c -o dumy

nasm -f elf64 -o RevShellLinux.o RevShellLinux.nasm
ld -o RevShellLinux RevShellLinux.o
objcopy -O binary RevShellLinux RevShellLinux.bin
hexdump -v -e '"\\""x" 1/1 "%02x" ""' RevShellLinux.bin > RevShellLinux.hex
