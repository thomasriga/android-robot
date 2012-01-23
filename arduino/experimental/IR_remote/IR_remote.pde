/*
 *************************************************
 * Arduino Sketch: Polling RC5 Infrared Receiver *
 *************************************************
 *
 *
 * $Source: /home/local/CVS-Repository-WWW/Exercises/Microcontroller/Ex-Microcontroller-Arduino-Y2009/src/site/resources/experiments/pollingrc5infraredreceiver/sketches/PollingRC5InfraredReceiver.pde,v $
 * $Revision: 1.1 $
 * $Date: 2009/10/03 09:14:56 $
 * $Author: rz65 $
 */

#define FALSE 0
#define TRUE 1
#define PIN_INPUT A0
#define PIN_SENSOR_OUTPUT_DATA 9
#define PIN_CURRENT_BIT 13
#define BAUD_RATE 115200
#define USEC_PER_BIT 1778
#define USEC_PER_HALF_BIT 889
#define USEC_PER_QUARTER_BIT 444

static unsigned char startBitFound;
static unsigned char readBitError;
static unsigned char wordCompleted;
static unsigned int systemCode;
static unsigned int commandCode;
static unsigned int toggleBit;
static unsigned char currentBit;
static unsigned char precursor;
static unsigned long timestamp;

static void resetStatus(void);
static void findStartBit(void);
static int readAndDisplaySensorOutput(void);
static unsigned char startBitWasFound(void);
static void readBitSequence(void);
static void appendCommandBit(void);
static void appendToggleBit(void);
static void appendSystemBit(void);
static void readBit(void);
static void printResults(void);
static void delayQuarterBit(void);
static void delayHalfBit(void);


void setup() {
  pinMode(PIN_SENSOR_OUTPUT_DATA,OUTPUT);
  pinMode(PIN_CURRENT_BIT,OUTPUT);
  resetStatus();
  Serial.begin(BAUD_RATE);
}

void loop () {
  if (startBitWasFound()) {
    findStartBit(); 
  } else {
    readBitSequence();
    if (wordCompleted) {
      printResults();
      resetStatus();
    }
  }
}

static void resetStatus(void) {
  startBitFound = FALSE;
  wordCompleted = FALSE;
  toggleBit = FALSE;
  systemCode = 0;
  commandCode = 0;
  timestamp = micros(); 
}

static void findStartBit(void) {
  resetStatus();
  while (readAndDisplaySensorOutput()) ; // wait for falling edge
  delayQuarterBit();
  if (readAndDisplaySensorOutput()) {
    // Oops! expected LOW signal, but found HIGH
    resetStatus();
  } else {
    startBitFound = TRUE;
  }
}

static int readAndDisplaySensorOutput(void) {
  int result = digitalRead(PIN_INPUT);
  if (result) {
    digitalWrite(PIN_SENSOR_OUTPUT_DATA,HIGH);
  } else {
    digitalWrite(PIN_SENSOR_OUTPUT_DATA,LOW);
  }
  return result;
}

static unsigned char startBitWasFound() {
  return startBitFound;
}

static void readBitSequence(void) {
  int i;
  for (int i=1; i<=13; i++) {
    readBit();
    if (readBitError) {
      resetStatus();
      return;
    }
    switch (i) {
    case 1: // C6 inverted 
      currentBit = !currentBit;
      appendCommandBit();
      break;
    case 8: // C5
    case 9: // C4
    case 10: // C3
    case 11: // C2
    case 12: // C1
    case 13: // C0
      appendCommandBit();
      break;
    case 2: 
      appendToggleBit();
      break;
    case 3: // S4
    case 4: // S3
    case 5: // S2
    case 6: // S1
    case 7: // S0
      appendSystemBit();
      break;
    }
    wordCompleted = TRUE;
  }
}

static void readBit(void) {
  delayHalfBit();
  precursor = !readAndDisplaySensorOutput();
  delayHalfBit();
  currentBit = !readAndDisplaySensorOutput();
  if (currentBit) {
    digitalWrite(PIN_CURRENT_BIT,HIGH);
  } else {
    digitalWrite(PIN_CURRENT_BIT,LOW);
  }
  readBitError = (precursor == currentBit);
}

static void appendCommandBit(void) {
  commandCode <<= 1;
  if (currentBit) {
    commandCode |= 1;
  }
}

static void appendToggleBit(void) {
  toggleBit = currentBit;
}

static void appendSystemBit(void) {
  systemCode <<= 1;
  if (currentBit) {
    systemCode |= 1;
  }
}

static void printResults(void) {
  timestamp = micros() - timestamp;
  Serial.print("system: ");
  Serial.print(systemCode);
  Serial.print("  command: ");
  Serial.print(commandCode);
  Serial.print("  toggle: ");
  Serial.print(toggleBit);
  Serial.print("  us: ");
  Serial.println(timestamp);
}

static void delayQuarterBit(void) {
  delayMicroseconds(USEC_PER_QUARTER_BIT);
}

static void delayHalfBit(void) {
  delayMicroseconds(USEC_PER_HALF_BIT);
}



