#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    struct  stat statbuf;

    //if argv is bigeger than 2
    if (argc < 2){
        printf("usage: %s filename\n",argv[0]);
        return -1;
    }
    if (stat(argv[1],&statbuf)){
        printf("stat() fail\n");
        return -1;
    }

    printf("filename: %s\n",argv[1]);
    printf("size: %ld\n",statbuf.st_size);
    if(S_ISDIR(statbuf.st_mode)){
        printf("it is directoty\n");
    }else if(S_ISREG(statbuf.st_mode)){
        printf("it is regular file\n");
    }else {
        printf("???\n");
    }
    return 0;
}