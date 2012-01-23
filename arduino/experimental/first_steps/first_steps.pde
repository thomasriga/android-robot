void setup() {                
  pinMode(8, OUTPUT);     
  pinMode(9, OUTPUT);     
  pinMode(10, OUTPUT);     
  pinMode(11, OUTPUT);     
  pinMode(12, OUTPUT);     
}

void loop() {
  digitalWrite(8, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  //digitalWrite(10, LOW);    // set the LED off
  digitalWrite(9, HIGH);   // set the LED on
  delay(3000);              // wait for a second
  digitalWrite(8, LOW);    // set the LED off
  digitalWrite(9, LOW);    // set the LED off
  delay(1000);              // wait for a second
  digitalWrite(8, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(8, LOW);    // set the LED off

  digitalWrite(10, HIGH);   // set the LED on
  delay(100);              // wait for a second
  digitalWrite(10, LOW);    // set the LED off
  delay(50);              // wait for a second
  digitalWrite(11, HIGH);   // set the LED on
  delay(100);              // wait for a second
  digitalWrite(11, LOW);    // set the LED off
  delay(50);              // wait for a second
  digitalWrite(12, HIGH);   // set the LED on
  delay(100);              // wait for a second
  digitalWrite(12, LOW);    // set the LED off
}
