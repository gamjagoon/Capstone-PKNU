#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main()
{
    int fd;
    int *pmmap;
    int i;
    fd = open("mymmap.mm", O_RDWR|O_CREAT, 0666);
    if(fd < 0)
    {
        perror("open");
        exit(1);
    }

    ftruncate(fd,4096);//! make file size

    pmmap = (int *)mmap(0,4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( (unsigned)pmmap == (unsigned)-1)
    {
        perror("mmap");
        exit(1);
    }

    for(i = 0; i < 100;++i){
        pmmap[i] = i;
    }
    pmmap[i+1] = -1;
    getchar();
    munmap(pmmap, 4096);
    close(fd);
    return 0;
}