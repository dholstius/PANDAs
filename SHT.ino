#define SHT_CLOCK_PIN 6 // green wire
#define SHT_DATA_PIN 5  // blue wire

#define temperatureCommand B00000011 
#define humidityCommand B00000101 

float getTemperature() {
  int val;
  sendCommandSHT(temperatureCommand);
  waitForResultSHT();
  val = getData16SHT();
  skipCrcSHT();
  float d1 = -40;
  float d2 = 0.01;
  return(d1 + d2 * val);
}

float getHumidity() {
  // TODO: nonlinear temperature correction
  int val;
  sendCommandSHT(humidityCommand);
  waitForResultSHT();
  val = getData16SHT();
  skipCrcSHT();
  float c1 = -4;
  float c2 = 0.0405;
  float c3 = -2.3E-6;
  return(c1 + c2 * val + c3 * val * val);
}

int shiftIn(int numBits) {
  int ret = 0;

  for (int i=0; i<numBits; ++i) {
    digitalWrite(SHT_CLOCK_PIN, HIGH);
    //delay(10); not needed :)
    ret = ret*2 + digitalRead(SHT_DATA_PIN);
    digitalWrite(SHT_CLOCK_PIN, LOW);
  }
  return(ret);
}

// send a command to the SHTx sensor
void sendCommandSHT(int command) {
  int ack;

  // transmission start
  pinMode(SHT_DATA_PIN, OUTPUT);
  pinMode(SHT_CLOCK_PIN, OUTPUT);
  digitalWrite(SHT_DATA_PIN, HIGH);
  digitalWrite(SHT_CLOCK_PIN, HIGH);
  digitalWrite(SHT_DATA_PIN, LOW);
  digitalWrite(SHT_CLOCK_PIN, LOW);
  digitalWrite(SHT_CLOCK_PIN, HIGH);
  digitalWrite(SHT_DATA_PIN, HIGH);
  digitalWrite(SHT_CLOCK_PIN, LOW);

  // shift out the command (the 3 MSB are address and must be 000, the last 5 bits are the command)
  shiftOut(SHT_DATA_PIN, SHT_CLOCK_PIN, MSBFIRST, command);

  // verify we get the right ACK
  digitalWrite(SHT_CLOCK_PIN, HIGH);
  pinMode(SHT_DATA_PIN, INPUT);
  ack = digitalRead(SHT_DATA_PIN);
  if (ack != LOW)
    Serial.println("ACK error 0");
  digitalWrite(SHT_CLOCK_PIN, LOW);
  ack = digitalRead(SHT_DATA_PIN);
  if (ack != HIGH)
    Serial.println("ACK error 1");
}

// wait for the SHTx answer
void waitForResultSHT() {
  int ack;

  pinMode(SHT_DATA_PIN, INPUT);
  for(int i=0; i<100; ++i) {
    delay(10);
    ack = digitalRead(SHT_DATA_PIN);
    if (ack == LOW)
      break;
  }
  if (ack == HIGH)
    Serial.println("ACK error 2");
}

// get data from the SHTx sensor
int getData16SHT() {
  int val;

  // get the MSB (most significant bits)
  pinMode(SHT_DATA_PIN, INPUT);
  pinMode(SHT_CLOCK_PIN, OUTPUT);
  val = shiftIn(8);
  val *= 256; // this is equivalent to val << 8;

  // send the required ACK
  pinMode(SHT_DATA_PIN, OUTPUT);
  digitalWrite(SHT_DATA_PIN, HIGH);
  digitalWrite(SHT_DATA_PIN, LOW);
  digitalWrite(SHT_CLOCK_PIN, HIGH);
  digitalWrite(SHT_CLOCK_PIN, LOW);

  // get the LSB (less significant bits)
  pinMode(SHT_DATA_PIN, INPUT);
  val |= shiftIn(8);
  return val;
}

// skip CRC data from the SHTx sensor
void skipCrcSHT() {
  pinMode(SHT_DATA_PIN, OUTPUT);
  pinMode(SHT_CLOCK_PIN, OUTPUT);
  digitalWrite(SHT_DATA_PIN, HIGH);
  digitalWrite(SHT_CLOCK_PIN, HIGH);
  digitalWrite(SHT_CLOCK_PIN, LOW);
}

