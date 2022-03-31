#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


int main()
{
	while(1)
	{
		system("/bin/whoami");
		sleep(1);
	}

	return 0;
}
