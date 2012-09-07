#include <Servo.h>

Servo left_servo;         
Servo right_servo;         
int green_led_pin = 13;
int yellow_led_pin = 12;
int red_led_pin = 11;
int left_servo_pin = A0;
int right_servo_pin = A1;
int trigger_pin = A2;
int echo_pin = A3;

void setup() { 
  left_servo.attach(left_servo_pin); 
  right_servo.attach(right_servo_pin);  
  pinMode(green_led_pin, OUTPUT); 
  pinMode(yellow_led_pin, OUTPUT); 
  pinMode(red_led_pin, OUTPUT); 
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);  
} 

void loop() { 
  long msec, cm, wait;
  digitalWrite(green_led_pin, HIGH);   
  delay(300);               
  digitalWrite(green_led_pin, LOW);    
  digitalWrite(yellow_led_pin, HIGH);   
  delay(300);               
  digitalWrite(yellow_led_pin, LOW);    
  digitalWrite(red_led_pin, HIGH);   
  delay(300);               
  digitalWrite(red_led_pin, LOW);                  
  msec = microsec_srf04();
  cm = microsec_to_cm(msec);
  wait = cm * 100;
  digitalWrite(green_led_pin, HIGH);   
  digitalWrite(red_led_pin, HIGH);   
  digitalWrite(yellow_led_pin, HIGH);   
  delay(wait);               
  digitalWrite(green_led_pin, LOW);    
  digitalWrite(red_led_pin, LOW);   
  digitalWrite(yellow_led_pin, LOW);   
  delay(1000); 
 
  left_servo.writeMicroseconds(1700);  
  delay(2000);                      
  left_servo.writeMicroseconds(1300);  
  delay(2000);
  left_servo.writeMicroseconds(1500);  
  delay(2000); 
  right_servo.writeMicroseconds(1700);  
  delay(2000);                      
  right_servo.writeMicroseconds(1300);  
  delay(2000);
  right_servo.writeMicroseconds(1500);  
  delay(2000); 
  
}

long microsec_srf04() {
  long pulse_time;
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  pulse_time = pulseIn(echo_pin, HIGH);
  return pulse_time;
}

long microsec_to_cm(long microsec)
{
  return microsec / 29l / 2l;
}
