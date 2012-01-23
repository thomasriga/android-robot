int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

#define IR_CLOCK_RATE    36000L

#define pwmPin 11   // IR Carrier 

void setup()  {
  // set the data rate for the Serial port
  Serial.begin(2400);
  
  // toggle on compare, clk/1
  TCCR2A = _BV(WGM21) | _BV(COM2A0);
  TCCR2B = _BV(CS20);
  // 36kHz carrier/timer
  OCR2A = (F_CPU/(IR_CLOCK_RATE*2L)-1);
  pinMode(pwmPin, OUTPUT);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);    
}

void loop() 
{
  char i;
  //for(i='A'; i<='Z'; i++)
  //{
  //  Serial.print(i);
  //}
  Serial.print('M');
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin); 
  if(!sensorValue) {  
    // turn the ledPin on
    digitalWrite(ledPin, HIGH);  
    // stop the program for <sensorValue> milliseconds:
    delay(1000);          
    // turn the ledPin off:        
    digitalWrite(ledPin, LOW);   
    delay(100);          
  }
  delay(40);
}
 
