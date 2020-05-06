#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    int fd = -1;
    int wd1 = -1;
    int wd2 = -1;
    int ret;
    char buf[1024];
    struct inotify_event *event;
    int epfd;

    fd = inotify_init();
    if(fd == -1){
        printf("inotify_init() fail()\n");
        return -1;
    }

    wd1 = inotify_add_watch(fd, ".", IN_CREATE | IN_DELETE);
    if(wd1 == -1){
        printf("add_watch() fail\n");
        goto err;
    }
    wd2 = inotify_add_watch(fd, ".", IN_CREATE | IN_DELETE);
    if(wd2 == -1){
        printf("add_watch() fail\n");
        goto err;
    }
    struct epoll_event epevent;
    epfd = epoll_create1(0);
    if (epfd == -1){
        printf("epoll_create() fail\n");
        goto err;
    }
    memset(&epevent, 0,sizeof(epevent));
    epevent.events = EPOLLIN;
    epevent.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epevent) == -1){
        printf("epoll_ctl() fail\n");
        goto err;
    }
    memset(&epevent, 0,sizeof(epevent));
    epevent.events = EPOLLIN;
    epevent.data.fd = STDIN_FILENO;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &epevent) == -1){
        printf("epoll_ctl() fail\n");
        goto err;
    }

    while(1){

        memset(&epevent, 0 , sizeof(epevent));
        ret = epoll_wait(epfd, &epevent, 1, 1000);

        if(ret <= -1){
            goto err;
        }else if(ret == 0){
            printf("epoll() timeout occur!!\n");
        }else
        {
            if(epevent.data.fd == fd){
                ret = read(fd,buf, sizeof(buf));
                if(ret == -1){
                    printf("read() fail\n");
                    break;
                }
                event = (struct inotify_event *)&buf[0];
                while(ret > 0){
                if(event->mask & IN_CREATE){
                    printf("file %s & IN_CREATE\n",event->name);
                }
                if(event->mask & IN_DELETE){
                    printf("file %s is deleted\n",event->name);
                }
                event = (struct inotify_event *)((char *)event + sizeof(struct inotify_event)+ event->len);
                ret -= (sizeof(struct inotify_event)+event->len);
                }
            }else if(epevent.data.fd == STDIN_FILENO){
                memset(buf,0,sizeof(buf));
                ret = read(STDIN_FILENO,buf, sizeof(buf));
                if(ret == -1){
                    printf("read() fail\n");
                    break;
                }
                printf("user input : [%s]\n",buf);
            }
        }
        
    }
    close(epfd);
    close(wd1);
    close(wd2);
    close(fd);
    return 0;
err:
    if(fd >= 0 ){
        close(fd);
    }
    if(wd1 >= 0 ){
        close(wd1);
    }
    if(wd2 >= 0 ){
        close(wd2);
    }
    if(epfd >= 0 ){
        close(epfd);
    }

    return -1;
}

























