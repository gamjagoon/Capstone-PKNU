#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY_NUM 1111
#define MEN_SIZE 1024

int main(void)
{
    int shm_id;
    void *shm_addr;

    if(-1 == ( shm_id = shmget( (key_t)KEY_NUM, MEN_SIZE, IPC_CREAT)))
    {
        perror("shmget");
        return -1;
    }
    if( (void *)-1 == (shm_addr = shmat( shm_id, (void *) 0, 0)))
    {
        perror("shmat");
        return -1;
    }

    while(1)
    {
        printf("%s\n",(char*)shm_addr);
        sleep(1);
    }
    return 0;
}