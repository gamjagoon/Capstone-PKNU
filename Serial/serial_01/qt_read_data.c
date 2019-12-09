#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#define MAX_BUF 128
//sudo gcc qt_read_data.c -o qt_read_data -l wiringPi -DRaspberryP
unsigned int _time = 0;
unsigned int inteval = 200;
int serialdescriptor;
unsigned long baud = 115200;
char *device[] = {"/dev/ttyACM0","/dev/ttyACM1"};
char filepath[] = "Data_1";
//0left 1 2 3 keep 4 5 6 right
int check_signal[7] = {0};
const char *commend_buf[]={"$car:hnd:r:;","$car:hnd:keep:;","$car:hnd:l:;"};
int main()
{
	int fd;
	char buf[MAX_BUF];
	char prev_buf[MAX_BUF];
	int k = 0;
	while(1){
		if(millis()-_time>=inteval)
		{
			if( (serialdescriptor = serialOpen(device[k], baud)) < 0)
			{
				fprintf(stdout,"serailOpen() fail retry\n");
				_time = millis();
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
		exit(1);
	}

	while(1)
	{
		if(millis()-_time>=inteval)
		{
			fd = open("Data_1",O_RDONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fd == -1){
				printf("Data_1 is not found\n");
				_time = millis();
				continue;
			}
			else{
				break;
			}
		}
	}
	//read file end print file 
	int n;
	
	while(1)
	{
		int len = 0;
		off_t a = lseek(fd, 0, SEEK_SET);
		if(a == -1){
			printf("SEEK_SET fail\n");
			continue;
		}
		n = read(fd,&buf,sizeof(buf));
		if(n == -1){
			printf("read fail \n");
			continue;
		}
		for(size_t i = 0; i < n;++i)
		{
			if(buf[i] == ';')
			{
				buf[i++] = ';';
				buf[i] = '\0';
				break;
			}
		}
		char *hnd = strtok(buf,":");
		//500보다 값이 작아야 출력 가변저항은 우로 돌려야 작아진다
		if(strcmp(hnd,"$300")< 0 ){
			if( check_signal[6]==0)
			{
			memset(check_signal,0,sizeof(check_signal));
			printf("doright\n");
			check_signal[6] = 1;
			serialPuts (serialdescriptor, commend_buf[0]);
			}
		}
		else if(strcmp(hnd,"$700")< 0){
			if(check_signal[3] == 0)
			{
			memset(check_signal,0,sizeof(check_signal));
			printf("keepgo\n");
			check_signal[3] = 1;
			serialPuts (serialdescriptor, commend_buf[1]);
			}
		}
		else if(strcmp(hnd,"$999")<0){
			if(check_signal[0] == 0)
			{
			memset(check_signal,0,sizeof(check_signal));
			printf("doleft\n");
			check_signal[0] = 1;
			serialPuts (serialdescriptor, commend_buf[2]);
			}
		}
		memset(buf,0,sizeof(buf));
		delay(20);
	}
	close(fd);
	return 0;
}