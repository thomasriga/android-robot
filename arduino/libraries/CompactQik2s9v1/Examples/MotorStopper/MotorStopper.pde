/*************************************************************
 * CompactQik2s9v1 - basic class to control Pololu's Qik2s9v1
 * motor controller (http://www.pololu.com/catalog/product/1110)
 * 
 * This uses the default settings for the motor controller and the 
 * Compact Protocol to communicate to it.
 * 
 * This library also depends on the NewSoftSerial library which
 * can be found at: http://arduiniana.org.
 * 
 * This sketch shows the required setup and then tells the
 * motors to stop
 * 
 *************************************************************/

#include <CompactQik2s9v1.h>
#include <NewSoftSerial.h>

#define rxPin 3
#define txPin 4
#define rstPin 5

NewSoftSerial mySerial =  NewSoftSerial(rxPin, txPin);
CompactQik2s9v1 motor = CompactQik2s9v1(&mySerial,rstPin);
byte fwVersion = -1;

void setup()  
{
  mySerial.begin(9600);
  motor.begin();
  motor.stopBothMotors();
}



void loop() 
{
  // nothing to do, used to stop the motors
}
