#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    int fd;
    int i = 0;
    char *file = NULL;
    char *linebuf;
    int *maped;
    int flag = PROT_WRITE | PROT_READ;

    if((fd = open("mymmap.mm",O_RDWR,0666)) < 0)
    {
        perror("FILE Open Error");
        exit(1);
    }
    if((maped = (int*)mmap(0,4096,flag, MAP_SHARED,fd,0)) == -1)
    {
        perror("mmap error");
        exit(1);
    }

    while(1)
    {
        if(maped[i] == -1)break;
        printf(">%d",maped[i]);
        i++;
    }
    close(fd);
}