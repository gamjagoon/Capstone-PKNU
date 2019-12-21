#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
int main(int argc, char *argv[]) {
    int fd;
    char* addr;
    struct stat statbuf;

    if(argc != 2){
        fprintf(stderr, "USage : %s filename \n",argv[0]);
        exit(1);
    }
    if( (fd = open(argv[1],O_RDWR)) == -1)
    {
        perror("File Open Error");
        exit(1);
    }

    if(fstat(fd, &statbuf) < 0)
    {
        perror("fstat error");
        exit(1);
    }

    addr = mmap(NULL,statbuf.st_size, PROT_READ |PROT_WRITE, MAP_SHARED, fd,(off_t)0);
    if(addr == MAP_FAILED){
        exit(1);
    }
    close(fd);

    printf("%s",addr);

    if(munmap(addr, statbuf.st_size) == -1) 
    { 
        exit(1); 
    }

    return 0;
}
