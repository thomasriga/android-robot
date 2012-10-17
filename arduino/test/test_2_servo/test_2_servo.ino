#include <Servo.h> 
 
Servo myservo1;
Servo myservo2;

void setup() {
  myservo1.attach(A0);
  myservo2.attach(A1);
}

void loop() 
{
  myservo1.write(0);
  delay(1000);
  myservo1.write(90);
  delay(1000);
  myservo1.write(180);
  delay(1000);
  myservo2.write(0);
  delay(1000);
  myservo2.write(60);
  delay(1000);
  myservo2.write(150);
  delay(1000);
}

