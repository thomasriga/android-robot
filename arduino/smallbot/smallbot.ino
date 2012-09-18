#include <Servo.h>

Servo left_servo;         
Servo right_servo;         
int green_led_pin = 13;
int yellow_led_pin = 12;
int red_led_pin = 11;
int right_servo_pin = A0;
int left_servo_pin = A1;
int trigger_pin = A2;
int echo_pin = A3;
int dir_flag = 0;

void setup() { 
  left_servo.attach(left_servo_pin); 
  right_servo.attach(right_servo_pin);  
  pinMode(green_led_pin, OUTPUT); 
  pinMode(yellow_led_pin, OUTPUT); 
  pinMode(red_led_pin, OUTPUT); 
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);  
  red_blink(300); 
  yellow_blink(300);
  green_blink(300);
} 

void loop() {
  rotate_just_enough();
}

// best one until now
void rotate_just_enough() {
  unsigned long cm = 0, min_dist = 20;
  int rotated = 0;
  cm = microsec_to_cm(microsec_srf04());
  while(cm < min_dist) {
    if(dir_flag) {
      rotate_left_msec(50);
    }
    else {
      rotate_right_msec(50);
    }
    cm = microsec_to_cm(microsec_srf04());
    rotated = 1;
  }
  if(dir_flag) {
    dir_flag = 0;
  }
  else {
    dir_flag = 1;
  }
  if(!rotated) {
    if(dir_flag) {
      rotate_left_msec(200);
    }
    else {
      rotate_right_msec(200);
    }
  }
  cm = microsec_to_cm(microsec_srf04());
  if(cm >= min_dist) {
    forward_msec(200);
  }
  else if(cm < (min_dist/2)) {
    backward_msec(400);
  } 
}

void simple_3_way_max_dist() {
  unsigned long msec1, msec2, msec3;
  all_blink(500);
  rotate_left_msec(500);
  msec1 = microsec_srf04();
  rotate_right_msec(500);
  msec2 = microsec_srf04();
  rotate_right_msec(500);
  msec3 = microsec_srf04();
  if(msec1 > msec2) {
    if(msec1 > msec3) {
      // left wins
      rotate_left_msec(1000);
      green_blink(500);
    }
  }
  if(msec2 > msec1) {
    if(msec2 > msec3) {
      // center wins
      rotate_left_msec(500);
      yellow_blink(500);
    }
  }
  if(msec3 > msec1) {
    if(msec3 > msec1) {
      // right wins, do nothing
      red_blink(500);
    }
  }
  forward_msec(1000);
  delay(20);
}

void test() { 
  unsigned long msec, cm, wait;
  msec = microsec_srf04();
  cm = microsec_to_cm(msec);
  wait = cm * 10;
  all_blink(wait); 
  green_blink(500);
  left_forward_msec(2000);
  green_blink(500);
  left_backward_msec(2000);
  yellow_blink(500);
  right_forward_msec(2000);
  yellow_blink(500);
  right_backward_msec(2000);
  red_blink(500);
  rotate_left_msec(2000);
  red_blink(500);
  rotate_right_msec(2000);
}

unsigned long microsec_srf04() {
  unsigned long pulse_time;
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  pulse_time = pulseIn(echo_pin, HIGH);
  return pulse_time;
}

unsigned long microsec_to_cm(unsigned long microsec)
{
  return microsec / 29l / 2l;
}

void forward() {
  left_forward();
  right_forward();
}

void backward() {
  left_backward();
  right_backward();
}

void forward_msec(int msec) {
  forward();
  delay(msec);
  stop();
}

void backward_msec(int msec) {
  backward();
  delay(msec);
  stop();
}

void left_forward() {
  left_servo.writeMicroseconds(1300);  
}

void left_backward() {
  left_servo.writeMicroseconds(1700);  
}

void left_stop() {
  left_servo.writeMicroseconds(1500);  
}

void right_forward() {
  right_servo.writeMicroseconds(1700);  
}

void right_backward() {
  right_servo.writeMicroseconds(1300);  
}

void right_stop() {
  right_servo.writeMicroseconds(1500);  
}

void left_forward_msec(int msec) {
  left_forward();
  delay(msec);
  left_stop();  
}

void left_backward_msec(int msec) {
  left_backward();
  delay(msec);
  left_stop();  
}

void right_forward_msec(int msec) {
  right_forward();
  delay(msec);
  right_stop();  
}

void right_backward_msec(int msec) {
  right_backward();
  delay(msec);
  right_stop();  
}

void stop() {
  left_stop();
  right_stop();
}

void rotate_left() {
  left_backward();
  right_forward();
}

void rotate_right() {
  right_backward();
  left_forward();
}

void rotate_left_msec(int msec) {
  rotate_left();
  delay(msec);
  stop();
}

void rotate_right_msec(int msec) {
  rotate_right();
  delay(msec);
  stop();
}

void green_blink(int msec) {
  digitalWrite(green_led_pin, HIGH);   
  delay(msec);               
  digitalWrite(green_led_pin, LOW);    
}

void yellow_blink(int msec) {
  digitalWrite(yellow_led_pin, HIGH);   
  delay(msec);               
  digitalWrite(yellow_led_pin, LOW);    
}

void red_blink(int msec) {
  digitalWrite(red_led_pin, HIGH);   
  delay(msec);               
  digitalWrite(red_led_pin, LOW);    
}

void all_blink(int msec) {
  digitalWrite(green_led_pin, HIGH);   
  digitalWrite(yellow_led_pin, HIGH);   
  digitalWrite(red_led_pin, HIGH);   
  delay(msec);               
  digitalWrite(green_led_pin, LOW);    
  digitalWrite(yellow_led_pin, LOW);    
  digitalWrite(red_led_pin, LOW);    
}
