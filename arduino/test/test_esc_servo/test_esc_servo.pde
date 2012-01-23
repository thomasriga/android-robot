#include <Servo.h> 
 
Servo myservo;
Servo myservo2;
int armed = 0;

void setup() {
  myservo.attach(A0);
  myservo2.attach(A1);
  pinMode(13, OUTPUT);
}

void loop() 
{
  if(armed == 0) {
    digitalWrite(13, HIGH);
    myservo2.write(31);
    delay(200);
    myservo2.write(90);
    delay(200);
    armed = 1;
    digitalWrite(13, LOW);
  }


  myservo.write(40);
  delay(1000);
  myservo.write(90);
  delay(1000);
  myservo.write(140);
  delay(1000);
  myservo2.write(80);
  delay(2000);
  myservo2.write(100);
  delay(2000);
  myservo2.write(90);
}

