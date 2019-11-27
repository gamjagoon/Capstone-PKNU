#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT 5 //1000 millies()
#define BUF_LEN 1024

int main(void)
{
    static struct timeval tv;
    static fd_set readfds;
    static int ret;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO,&readfds);
    //init timval struct 
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    //descripter , 
    ret = select(STDIN_FILENO + 1,&readfds, NULL,NULL,&tv);
    if(FD_ISSET(STDIN_FILENO, &readfds)){
        char buf[BUF_LEN+1];
        int len;

        len = read(STDIN_FILENO,buf,BUF_LEN);
        if(len == -1){
            perror ("read");
            return 1;
        }

        if(len){
            buf[len] = '\0';
            printf("read : %s\n",buf);
        }
        return 0;
    }
    fprintf(stderr, "This should not happen!\n");
    return 1;
}