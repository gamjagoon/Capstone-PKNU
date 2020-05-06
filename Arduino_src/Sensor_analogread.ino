#include <string.h>
#include <stdlib.h>
#define MQ3_Threshold 450
//data struct
char buf[32];
char *command, *argu1, *argu2;

//time
unsigned long previousMillis_send = 0;
unsigned long previousMillis_MQ3 = 0;
//loadcell and hadle
unsigned long previousMillis_handle = 0;

const long send_interval = 30;
const long handle_interval = 25;
const long MQ3_interval = 1000;

//analog read
int A0_read = 0;
int A1_read = 0;
/*속도 0 0 1 10 2 20 ...*/

volatile int now_spd;
volatile int prev_spd;
int now_sig = 0;
int prev_sig = 0;
int now_dir = 0;
int prev_dir = 0;
//음주 유무 및 운행 가능
bool sw = false;
bool Moveable = false;
bool diff = false;
unsigned long currentMillis;

void setup()
{
  Serial.begin(9600);
  pinMode(4,INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(12, INPUT);
  digitalWrite(8, LOW);
  digitalWrite(7, LOW);
}
void loop()
{
  while (sw && Moveable)
  {
    //명령처리부
    currentMillis = millis();
    if (Serial.available() > 0)
    {
      memset(buf, 0, sizeof(buf));
      byte len = Serial.readBytes(buf, 32);
      if (len <= 0)continue;
      else
      {
        command = strtok(buf, ";");
        if (strcmp(command, "$stop") == 0) {
          sw = false;
          digitalWrite(8, LOW);
        }
        else {
          continue;
        }
      }
    }
    //sensing
    if (currentMillis - previousMillis_handle >= handle_interval) 
    {
      prev_spd = now_spd;
      prev_dir = now_dir;
      prev_sig = now_sig;
      A0_read = analogRead(A0);
      now_spd = digitalRead(12);
      now_dir = (int)(A0_read/5.68);
      if(now_dir > 130)now_dir = 130;
      else if(now_dir < 30)now_dir = 30;
      now_sig = digitalRead(4);
      if(now_spd == 1)diff =true;
      else if (prev_dir != now_dir || prev_spd != now_spd || prev_sig != now_sig)diff = true;
      previousMillis_handle = currentMillis;
    }

    //sw on moveable
    if (currentMillis - previousMillis_send >= send_interval) 
    {
      if (diff)
      {
        String msg = "$" ;
        if(now_dir < 100){
          msg+="0";
          msg=msg+now_dir+':' + now_spd + ':' + now_sig + ':';
        }
        else{
          msg=msg+now_dir+':' + now_spd + ':' + now_sig + ':';
        }
        if (Moveable) {
          msg += "m;";
          Serial.println(msg);
        }
        else {
          msg += "e;";
          Serial.println(msg);
          break;
        }
        diff = false;
        
      }
      previousMillis_send = currentMillis;
    }
    //check MQ3
    if (currentMillis - previousMillis_MQ3 >= MQ3_interval)
    {
      if(now_sig == 1)digitalWrite(4,HIGH);
      A1_read = analogRead(A1);
      if (A1_read > MQ3_Threshold) {
        Moveable = false;
        diff = true;
        break;
      }
      previousMillis_MQ3 = currentMillis;
    }
  }
  while (sw)
  {
    currentMillis = millis();
    if (currentMillis - previousMillis_MQ3 >= MQ3_interval)
    {
      if (A1_read > MQ3_Threshold) {
        Moveable = false;
        diff = true;
      }
      else {
        Moveable = true;
        diff = true;
        break;
      }
      previousMillis_MQ3 = currentMillis;
    }
    //send msg
    if (currentMillis - previousMillis_send >= send_interval) 
    {
      if (diff)
      {
        String msg = "$" ;
        if(now_dir < 100){
          msg+="0";
          msg=msg+now_dir+':' + now_spd + ':' + now_sig + ':';
        }
        else{
          msg=msg+now_dir+':' + now_spd + ':' + now_sig + ':';
        }
        if (Moveable) {
          msg += "m;";
          Serial.println(msg);
          break;
        }
        else {
          msg += "e;";
          Serial.println(msg);
        }
        diff = false;
      }
      previousMillis_send = currentMillis;
    }
  }
  if (Serial.available() > 0 && !sw)
  {
    memset(buf, 0, sizeof(buf));
    byte len = Serial.readBytes(buf, 32);
    if (len > 0 && buf[0] == '$')
    {
      command = strtok(buf, ";");
      if ( strcmp(command, "$on") == 0)
      {
        if (analogRead(A1) < MQ3_Threshold) 
        {
          sw = true;
          Moveable = true;
          digitalWrite(8, HIGH);
        }
        else 
        {
          String msg = "$3:0:1:e;";
          Serial.println(msg);
        }
      }
    }
  }
}
