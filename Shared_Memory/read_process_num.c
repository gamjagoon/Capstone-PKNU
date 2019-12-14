#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int shmid;
	int skey = 5678;

	int *shared_memory;

	shmid = shmget( (key_t)skey, sizeof(int), 0666);
	
}