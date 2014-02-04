/*
  
 For SHT15 temperature/humidity sensor

*/

const int temperatureCommand  = B00000011; 
const int humidityCommand = B00000101; 

float getTemperature() {
  int val;
  sendCommandSHT(temperatureCommand, SHT_DATA_PIN, SHT_CLOCK_PIN);
  waitForResultSHT(SHT_DATA_PIN);
  val = getData16SHT(SHT_DATA_PIN, SHT_CLOCK_PIN);
  skipCrcSHT(SHT_DATA_PIN, SHT_CLOCK_PIN);
  float d1 = -40;
  float d2 = 0.01;
  return(d1 + d2 * val);
}

float getHumidity() {
  // TODO: nonlinear temperature correction
  int val;
  sendCommandSHT(humidityCommand, SHT_DATA_PIN, SHT_CLOCK_PIN);
  waitForResultSHT(SHT_DATA_PIN);
  val = getData16SHT(SHT_DATA_PIN, SHT_CLOCK_PIN);
  skipCrcSHT(SHT_DATA_PIN, SHT_CLOCK_PIN);
  float c1 = -4;
  float c2 = 0.0405;
  float c3 = -2.3E-6;
  return(c1 + c2 * val + c3 * val * val);
}

int shiftIn(int dataPin, int clockPin, int numBits) {
  int ret = 0;

  for (int i=0; i<numBits; ++i) {
    digitalWrite(clockPin, HIGH);
    //delay(10); not needed :)
    ret = ret*2 + digitalRead(dataPin);
    digitalWrite(clockPin, LOW);
  }
  return(ret);
}

// send a command to the SHTx sensor
void sendCommandSHT(int command, int dataPin, int clockPin) {
  int ack;

  // transmission start
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, LOW);

  // shift out the command (the 3 MSB are address and must be 000, the last 5 bits are the command)
  shiftOut(dataPin, clockPin, MSBFIRST, command);

  // verify we get the right ACK
  digitalWrite(clockPin, HIGH);
  pinMode(dataPin, INPUT);
  ack = digitalRead(dataPin);
  if (ack != LOW)
    Serial.println("ACK error 0");
  digitalWrite(clockPin, LOW);
  ack = digitalRead(dataPin);
  if (ack != HIGH)
    Serial.println("ACK error 1");
}

// wait for the SHTx answer
void waitForResultSHT(int dataPin) {
  int ack;

  pinMode(dataPin, INPUT);
  for(int i=0; i<100; ++i) {
    delay(10);
    ack = digitalRead(dataPin);
    if (ack == LOW)
      break;
  }
  if (ack == HIGH)
    Serial.println("ACK error 2");
}

// get data from the SHTx sensor
int getData16SHT(int dataPin, int clockPin) {
  int val;

  // get the MSB (most significant bits)
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  val = shiftIn(dataPin, clockPin, 8);
  val *= 256; // this is equivalent to val << 8;

  // send the required ACK
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  // get the LSB (less significant bits)
  pinMode(dataPin, INPUT);
  val |= shiftIn(dataPin, clockPin, 8);
  return val;
}

// skip CRC data from the SHTx sensor
void skipCrcSHT(int dataPin, int clockPin) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

