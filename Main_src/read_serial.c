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
#define MAX_BUF 100

//sudo gcc read_serial.c -o read_serial -l wiringPi -lpthread
char read_buf[MAX_BUF];
char commend_buf[MAX_BUF];
int sd, fd;
unsigned long baud = 9600;
unsigned long currentMillis;

unsigned long previousMillis_connect = 0;
unsigned long previousMillis_read = 0;

const long connect_interval = 50;
const long read_interval = 30;
char *device[] = {"/dev/ttyACM0","/dev/ttyACM1"};
pthread_t p_thread;
int j = 0;

void *commend_thread(void *p)
{
	char ch; 
	while( (ch = fgetc(stdin)))
	{
		if(ch == '\n')continue;
		if(ch != ';'){
			commend_buf[j++] = ch;
		}
		else{
			commend_buf[j++] = ';';
			commend_buf[j] = '\0';
			printf("%s\n",commend_buf);
			serialPuts (sd, commend_buf);
			if(strcmp(commend_buf,"$endprogram;")==0){
				printf("end\n");
			}
			memset(commend_buf,0,sizeof(commend_buf));
			j = 0;
		}
	}
}
int k = 0;
int main()
{
	//check for this process

	printf("%s \n", "serial begin\n");
	fflush(stdout);
	//!open serial
	while(1){
		currentMillis = millis();
		if(currentMillis-previousMillis_connect>=connect_interval)
		{
			if( (sd = serialOpen(device[k], baud)) < 0)
			{
				fprintf(stdout,"serailOpen() fail retry\n");
				previousMillis_connect = millis();
				k++;
				continue;
			}
			else {
				fprintf(stdout,"serailOpen() Success\n");
				break;
			}
		}
	}
	if (wiringPiSetup () == -1){
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		exit(1);
	}
	serialFlush (sd);
	//!open file
	while(1){
		currentMillis = millis();
		if(currentMillis-previousMillis_connect>=connect_interval)
		{
			if( (fd = open ("Data_1", O_WRONLY|O_TRUNC|O_CREAT,0644))  < 0 )
			{
				fprintf(stdout,"open() fail retry \n");
				previousMillis_connect = millis();
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
	int thr_id = pthread_create(&p_thread, NULL, commend_thread, (void *)NULL);
	if( thr_id < 0){
		fprintf (stdout, "Unable to start thread: %s\n", strerror (errno)) ;
		exit(1);
	}
	fflush(stdin);


	while(1){
		currentMillis = millis();
		if(currentMillis-previousMillis_read>=read_interval)
		{
			if(serialDataAvail (sd))
			{
				ch = serialGetchar(sd);
				if(ch == '$')
				{
					i = 0;
					read_buf[i++] = ch;
					while( (ch = serialGetchar(sd) ) != ';')
					{
						read_buf[i++] = ch;
					}
					off_t a = lseek(fd, 0, SEEK_SET);
					read_buf[i++] = ch;
					read_buf[i] = '\0';
					int n = write(fd,read_buf,i);
					memset(read_buf,0,sizeof(read_buf));
					serialFlush (sd);
				}
			}
			previousMillis_read= millis();
		}
	}
	close(fd);
	serialClose(sd);
	return 0;
}