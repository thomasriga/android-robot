#include <CompactQik2s9v1.h>
#include <NewSoftSerial.h>
#include <Servo.h> 
#include <MeetAndroid.h>

// pins for serial connection to pololu qik 2s9v1
#define rxPin 2
#define txPin 3
// serial connection reset pin is not connected
#define rstPin 4
// led pins
#define yellowPin 5
#define greenPin 6
#define redPin 13
// pins to devantech srf04
#define srf04EchoPin 8             
#define srf04TriggerPin 9
// pins to left parallax ping (left when looking at the front)
#define leftPingPin 10            
// pins to right parallax ping (right when looking at the front)
#define rightPingPin 11           
// servo input pin
#define servoPin A0
// button pin
#define buttonPin A1
// debug flag
#define debug 0
// useful constants
#define LEFT 0
#define STRAIGHT 1
#define RIGHT 2
#define MIN_DIST_SIDES 40
#define MIN_DIST_FRONT 20

Servo myservo;
NewSoftSerial mySerial = NewSoftSerial(rxPin, txPin);
CompactQik2s9v1 motor = CompactQik2s9v1(&mySerial,rstPin);
MeetAndroid meetAndroid;
int curr_dir = STRAIGHT;
int button_state = 0;
int remote_state = 0;

void setup() {
  // baud rate for bluetooth modem on arduino BT
  Serial.begin(115200);
  // define callback for BT commands
  meetAndroid.registerFunction(phoneorient, 'A'); 
  meetAndroid.registerFunction(change_mode, 'B'); 
  // set up serial connection to pololu qik 2s9v1
  mySerial.begin(9600);
  motor.begin();
  // set up devantech srf04
  pinMode(srf04TriggerPin, OUTPUT);
  pinMode(srf04EchoPin, INPUT);
  // no set up necesary for the two ping parallax
  // set up servo connection
  myservo.attach(A0);
  // set up led pins
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  // default position
  steer_straight();
  // say hi
  led_wave(2, 100);
  led_bleep(1, 1000, yellowPin);
  led_wave(2, 100);
}

void loop() 
{
  button_state = digitalRead(buttonPin);
  meetAndroid.receive();
  if (button_state == HIGH) {     
   if(remote_state == 1) { 
    avoid_obstacles();
   }
  } 
  else {
    test(); 
  }
}

void change_mode(byte flag, byte numOfValues)
{
  int val;
  val = meetAndroid.getInt();
  if(val == 0) {
    led_bleep(1, 300, yellowPin); 
  }
  else if(val == 1) {
    led_bleep(1, 300, greenPin); 
  }
  else {
    led_bleep(1, 300, redPin); 
  }  
  remote_state = val;
}

void phoneorient(byte flag, byte numOfValues)
{
  if(remote_state == 0) {
    //Phone Orientation Controller
    int values[]={0,0,0};
    meetAndroid.getIntValues(values);
    // You must hold phone in LANDSCAPE for following orientation.
    int Steer=values[0];   //Steering   , Roll
    int Delta=values[1];   //Throttle  ,  Pitch
    int Heading=values[2];    //Heading  , Yaw
    if(Delta < -15) {
      steer_left();
    }
    else if(Delta < 15) {
      steer_straight();
    }
    else {
      steer_right();
    }
    if (Heading < -15) {   //Forward
      motor_forward();
    }
    else if(Heading > 15) {
      motor_backward();
    }
    else {
      motor_stop();
    }
  }
  
}

void avoid_obstacles() {
  long left_ping_dist;
  long srf04_dist;
  long right_ping_dist;
  long ptime;
  int steer_flag;
  // left parallax ping
  ptime = microsec_ping(leftPingPin);
  left_ping_dist = microsec_to_cm(ptime);
  // devantech srf04
  ptime = microsec_srf04();
  srf04_dist = microsec_to_cm(ptime);
  // right parallax ping
  ptime = microsec_ping(rightPingPin);
  right_ping_dist = microsec_to_cm(ptime);
  // avoid obstacles logic - reading the sonars frontal, left, right
  // and react on a distance < 20 cm frontal & 30 cm on the sides
  steer_flag = 0;
  if(srf04_dist < MIN_DIST_FRONT) {
    if(curr_dir == LEFT) {
      steer_right();
    }
    else if(curr_dir == RIGHT) {
      steer_left();
    }
    else if(left_ping_dist < right_ping_dist) {
      steer_left();
    }
    else {
      steer_right();
    }
    motor_backward();
    delay(3000);
  }
  else if(left_ping_dist < right_ping_dist) {
    if(left_ping_dist < MIN_DIST_SIDES) {
      steer_right();
      steer_flag = 1;
    }
    motor_forward();
  }
  else {
    if(right_ping_dist < MIN_DIST_SIDES) {
      steer_left();
      steer_flag = 1;
    }
    if(steer_flag == 0) {
      steer_straight();
    }
    motor_forward();
  } 
}

void test() {
  long left_ping_dist;
  long srf04_dist;
  long right_ping_dist;
  long ptime;
  led_wave(9, 100);
  // test logic - reading the sonars from left to right
  // and display distance: bleep led once for every 10 cm
  // left parallax ping
  ptime = microsec_ping(leftPingPin);
  left_ping_dist = microsec_to_cm(ptime);
  led_bleep(left_ping_dist / 10, 200, yellowPin);
  // devantech srf04
  ptime = microsec_srf04();
  srf04_dist = microsec_to_cm(ptime);
  led_bleep(srf04_dist / 10, 200, greenPin);
  // right parallax ping
  ptime = microsec_ping(rightPingPin);
  right_ping_dist = microsec_to_cm(ptime);
  led_bleep(right_ping_dist / 10, 200, redPin);
  // steering
  steer_left();
  led_bleep(1, 3000, yellowPin);
  steer_right();
  led_bleep(1, 3000, greenPin);
  steer_straight();
  led_bleep(1, 3000, redPin);
  // engine
  motor_forward();
  led_bleep(1, 3000, yellowPin);
  motor_backward();
  led_bleep(1, 3000, greenPin);
  motor_stop(); 
  led_bleep(1, 3000, redPin);
}

long microsec_srf04() {
  long pulseTime;
  digitalWrite(srf04TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(srf04TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(srf04TriggerPin, LOW);
  pulseTime = pulseIn(srf04EchoPin, HIGH);
  return pulseTime;
}

int microsec_ping(int pin) {
  long pulseTime;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  pulseTime = pulseIn(pin, HIGH);
  return pulseTime;
}

long microsec_to_cm(long microsec)
{
  return microsec / 29 / 2;
}

void steer_straight() {
    if(curr_dir != STRAIGHT) {
      myservo.write(90);
      curr_dir = STRAIGHT;
    }
}

void steer_right() {
    if(curr_dir != RIGHT) {
      myservo.write(50);
      curr_dir = RIGHT;
    }
}

void steer_left() {
  if(curr_dir != LEFT) {
    myservo.write(130);
    curr_dir = LEFT;
  }
}

void motor_forward() {
    motor.motor0Forward(127);
}

void motor_backward() {
    motor.motor0Reverse(127);
}

void motor_stop() {
    motor.motor0Forward(0);
}

void led_bleep(int num, int interval, int pin) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
    delay(interval);
  }
}

void led_wave(int num, int interval) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(yellowPin, HIGH);
    delay(interval);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin, HIGH);
    delay(interval);
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    delay(interval);
    digitalWrite(redPin, LOW);
    delay(interval);
  }
}


