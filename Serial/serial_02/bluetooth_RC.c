#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <errno.h>
#include <pthread.h>
#define MAX_BUF 32

//sudo gcc bluetooth_RC.c -o bluetooth_RC -l wiringPi
char read_buf[MAX_BUF];
int BTsd,fd;
unsigned long baud = 9600;
unsigned long cur_time=0;
unsigned long prev_time_1=0;
unsigned long prev_time_2=0;
unsigned long inteval = 20;
char device[]= "/dev/rfcomm1";
char filepath[] = "Data_1";
int main()
{
	//check for this process
	printf("%s \n", "serial begin\n");
	fflush(stdout);
	//!open serial
	while(1){
		cur_time = millis();
		if(cur_time-prev_time_1>=inteval)
		{
			if( (BTsd = serialOpen(device, baud)) < 0)
			{
				fprintf(stdout,"BTserailOpen() fail retry\n");
				prev_time_1 = cur_time;
				continue;
			}
			else {
				fprintf(stdout,"BTserailOpen() Success\n");
				break;
			}
		}
	}
	if (wiringPiSetup () == -1){
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		exit(1);
	}
	serialFlush (BTsd);
	//!open file
	while(1){
		cur_time = millis();
		if(cur_time-prev_time_1>=inteval)
		{
			if( (fd = open (filepath,O_RDONLY,0644))  < 0 )
			{
				fprintf(stdout,"open() fail retry \n");
				prev_time_1 = cur_time;
				continue;
			}
			else{
				fprintf(stdout,"open() Sucess\n");
				break;
			}
		}
	}
	
	//**read serial end write at Data_1;
	int i = 0;
	char ch;
	while(1){
		cur_time = millis();
		if(cur_time-prev_time_1>=inteval)
		{
			off_t a = lseek(fd, 0, SEEK_SET);
			memset(read_buf,0,sizeof(read_buf));
			int n = read(fd,read_buf,32);
			serialPuts (BTsd, read_buf);
			printf("%s\n",read_buf);
			prev_time_1 = cur_time;
			serialFlush (BTsd);
		}
		if(cur_time-prev_time_2>=inteval && BTsd <= 0)
		{
			if( (BTsd = serialOpen(device, baud)) <= 0)
			{
				fprintf(stdout,"BTserailOpen() fail retry\n");
				prev_time_2 = cur_time;
			}
		}
	}
	close(fd);
	serialClose(BTsd);
	return 0;
}