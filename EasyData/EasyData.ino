#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Streaming.h>
#include <SD.h>

File dataFile;

#define SEP "\t"

SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

unsigned long pulseHigh = 0;    // number of samples taken (of the PPD42NS signal)
unsigned long pulseTotal = 0;   // number of samples in which the signal was HIGH

#define EMULATE_KEYBOARD true

#define LED_PIN           13
#define SHINYEI_PIN       9

#define HEADER "Elapsed \tTemp\tHumid\tPM*\tLatitude\tLongitude\tUTC"

void setup()  
{
  Serial.begin(57600);
  delay(1000);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
  GPS.sendCommand(PGCMD_NOANTENNA);

  // If the SD card is removed, it will stop here.
  // Thus, it won't behave like a keyboard, so you can reprogram it.
  initSD();
  delay(4000);

#ifdef EMULATE_KEYBOARD
  Keyboard.begin();
  Keyboard.println(HEADER);
  Keyboard.end();
#else
  Serial.println(HEADER);
#endif
  //dataFile.println(HEADER);
}

#define temperatureCommand B00000011 

unsigned long elapsed = 0;
int temperature = -999;
int humidity = -999;
void loop()                     // run over and over again
{
  unsigned long loop_begin = millis();

  while (!GPS.newNMEAreceived()) {
    char c = GPS.read();  
  }
  GPS.parse(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
    //return;  // we can fail to parse a sentence in which case we should just wait for another

  temperature = getTemperature();
  humidity = getHumidity();

  //digitalWrite(LED_PIN, HIGH);
  pulseHigh = 0;
  pulseTotal = 0;
  while(millis() - loop_begin < 5000) { 
    pulseTotal += 1;
    pulseHigh += digitalRead(9);
  }
  //digitalWrite(LED_PIN, LOW);Elapsed   Temp  Humid  PM*  Latitude  Longitude  UTC

  elapsed += 5;
  unsigned int minutes = elapsed / 60;
  unsigned int seconds = elapsed % 60;
  String status = String("00:");
  if (minutes < 10) status += "0";
  status += String(minutes) + ":";
  if (seconds < 10) status += "0";
  status += String(seconds);
  status += SEP + getStatus();

#ifdef EMULATE_KEYBOARD
  Keyboard.begin();
  Keyboard.println(status);
  Keyboard.end();
#else
  Serial.println(status);
#endif
  dataFile.println(status);
  //char *stringptr = GPS.lastNMEA();
  //writeToSD(stringptr);

}


