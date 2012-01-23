//#include <Servo.h> 

#define srf04EchoPin 8             
#define srf04TriggerPin 9
#define leftPingPin 10            
#define rightPingPin 11           

//Servo myservo;

void setup() {
  Serial.begin(115200);
  Serial.print("\r\nStart");
  //myservo.attach(A0);
  pinMode(srf04TriggerPin, OUTPUT);
  pinMode(srf04EchoPin, INPUT);

}

void loop() 
{
  int left_ping_dist;
  int srf04_dist;
  int right_ping_dist;
  int ptime;
  
  ptime = microsec_ping(leftPingPin);
  left_ping_dist = microsec_to_cm(ptime);
  Serial.print("left_ping_dist: ");
  Serial.println(left_ping_dist);

  // devantech srf04
  ptime = microsec_srf04();
  srf04_dist = microsec_to_cm(ptime);
  // right parallax ping
  ptime = microsec_ping(rightPingPin);
  right_ping_dist = microsec_to_cm(ptime);
  Serial.print("right_ping_dist: ");
  Serial.println(right_ping_dist);
  /*
  if((right_ping_dist == 0) && (left_ping_dist == 0)) {
    myservo.write(90); 
  }
  else if(right_ping_dist >= left_ping_dist) {
    myservo.write(40);
  }
  else {
    myservo.write(140);
  }
  */
  delay(1000);
}

int microsec_srf04() {
  int pulseTime;
  digitalWrite(srf04TriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(srf04TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(srf04TriggerPin, LOW);
  pulseTime = pulseIn(srf04EchoPin, HIGH);
  return pulseTime;
}

long microsec_ping(int pin) {
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
