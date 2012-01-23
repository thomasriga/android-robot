/* Ping))) Sensor
  
   This sketch reads a PING))) ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse 
   to return.  The length of the returning pulse is proportional to 
   the distance of the object from the sensor.
     
   The circuit:
	* +V connection of the PING))) attached to +5V
	* GND connection of the PING))) attached to ground
	* SIG connection of the PING))) attached to digital pin 7

   http://www.arduino.cc/en/Tutorial/Ping
   
   created 3 Nov 2008
   by David A. Mellis
   modified 30 Jun 2009
   by Tom Igoe
 
   This example code is in the public domain.

 */

// this constant won't change.  It's the pin number
// of the sensor's output:
// arduino UNO: 9,10,11 good candidates for ping pin
const int pingPin = 11;
const int ledPin1 = 12;              // blinking led
const int ledPin2 = 13;              // blinking led

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void ledactive(int sec ,int led) {
  digitalWrite(led, HIGH);
  delay(sec * 1000);
  digitalWrite(led, LOW);
}  

void mbleep(int num ,int led) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
  }
}  

void loop()
{
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, inches, cm;
  ledactive(1, ledPin1);
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  mbleep(cm / 10, ledPin2);
  delay(100);
}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
