#include <sys/types.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
int main(int argc, char *argv[]) 
{ 
    int fd; pid_t pid; __caddr_t addr;
    struct stat statbuf; 
    if(argc != 2) 
    { fprintf(stderr, "Usage : %s filename\n", argv[0]); 
    exit(1); 
    } 
    if(stat(argv[1], &statbuf) == -1) 
    { //stat 함수로 파일의 속성 정보를 검색해 파일의 크기를 알아낸다. 
        perror("stat"); 
        exit(1); 
    } 
    if((fd = open(argv[1], O_RDWR)) == -1) 
    { // 메모리에 매핑할 데이터 파일을 연다. 
        perror("open"); 
        exit(1); 
    } // 파일을 메모리에 매핑한다. 
    addr = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)0);

    if(addr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    close(fd);

    switch (pid = fork())
    {
    case -1:
        perror("fork");
        exit(1);
        break;
    case 0:
        printf("1. Child Process : addr = %s\n",addr);
        sleep(1);
        addr[0] = 'x';
        printf("2. Chhile Process : addr=%s\n",addr);
        sleep(2);
        printf("3. Child Process : addr=%s\n",addr);
        break;
    default:
        printf("1. Parent Process : addr = %s\n",addr);
        sleep(2);
        printf("2. Parent Process : addr=%s\n",addr);
        addr[1] = 'y';
        printf("3. Parent Process : addr=%s\n",addr);
        break;
    }
    return 0;
}