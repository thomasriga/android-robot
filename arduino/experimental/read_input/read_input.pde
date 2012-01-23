int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin); 
  if(sensorValue > 65) {  
    // turn the ledPin on
    digitalWrite(ledPin, HIGH);  
    // stop the program for <sensorValue> milliseconds:
    delay(1000);          
    // turn the ledPin off:        
    digitalWrite(ledPin, LOW);   
    delay(100);          
  }
}

