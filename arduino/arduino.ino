#include <avr/io.h>

#define E (1<<7)
#define RW  (1<<6)
#define RS  (1<<5)

void clock()
{
 PORTD|=E;
 delay(1);
 PORTD&=~E;
}

void commandClock()  //slow down for sending commands
{
 PORTD|=E;
 delay(5);
 PORTD&=~E;
}

void command(uint8_t cmd)
{
  PORTD&=~(RW|RS);
  PORTB =((cmd>>4)&0xf);
  commandClock();
  PORTB=(cmd&0xf);
  commandClock();
}

void lcdWrite(uint8_t cmd)
{
  PORTD&=~RW;
  PORTD|=RS;
  PORTB =((cmd>>4)&0xf);
  clock();
  PORTB=(cmd&0xf);
  clock();
}

void lcdCursor(uint8_t cmd)
{
 command(0x80|(cmd&0x7f)); 
}

void flipBuffer(uint8_t *buffer)
{
  //command(0x01);  //clear display
  lcdCursor(0);
  for(uint8_t y=0;y<=1;y++)
  {
    for(uint8_t x=0;x<=8;x++)
      lcdWrite(*(buffer+x+8*y));
    lcdCursor(40);  //second row, first character
  }
}

void setup()
{   
   //parallel pins as output
   DDRB=0b1111;
   //RS, r/w, E as otuput
   DDRD=E|RW|RS;
   PORTD&=~(E|RW|RS);
   //buttons as input
   DDRD&=~((1<<3)|(1<<2));
   PORTD=(1<<3)|(1<<2);
   
   //initialize three times
   PORTB=0x3;
   clock();
   delay(10);
   clock();
   delay(10);
   clock();
   delay(10);
   clock();
   delay(10);
   PORTB=0x2;
   clock();
   
   //setup
   command(0x01);  //clear display
   command(0x28);
   command(0x10);
   command(0x0c);
   command(0x06);
   
   Serial.begin(9600);
}

void loop()
{
  static uint8_t buttons=0xff;
  uint8_t nbuttons = (~PIND&0b1100)>>2;
  if(nbuttons != buttons)
  {
    buttons=nbuttons;
    Serial.println(buttons);
  }
}

void serialEvent()
{ 
  static uint8_t sbuffer[16];
  static uint8_t scursor=0;
  uint8_t data;
  data=Serial.read();
  if(data == 0x0A)
  {
    flipBuffer(&sbuffer[0]);  //draw on lcd
    scursor=0;
  }
  else
  {
    sbuffer[scursor]=data;
    if(scursor<17)
      scursor++;
  }  
}
