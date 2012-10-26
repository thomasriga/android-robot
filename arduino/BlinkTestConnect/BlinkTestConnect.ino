#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

int led = 13;

AndroidAccessory acc("Lcaggio, Inc.",
		     "DemoKit",
		     "DemoKit Arduino Board",
		     "1.0",
		     "http://www.android.com",
		     "0000000012345678");

void setup() {                
  pinMode(led, OUTPUT);     
  digitalWrite(led, LOW);
  
  Serial.begin(115200);
  Serial.print("\r\nStart");
  
  acc.powerOn();  
}

void loop() {
  byte err;
  byte idle;
  static byte count = 0;
  
  byte msg[3];
  
  if (acc.isConnected()) {
    int len = acc.read(msg, sizeof(msg), 1);
    int i;
    byte b;
    
    if (len > 0) {
      if (msg[0] == 1) {
        digitalWrite(led, HIGH);
      } else {
        digitalWrite(led, LOW);
      }
    }
    
    // digitalWrite(led, HIGH);  
    // delay(1000);              
    // digitalWrite(led, LOW);   
    // delay(1000);              
  }
  //digitalWrite(led, LOW);
}
