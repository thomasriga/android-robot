// work around the arduino preprocessor bugs
int dummy; 

// comment out one of these lines to configure USB or BlueTooth functionality
//#define BLUETOOTH_COMM
#define USB_COMM

#include <Servo.h> 
#include <MeetAndroid.h>
// comment out the imports when working on a non ADK board 
#include <Wire.h>
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// led pins
#define greenPin 13
#define redPin 12
// pins to devantech srf04
#define srf04EchoPin 8             
#define srf04TriggerPin 9
// pins to left parallax ping (left when looking at the front)
#define leftPingPin 10            
// pins to right parallax ping (right when looking at the front)
#define rightPingPin 11           
// servo input pin
#define steerPin A0
// esc input pin
#define escPin A1
// potentiometer pin
#define potPin A2
// switch pin
#define switchPin 6
// pan servo pin
#define panPin A4
// tilt servo pin
#define tiltPin A3
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
#define MIN_DIST_SIDES 50
#define MIN_DIST_FRONT 60
#define MIN_PAN 10
#define MAX_PAN 170
#define MIN_TILT 10
#define MAX_TILT 170
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

#ifdef BLUETOOTH_COMM
  MeetAndroid meetAndroid;
#endif

#ifdef USB_COMM

  AndroidAccessory acc("thomasriga.com",
		     "CarBotVision",
		     "CarBotVision",
		     "1.0",
		     "http://www.thomasriga.com",
		     "0000000012345678");

#endif

int suggested_dir = NO_DIR;
int suggested_tilt_dir = NO_DIR;
Servo steer_servo;
Servo esc_servo;
Servo pan_servo;
Servo tilt_servo;
int curr_dir = STRAIGHT;
int curr_vel = STOP;
int curr_pan = 0;
int curr_tilt = 0;
int buttonState = 0;
int potState = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  #ifdef USB_COMM
    acc.powerOn();
    Serial.println("usb accessory ON");
  #endif
  // set up devantech srf04
  pinMode(srf04TriggerPin, OUTPUT);
  pinMode(srf04EchoPin, INPUT);
  // no set up necesary for the two ping parallax
  // set up servo connection
  steer_servo.attach(steerPin);
  esc_servo.attach(escPin);
  pan_servo.attach(panPin);
  tilt_servo.attach(tiltPin);
  // set up led pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  // set up switch & potentiometer pin
  pinMode(switchPin, INPUT);     
  pinMode(potPin, INPUT);     
  // default position
  steer_straight();
  pan(90);
  tilt(90);
  // arm esc
  digitalWrite(redPin, HIGH);
  esc_servo.write(31);
  delay(200);
  esc_servo.write(90);
  delay(200);
  digitalWrite(redPin, LOW);
  // say hi
  led_bleep(3, 100, greenPin); 
  
  #ifdef BLUETOOTH_COMM
    meetAndroid.registerFunction(suggest_dir, 'A'); 
  #endif
  
  Serial.println("setup finished");
}

void loop() 
{
  char tmp[20];
  //Serial.println("loop");
  buttonState = digitalRead(switchPin);
  potState = analogRead(potPin);
  
  #ifdef BLUETOOTH_COMM
    meetAndroid.receive();
  #endif
  
  if(buttonState == 1) {
    test();
  }
  else {
    #ifdef USB_COMM
      int msg_len = 6, len, old_len = 0, c = -1, x, message_complete = 0;
      char tmp[400];
      byte msg[msg_len];
      byte parsed_msg[msg_len];
      byte complete_msg[msg_len];
      if (acc.isConnected()) {
        do {
          len = acc.read(msg, msg_len, 1);
          sprintf(tmp, "msg len: %d, buffer contains [%d,%d,%d,%d,%d,%d]", len, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);        
          Serial.println(tmp);

          for(x = 0; x < len; x++) {
            if(msg[x] == 255) {
              c = 0;
            }
            if(c >= 0) {
              parsed_msg[c] = msg[c];
              c++;
              if(c == msg_len) {
                message_complete = 1;
                c = - 1;
                for(x = 0; x < msg_len; x++) {
                  complete_msg[x] = parsed_msg[x];
                }
              }
            }
          }
        } while(len > 0);
        if(message_complete == 1) {  
          // pan
          if((complete_msg[1] + ((complete_msg[2] - complete_msg[1]) / 2)) < 40) {
            suggested_dir = FACE_LEFT;
          }
          else if((complete_msg[1] + ((complete_msg[2] - complete_msg[1]) / 2)) > 60) {
            suggested_dir = FACE_RIGHT;
          }
          else {
            suggested_dir = FACE_CENTER;
          }
          // tilt
          if((complete_msg[3] + ((complete_msg[4] - complete_msg[3]) / 2)) < 40) {
            suggested_tilt_dir = FACE_DOWN;
          }
          else if((complete_msg[3] + ((complete_msg[4] - complete_msg[3]) / 2)) > 60) {
            suggested_tilt_dir = FACE_UP;
          }
          else {
            suggested_tilt_dir = FACE_CENTER;
          }
        }
        else {
          Serial.println("incomplete message");
          suggested_dir = NO_FACE;
          suggested_tilt_dir = NO_FACE;
        }
      }
      else {
        Serial.println("accessory disconnected");
      }
    #endif
    
    // pan
    if(suggested_dir == FACE_LEFT) {
      pan_step(LESS);
    }
    else if(suggested_dir == FACE_RIGHT) {
      pan_step(MORE);
    }
    else if(suggested_dir == NO_FACE) {
      //pan(90);
    }
    // tilt
    if(suggested_tilt_dir == FACE_UP) {
      tilt_step(LESS);
    }
    else if(suggested_tilt_dir == FACE_DOWN) {
      tilt_step(MORE);
    }
    else if(suggested_tilt_dir == NO_FACE) {
      //pan(90);
    }
    delay(20);
    //avoid_obstacles(STRAIGHT);
  }
}

#ifdef BLUETOOTH_COMM
  void suggest_dir(byte flag, byte numOfValues)
  {
    int *val;
    meetAndroid.getIntValues(val);
    char tmp[500];
    sprintf(tmp, "Arduino: received %d values", numOfValues); 
    meetAndroid.send(tmp);
  }
#endif

void avoid_obstacles(int suggestion) {
  long ptime;
  int treshold = 20;
  struct dist_dir distances[3];
  char * tmp = (char *) malloc(sizeof(char) * 512);
   
  // left parallax ping
  ptime = microsec_ping(leftPingPin);
  distances[0].dist = microsec_to_cm(ptime);
  distances[0].dir = LEFT;
  // devantech srf04
  ptime = microsec_srf04();
  distances[1].dist = microsec_to_cm(ptime);
  distances[1].dir = STRAIGHT;
  // right parallax ping
  ptime = microsec_ping(rightPingPin);
  distances[2].dist = microsec_to_cm(ptime);
  distances[2].dir = RIGHT;  
  // sort from minor to major distance
  qsort(distances, 3, sizeof(struct dist_dir), struct_cmp_by_dist);
  
  sprintf(tmp, "A: %d (%ld), B: %d (%ld), C: %d (%ld)", distances[0].dir, distances[0].dist, distances[1].dir, distances[1].dist, distances[2].dir, distances[2].dist);
  Serial.println(tmp);
  
  // check if minimum space is not available and then go backward
  if((distances[0].dist < treshold) || (distances[1].dist < treshold) || (distances[2].dist < treshold)) {
    if(curr_vel != BACKWARD) {
      if(distances[0].dir == STRAIGHT) {
        steer_straight();
      }
      else if(distances[0].dir == LEFT) {
        steer_left();
      }
      else {
        steer_right();
      }
      motor_backward();
      delay(400);
    }
  }
  // determine forward direction
  else {
    if(suggested_dir == NO_DIR) { // go where there is more space 
      if(distances[2].dir == STRAIGHT) {
        steer_straight();
      }
      else if(distances[2].dir == LEFT) {
        steer_left();
      }
      else {
        steer_right();
      } 
      motor_forward();
    }
    else { // suggestion from phone
      if(suggested_dir == LEFT) {
        steer_left();
        motor_forward();
      }
      else if(suggested_dir == RIGHT) {
        steer_right();
        motor_forward();
      }
      else if(suggested_dir == BACKWARD) {
        steer_straight();
        motor_backward();
      }
      else {
        steer_straight();
        motor_forward();
      } 
      motor_forward();
    }
  }
  free(tmp);
  delay(200);
}

void test() {
  long left_ping_dist;
  long srf04_dist;
  long right_ping_dist;
  long ptime;
  // test logic - reading the sonars from left to right
  // and display distance: bleep led once for every 10 cm
  // left parallax ping
  led_bleep(1, 500, redPin);
  ptime = microsec_ping(leftPingPin);
  left_ping_dist = microsec_to_cm(ptime);
  led_bleep(left_ping_dist / 10, 200, greenPin);
  // devantech srf04
  led_bleep(1, 500, redPin);
  ptime = microsec_srf04();
  srf04_dist = microsec_to_cm(ptime);
  led_bleep(srf04_dist / 10, 200, greenPin);
  // right parallax ping
  led_bleep(1, 500, redPin);
  ptime = microsec_ping(rightPingPin);
  right_ping_dist = microsec_to_cm(ptime);
  led_bleep(right_ping_dist / 10, 200, greenPin);
  // steering
  steer_left();
  led_bleep(1, 1000, redPin);
  steer_right();
  led_bleep(1, 1000, greenPin);
  steer_straight();
  led_bleep(1, 1000, redPin);
  // engine
  motor_forward();
  led_bleep(1, 3000, greenPin);
  motor_backward();
  led_bleep(1, 3000, redPin);
  motor_stop(); 
  led_bleep(1, 3000, greenPin);
  // pan/tilt
  pan(20);
  tilt(20);
  led_bleep(1, 1000, redPin);
  pan(160);
  tilt(160);
  led_bleep(1, 1000, greenPin);
  pan(90);
  tilt(90);
  led_bleep(1, 1000, redPin);
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
      steer_servo.write(90);
      curr_dir = STRAIGHT;
    }
}

void steer_right() {
    if(curr_dir != RIGHT) {
      steer_servo.write(40);
      curr_dir = RIGHT;
    }
}

void steer_left() {
  if(curr_dir != LEFT) {
    steer_servo.write(140);
    curr_dir = LEFT;
  }
}

void motor_forward() {
    if(curr_vel == BACKWARD) {
      motor_stop();
    }
    esc_servo.write(90 + (potState / 50));
    curr_vel = FORWARD;
}

void motor_backward() {
    if(curr_vel == FORWARD) {
      motor_stop();
    }
    // extra 10 off
    esc_servo.write(90 - (potState / 50));
    curr_vel = BACKWARD;
}

void motor_stop() {
    if(curr_vel == FORWARD) {
      esc_servo.write(0);
      delay(400);
    }
    /*
    else if(curr_vel == BACKWARD) {
      esc_servo.write(180);
      delay(200);
    }
    */
    esc_servo.write(90);
    delay(200);
    curr_vel = STOP;
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

void led_bleep(int num, int interval, int pin) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
    delay(interval);
  }
}

int struct_cmp_by_dist(const void *a, const void *b) 
{ 
    struct dist_dir *ia = (struct dist_dir *)a;
    struct dist_dir *ib = (struct dist_dir *)b;
    return (int) (ia->dist - ib->dist);
} 
