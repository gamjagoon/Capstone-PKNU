#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
void handle_error(int a,const char* str)
{
    if(a == -1){
        fprintf(stderr,"%s",str);
        exit(0);
    }
}

int main()
{
    int shmid;
    int pid;

    int *cal_num;
    void *shared_memory = (void *)0;
    //**make shar_memory devition
    shmid = shmget( (key_t)1234, sizeof(int), 0666|IPC_CREAT);

    handle_error(shmid,"shmget failed : ");

    cal_num =(int *)shared_memory;
    pid = fork();
    if (pid == 0)
    {
        shmid = shmget((key_t)1234, sizeof(int), 0);
        handle_error(shmid,"shmget failed : ");
        shared_memory = shmat(shmid, (void *)0, 0666|IPC_CREAT);
        if(shared_memory == (void *) -1)
        {
            perror("shmat failed : ");
            exit(0);
        }
        cal_num = (int *)shared_memory;
        *cal_num = 1;
        while(1)
        {
            *cal_num = *cal_num + 1;
            printf("child %d\n",*cal_num);
            sleep(1);
        }
    }
    else if(pid > 0)
    {
        while(1)
        {
            sleep(1);
            printf("%d\n",*cal_num);
        }
     }
}