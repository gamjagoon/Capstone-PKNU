#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define TARGET_FILE "hello_world"
int main(int argc, char **argv)
{
    if(link(TARGET_FILE, "hello_by_link")){
        printf("link() fail\n");
        return -1;
    }
    if(symlink(TARGET_FILE, "hello_by_symlink")){
        printf("symlink() fail\n");
        return -1;
    }
    return 0;
}