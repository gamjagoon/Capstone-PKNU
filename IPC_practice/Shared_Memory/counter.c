#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#define KEY_NUM 1111
#define MEN_SIZE 1024

int main(void)
{
    int shm_di;
    void *shm_addr;
    int count;

    if(-1 == (shm_di = shmget( (key_t)KEY_NUM, MEN_SIZE, IPC_CREAT|0666))){
        perror("shmget");
        exit(1);
    }

    if( (void *)-1 == (shm_addr = shmat( shm_di, (void *)0, 0)))
    {
        perror("shmat");
        exit(1);
    }
    count = 0;
    while(1)
    {
        sprintf( (char *)shm_addr, "%d", count++);
        sleep(1);
    }
    return 0;
}