#include <sys/types.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <sys/stat.h> 

int main(void) {
    int fd,pagesize,lengh;
    __caddr_t addr;

    pagesize = sysconf(_SC_PAGESIZE);
    lengh = 1*pagesize;

    if((fd = open("m.dat",O_RDWR|O_CREAT | O_TRUNC, 0666)) == -1){
        perror("open");
        exit(1);
    }

    if(ftruncate(fd, (off_t)lengh) == -1){
        perror("ftrunate");
        exit(1);
    }

    addr = mmap(NULL, lengh, PROT_READ | PROT_WRITE,MAP_SHARED,fd,(off_t)0);
    if(addr == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    close(fd);

    strcpy(addr, "Ftrucate Test\n");

    return 0;
}
