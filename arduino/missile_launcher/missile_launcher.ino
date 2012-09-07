// work around the arduino preprocessor bugs
int dummy; 

#include <Servo.h> 
#include <Wire.h>
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// pan servo pin
#define panPin A0
// tilt servo pin
#define tiltPin A1
// trigger servo pin
#define triggerPin A2
// pin to set direction of motor 1
#define motorOneDirPin 8
// pin to set direction of motor 2
#define motorTwoDirPin 9
// pin to set speed of motor 1
#define motorOneSpeedPin A3
// pin to set speed of motor 2
#define motorTwoSpeedPin A4

// useful constants
#define NO_DIR 0x0
#define LEFT 0x1
#define STRAIGHT 0x2
#define RIGHT 0x3
#define BACKWARD 0x4
#define STOP 0x5
#define FORWARD 0x6
#define LESS 0x1
#define MORE 0x2
#define DISABLED 0
#define MIN_PAN 10
#define MAX_PAN 170
#define MIN_TILT 20
#define MAX_TILT 50
#define STEP_PAN 1
#define STEP_TILT 1
#define NO_FACE 0x0
#define FACE_RIGHT 0x1
#define FACE_LEFT 0x2
#define FACE_CENTER 0x3
#define FACE_UP 0x4
#define FACE_DOWN 0x5

struct dist_dir { 
    int dir;
    long dist;
};


AndroidAccessory acc("thomasriga.com",
		     "AndroidRobot",
		     "AndroidRobot",
		     "1.0",
		     "http://www.thomasriga.com",
		     "0000000012345678");

Servo pan_servo, tilt_servo, trigger_servo;
int curr_dir = STRAIGHT, curr_vel = STOP, curr_pan = 0, curr_tilt = 0;
int suggested_dir = NO_FACE, suggested_tilt_dir = NO_FACE, action_timer = 0, tilt_scan_dir = MORE, pan_scan_dir = MORE;
unsigned long starttime, stoptime, diff;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  acc.powerOn();
  Serial.println("usb accessory ON");
  pan_servo.attach(panPin);
  tilt_servo.attach(tiltPin);
  trigger_servo.attach(triggerPin);
  pinMode(motorOneDirPin, OUTPUT);   
  pinMode(motorTwoDirPin, OUTPUT); 
  pan(90);
  tilt(90);
  Serial.println("setup finished");
}

void loop() 
{  
  Serial.println("detect faces");
  detect_faces();
  delay(40);
}

void detect_faces() {
  byte msg[3];
  if (acc.isConnected()) {
    int len = acc.read(msg, sizeof(msg), 1);
    if (len > 0) {
      suggested_dir = msg[0];
      suggested_tilt_dir = msg[1];
    }
  }
  else {
    Serial.println("accessory disconnected");
    suggested_dir = NO_FACE;
    suggested_tilt_dir = NO_FACE;
  }
    
  // pan
  if(suggested_dir == FACE_LEFT) {
    pan_step(LESS);
  }
  else if(suggested_dir == FACE_RIGHT) {
    pan_step(MORE);
  }
  else if(suggested_dir == NO_FACE) {
    if(max_pan()) {
      pan_scan_dir = LESS;
    }
    else if(min_pan()) {
      pan_scan_dir = MORE;
    }
    pan_step(pan_scan_dir);      
  }

  // tilt
  if(suggested_tilt_dir == FACE_UP) {
    tilt_step(LESS);
  }
  else if(suggested_tilt_dir == FACE_DOWN) {
    tilt_step(MORE);
  }
  else if(suggested_tilt_dir == NO_FACE) {
    if(max_tilt()) {
      tilt_scan_dir = LESS;
    }
    else if(min_tilt()) {
      tilt_scan_dir = MORE;
    }
    tilt_step(tilt_scan_dir);      
  }
  
  // shoot
  if((suggested_dir == FACE_CENTER) && (suggested_tilt_dir == FACE_CENTER)) {
    start_motors();
    delay(1000);
    pull_trigger();
    delay(1000);
    release_trigger();
    delay(1000);
    stop_motors();
    delay(1000);
  }
}

void start_motors() {
  digitalWrite(motorOneDirPin, HIGH); // start channel 1 
  //digitalWrite(motorTwoDirPin, HIGH); // Set channel 2 direction as forward
  //analogWrite(motorOneSpeedPin, 255);    // Set the channel 1 speed as full
  //analogWrite(motorTwoSpeedPin, 255);    // Set the channel 2 speed as full
}

void stop_motors() {
  digitalWrite(motorOneDirPin, LOW); // stop channel 1
  //analogWrite(motorOneSpeedPin, 0);      // Set the speed to stop
  //analogWrite(motorTwoSpeedPin, 0);      // Set the speed to stop
}

void pull_trigger() {
  trigger_servo.write(180);
}

void release_trigger() {
  trigger_servo.write(0);
}

void pan(int degrees) {
  int val = degrees;
  if(val > MAX_PAN) {
    val = MAX_PAN;
  }
  if(val < MIN_PAN) {
    val = MIN_PAN;
  }
  pan_servo.write(val);
  curr_pan = val;
}

void pan_step(int direction) {
  if(direction == LESS) {
    pan(curr_pan - STEP_PAN);
  }
  else {
    pan(curr_pan + STEP_PAN);
  }
}

int max_pan() {
  return (curr_pan == MAX_PAN);
}
  
int min_pan() {
  return (curr_pan == MIN_PAN);
}
  
void tilt(int degrees) {
  int val = degrees;
  if(val > MAX_TILT) {
    val = MAX_TILT;
  }
  if(val < MIN_TILT) {
    val = MIN_TILT;
  }
  tilt_servo.write(val);
  curr_tilt = val;
}

void tilt_step(int direction) {
  if(direction == LESS) {
    tilt(curr_tilt - STEP_TILT);
  }
  else {
    tilt(curr_tilt + STEP_TILT);
  }
}

int max_tilt() {
  return (curr_tilt == MAX_TILT);
}
  
int min_tilt() {
  return (curr_tilt == MIN_TILT);
}


