/*
  Ultrasonic Sensor sketch
 
 This program reads a Devantech SRF04 ultrasonic distance sensor
 The SRF04 sensor's pins are connected as described below.
 Created 9 November 2006
 By Tom Igoe and Neilson Abeel
 */
#define echoPin 8             // the SRF04's echo pin
#define initPin 9             // the SRF04's init pin
int pulseTime = 0;  // variable for reading the pulse

void setup() {
  // make the init pin an output:
  pinMode(initPin, OUTPUT);
  pinMode(13, OUTPUT);
  // make the echo pin an input:
  pinMode(echoPin, INPUT);
  // initialize the serial port:
  //Serial.begin(9600);
}

void mbleep(int num) {
  int x;
  for(x = 0; x < num; x++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
}  

void loop() {
  // send the sensor a 10microsecond pulse:
  digitalWrite(initPin, LOW);
  delayMicroseconds(2);
  digitalWrite(initPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(initPin, LOW);

  // wait for the pulse to return. The pulse
  // goes from low to HIGH to low, so we specify
  // that we want a HIGH-going pulse below:

  pulseTime = pulseIn(echoPin, HIGH);

  // print out that number
  //Serial.println(pulseTime, DEC);
  if(pulseTime > 0) {
    mbleep(pulseTime / 58 / 10);
  }
  else {
    mbleep(8);
  }
  delay(1000);
}

