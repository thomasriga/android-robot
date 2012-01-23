#include <CompactQik2s9v1.h>
#include <NewSoftSerial.h>
#include <Servo.h> 

// arduino BT
//#define rxPin 2
//#define txPin 3
#define rxPin 0
#define txPin 1
#define rstPin 4

NewSoftSerial mySerial =  NewSoftSerial(rxPin, txPin);
CompactQik2s9v1 motor = CompactQik2s9v1(&mySerial,rstPin);
byte fwVersion = -1;

void setup() {
  mySerial.begin(9600);
  motor.begin();
}

void loop() 
{
  motor.motor0Forward(127);
  delay(1000);
  motor.motor0Forward(0);
  delay(1000);
  motor.motor0Reverse(127);
  delay(1000);
  motor.motor0Forward(0);
  delay(1000);
}
