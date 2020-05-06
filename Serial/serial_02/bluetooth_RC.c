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
//sudo rfcomm bind rfcomm0 98:D3:31:FC:7A:B4
char read_buf[MAX_BUF];
int BTsd,fd;
unsigned long baud = 9600;
unsigned long currentMillis;

unsigned long previousMillis_connect = 0;
unsigned long previousMillis_BTconnect = 0;
unsigned long previousMillis_read = 0;

const long connect_interval = 50;
const long read_interval = 30;
char device[]= "/dev/rfcomm0";
char filepath[] = "Data_1";
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
			if( (BTsd = serialOpen(device, baud)) < 0)
			{
				fprintf(stdout,"BTserailOpen() fail retry\n");
				previousMillis_connect = currentMillis;
				continue;
			}
			else 
			{
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
		currentMillis = millis();
		if(currentMillis-previousMillis_connect>=connect_interval)
		{
			if( (fd = open (filepath,O_RDONLY,0644))  < 0 )
			{
				fprintf(stdout,"open() fail retry \n");
				previousMillis_connect = currentMillis;
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
	char prev_a00 = 0,prev_0a0 = 0,prev_00a = 0,prev_2 = 0,prev_4 = 0,prev_sig = 0;
	while(1){
		currentMillis = millis();
		if(currentMillis-previousMillis_connect>=connect_interval)
		{
			off_t a = lseek(fd, 0, SEEK_SET);
			memset(read_buf,0,sizeof(read_buf));
			int n = read(fd,read_buf,12);
			if(read_buf[3] != prev_00a || prev_0a0 != read_buf[2]||prev_a00 != read_buf[1]|| read_buf[5] != prev_2 || read_buf[7] != prev_sig || read_buf[9] != prev_4)
			{
				serialPuts (BTsd, read_buf);
				printf("%s\n",read_buf);
				prev_a00 = read_buf[1];
				prev_0a0 = read_buf[2];
				prev_00a = read_buf[3];
				prev_2 = read_buf[5];
				prev_4 = read_buf[9];
				prev_sig = read_buf[7];
			}
			previousMillis_connect = currentMillis;
		}
		while(BTsd < 0)
		{
			if(currentMillis-previousMillis_BTconnect>=connect_interval && BTsd <= 0)
			{
				if( (BTsd = serialOpen(device, baud)) <= 0)
				{
					fprintf(stdout,"BTserailOpen() fail retry\n");
					previousMillis_BTconnect = millis();
				}
				else{
					break;
				}
			}
		}
	}
	close(fd);
	serialClose(BTsd);
	return 0;
}