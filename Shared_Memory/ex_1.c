#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	int shmid;
	void *shared_memory = (void *)0;
	FILE *fp;
	char buf[1024];
	int skey = 5678;

	int *process_num;
	int local_num;

	shmid = shmget( (key_t)skey, sizeof(int), 066|IPC_CREAT);
	if(shmid == -1){
		perror("shmget failed : ");
		exit(0);
	}

	printf("key %x\n", skey);
	if(!shared_memory)
	{
		perror("shmat failed : ");
		exit(0);
	}

	process_num = (int *)shared_memory;

	for(;;)
	{
		local_num = 0;
		fp = popen("ps axh","r");
		if(fp != NULL)
		{
			while(fgets(buff,1024,fp)){
				local_num++;
			}
		}
		*process_num = local_num;
		printf("process_num is %d\n",(int)*process_num);
		sleep(5);
		pclose(fp);
	}
} 