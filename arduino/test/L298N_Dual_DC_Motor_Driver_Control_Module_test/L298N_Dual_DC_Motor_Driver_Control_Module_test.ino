// Arduino code for using the L298N Dual DC Motor Driver Control Module
// Courtesy of Babelduck Cybernetics
 
void setup() 
{  
  pinMode(9, OUTPUT);    // direction control of channel 1
  pinMode(10, OUTPUT);    // direction control of channel 2
 
  delay(3000);            // Wait 3 seconds before we start
} 
 
void loop() 
{ 
  digitalWrite(9, HIGH); // Set channel 1 direction as forward
  digitalWrite(10, HIGH); // Set channel 2 direction as forward
  analogWrite(A3, 255);    // Set the channel 1 speed as full
  analogWrite(A4, 255);    // Set the channel 2 speed as full
 
  delay(1000);            // Wait a second
 
  analogWrite(A3, 0);      // Set the speed to stop
  analogWrite(A4, 0);      // Set the speed to stop
 
  delay(1000);            // Wait a second
 
  digitalWrite(9, LOW ); // Set direction as reverse
  digitalWrite(10, LOW ); // Set direction as reverse
  analogWrite(A3, 127);    // Set the speed as half speed
  analogWrite(A4, 127);    // Set the speed as half speed
 
  delay(1000);            // Wait a second
 
  analogWrite(A3, 0);      // Set the speed to stop
  analogWrite(A4, 0);      // Set the speed to stop
 
  delay(1000);            // Wait a second
} 
 
