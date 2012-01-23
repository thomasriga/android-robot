#include <CompactQik2s9v1.h>
#include <NewSoftSerial.h>
#include <Servo.h> 
 
#define rxPin 0
#define txPin 1
#define rstPin 2
#define echoPin 5             
#define initPin 6           
#define ledPin 13
#define reverse_right 0
#define reverse_left 1 
#define FORWARD 1
#define RIGHT 2
#define LEFT 3
#define debug 0

int mindist = 25;

NewSoftSerial mySerial =  NewSoftSerial(rxPin, txPin);
CompactQik2s9v1 motor = CompactQik2s9v1(&mySerial,rstPin);
byte fwVersion = -1;
Servo myservo;
int pos = 0;    
int rspeed = 0;
int lspeed = 0;

void setup() {
  pinMode(initPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mySerial.begin(9600);
  motor.begin();
  myservo.attach(A0);
}

void loop() 
{
  step2();
}

void mbleep(int num, int interval) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(ledPin, HIGH);
    delay(interval);
    digitalWrite(ledPin, LOW);
    delay(interval);
  }
}

unsigned long distance() {
  unsigned long pulseTime;
  digitalWrite(initPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(initPin, LOW);
  pulseTime = pulseIn(echoPin, HIGH);
  return pulseTime / 58;
}

void stop()
{
 int i;
 int smax;
 if(lspeed > rspeed)
 {
   smax = lspeed;
 }
 else
 {
   smax = rspeed;
 }
 for(i = smax; i >= 0; i--) {
   if(rspeed > 1) {
     motor.motor0Forward(rspeed);
   }
   else
   {
     if(rspeed == 1) {
       motor.stopMotor0();
     }
   }
   rspeed--;
   if(lspeed > 1) {
     motor.motor1Forward(lspeed);
   }
   else
   {
     if(lspeed == 1) {
       motor.stopMotor1();
     }
   }
   lspeed--;
   delay(5);
 }
}

void step()
{
  unsigned long l, r, f;
  int moving = 0;
  stop();
  myservo.write(15);
  delay(1000);
  l = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(l / 10, 200);
  }
  myservo.write(165);
  delay(1000);
  r = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(r / 10, 200);
  }
  myservo.write(90);
  f = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(f / 10, 200);
  }
  if(r < l) {
    if(r < mindist) {
      //mbleep(3, 200);
      move2(LEFT, 3000); 
    }
  }
  else if(l < mindist) {
    //mbleep(6, 200);
    move2(RIGHT, 3000);
  }
  stop();
  //mbleep(1, 2000);
  move2(FORWARD, 6000);
}

void move(int dir, int duration)
{
  int x;
  int dly = duration/ 12;
  for(x = 10; x < 127; x += 10) {
    if(dir == LEFT) {
      lspeed = x / 2;
    }
    else {
      lspeed =x;
    }
    if(dir == RIGHT) {
      rspeed = x / 2;
    }
    else {
      rspeed = x;
    }
    motor.motor0Forward(rspeed);
    motor.motor1Forward(lspeed);
    delay(dly);
  }
}

void step2()
{
  unsigned long l, r, f;
  int moving = 0;
  //stop();
  myservo.write(150);
  delay(1000);
  l = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(l / 10, 200);
  }
  myservo.write(30);
  delay(1000);
  r = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(r / 10, 200);
  }
  myservo.write(90);
  f = distance();
  if(debug == 1) {
    mbleep(1, 1000);
    mbleep(f / 10, 200);
  }
  /*
  if((r < mindist) && (l < mindist)) {
    if(f < mindist) {
      motor.motor1Reverse(127);
      motor.motor0Forward(127);
      delay(1000);
      motor.motor1Forward(127);
    }
    else {
      motor.motor1Forward(127);
      motor.motor0Reverse(127);
    }
  }
  else */
  if(r < l) {
    if(r < mindist) {
      //mbleep(3, 200);
      //move2(LEFT, 3000);
      motor.motor1Forward(127);
    }
  }
  else if(l < mindist) {
    //mbleep(6, 200);
    //move2(RIGHT, 3000);
    motor.motor0Reverse(127);
  }
  //mbleep(1, 2000);
  //move2(FORWARD, 6000);
  delay(500);
  motor.motor1Forward(127);
  motor.motor0Reverse(127);
  delay(1000);
  motor.motor1Forward(80);
  motor.motor0Reverse(80);
  delay(300);
  motor.motor1Forward(40);
  motor.motor0Reverse(40);
  delay(300);
  motor.motor1Forward(20);
  motor.motor0Reverse(20);
  delay(300);
  motor.stopMotor0();
  motor.stopMotor1();
  //stop();
}

void move2(int dir, int duration)
{
  if(dir == LEFT) {
    lspeed = 0;
  }
  else {
    if(reverse_left == 1) {
      lspeed = -127;
    }
    else {
      lspeed = 127;
    }
  }
  if(dir == RIGHT) {
    rspeed = 0;
  }
  else {
    if(reverse_right == 1) {
      rspeed = -127;
    }
    else {
      rspeed = 127;
    }
  }
  motor.motor0Forward(rspeed);
  motor.motor1Forward(lspeed);
  delay(10000);
}
