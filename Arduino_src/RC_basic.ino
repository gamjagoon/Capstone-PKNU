#include <SoftwareSerial.h>
#include <Servo.h>
#include <string.h>
//#include <NS_Rainbow.h>

//#define PIN 12
//#define N_CELL 8

int ch_to_int(char *ptr,int i);
SoftwareSerial BTSerial(2, 3); // RX, TX

volatile int now_dir = 90;
volatile int now_spd = 30;
volatile int now_sig = 0;
volatile int prev_sig = 0;

//7high 6low foward
class MJ_moter
{
    int  updateInterval;
    unsigned long lastUpdate = 0;
    volatile int pos ;
    volatile byte state_foward = HIGH;
    volatile byte state_back = LOW;
    int foward_pin;
    int now_pin;
    int back_pin;
    volatile int increment;            // increment to move for each interval
    volatile int prev_val = 0; 
    float angle = 1;
    const float pi = 3.141592;
  public:
    MJ_moter(int interval,int pin,int fpin,int bpin,unsigned int sig)
    {
      updateInterval = interval;
      pinMode(fpin,OUTPUT);
      pinMode(bpin,OUTPUT);
      pinMode(pin,OUTPUT);
      foward_pin = fpin;
      back_pin = bpin;
      now_pin = pin;
      if(sig == 0)
      {
        digitalWrite(foward_pin, state_foward);
        digitalWrite(back_pin, state_back);
      }
      else
      {
        state_foward = !state_foward;
        state_back = !state_back;
        digitalWrite(foward_pin, state_foward);
        digitalWrite(back_pin, state_back);
      }
      pos = 30;
      analogWrite(pin,pos);
    }
    void doit(volatile int val)
    {
      if((millis() - lastUpdate) > updateInterval)  // time to update
      {
      lastUpdate = millis();
      if(val == 1)
      {
        if(prev_val != val)angle = 0;
        angle++;
        if(angle > 70)angle = 70;
        increment = 1.0+ tan(angle*pi/180);
        pos += (int)increment;
        if(pos > 130) pos = 130;
        prev_val = val;
        Serial.println(pos,DEC);
      }
      else
      {
        if(prev_val != val)angle = 70;
        angle--;
        if(angle < 1)angle = 1;
        increment = 1.0+ tan(angle*pi/180);
        pos -= (int)increment;
        if(pos < 30) pos = 30;
        prev_val = val;
      }
      analogWrite(11,pos);
      return;
      }
    }
    void dostop()
    {
      if ((millis() - lastUpdate) > updateInterval)
      {
        analogWrite(now_pin,20);
      }
    }
    void fwd_back()
    {
        state_foward = !state_foward;
        state_back = !state_back;
        digitalWrite(foward_pin, state_foward);
        digitalWrite(back_pin , state_back);
        pos = 30;
    }
};
/*data struct*/
char buf[16];
bool moveable = true;

//NS_Rainbow ns_stick = NS_Rainbow(N_CELL,PIN);
MJ_moter moter(15,11,7,4,now_sig);
Servo myservo;
char ch;
int i = 0;
void setup()
{
  Serial.begin(9600);
  BTSerial.begin(9600);
  myservo.attach(8);
  
//  ns_stick.begin();
//  ns_stick.setBrightness(50);
//  ns_stick.clear();
}

volatile int pos = 90;
unsigned long lastUpdate = 0 ;
unsigned long lastUpdate_read = 0 ;
unsigned long  updateInterval = 15;
unsigned long  read_Interval = 20;
void loop()
{
  if(moveable)
  {
      if ((millis() - lastUpdate) > updateInterval)
      {
        if(pos == now_dir);
        else if(pos < now_dir)
        {
          pos+=3;
          if(pos >130)pos = 130;
        }
        else
        {
          pos-=3;
          if(pos < 30)pos = 30;
        }
        myservo.write(pos);
        delay(30);
        lastUpdate = millis();
      }
      moter.doit(now_spd);
  }
  else
  {
    moter.dostop();
  }
  if ((millis() - lastUpdate_read) > read_Interval && BTSerial.available())
  {
    ch = BTSerial.read();
    if(ch=='$')
    { 
      byte len = BTSerial.readBytes(buf,10);
      if(len == 10)
      {
        now_dir = ch_to_int();
        now_spd = buf[4]-'0';
        now_sig = buf[6]-'0';
        if(buf[8] == 'm')
        {
          moveable = true;
        }
        else
        {
          moveable = false;
        }
      }
    BTSerial.flush();
    }
  }
}
//void rainbow(uint16_t interval) {
//  uint16_t n = ns_stick.numCells();
//
//  for(uint16_t j=0; j<255; j++) {  // one cycle 
//    for(uint16_t i=0; i<n; i++) {
//      byte r_pos = (((i<<8) -  i) / n + j) % 0xFF;
//      byte sect = (r_pos / 0x55) % 0x03, pos = (r_pos % 0x55) * 0x03;
//  
//      switch(sect) {
//      case 0: 
//        ns_stick.setColor(i,ns_stick.RGBtoColor(0xFF - pos, pos, 0x00)); break;
//      case 1: 
//        ns_stick.setColor(i,ns_stick.RGBtoColor(0x00, 0xFF - pos, pos)); break;
//      case 2:
//        ns_stick.setColor(i,ns_stick.RGBtoColor(pos, 0x00, 0xFF - pos)); break;
//      }  
//    }
//    ns_stick.show();
//    delay(interval);
//  }
//} 

volatile int ch_to_int()
{
  volatile int sum = 0;
  if(buf[0] == '1')sum+=100;
  sum += (buf[1]-'0')*10;
  sum += buf[2]-'0';
  return sum;
}
