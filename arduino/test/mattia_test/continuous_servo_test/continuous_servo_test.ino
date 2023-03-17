/* Continuous servo test.

*/

#include <Servo.h>

Servo myservo_1;
Servo myservo_2;

void setup() {
  myservo_1.attach(8); 
  myservo_2.attach(9);
}

void loop() {
  myservo_1.writeMicroseconds(1300); 
  delay(1000);
  myservo_1.writeMicroseconds(1500);
   delay(1000);
  myservo_1.writeMicroseconds(1700); 

  delay(1000);
  myservo_1.writeMicroseconds(1500);
  delay(1000);

  myservo_2.writeMicroseconds(1300); 
  delay(1000);
  myservo_2.writeMicroseconds(1500);
   delay(1000);
  myservo_2.writeMicroseconds(1700); 
  delay(1000);
  myservo_2.writeMicroseconds(1500);
  delay(1000);
}
