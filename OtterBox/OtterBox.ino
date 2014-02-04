/*
  Clock drives interrupt 0 (pin 2) to increment seconds-since-start
 */

#define VERSION        "007"
#define DEBUG

#define LOOP_DURATION  60000

#define RTC_SS_PIN     10
#define RTC_SQW_PIN     2

#define SHINYEI_PIN     9

#define ACC_X_PIN      A0
#define ACC_Y_PIN      A1
#define ACC_Z_PIN      A2
#define ACC_0_PIN      A3

#define SHT_DATA_PIN    8
#define SHT_CLOCK_PIN   7

#define LIGHT_PIN      A5

#include <SPI.h>
#include <Wire.h>

volatile unsigned long last_ISR0 = 0;     // time of exit from last ISR call
volatile unsigned long seconds = 0;

#include <RTClib.h>
#include <RTC_DS3234.h>
RTC_DS3234 RTC(RTC_SS_PIN);

const int acc_samples = 30;
int acc_x, acc_y, acc_z;
int acc_0;
char acc_buf[15];

void setup()
{
  
  // Sample the reference voltage from the accelerometer
  int i;
  acc_0 = 0;
  for(i=0; i<30; i++) {
    acc_0 += analogRead(ACC_0_PIN);
  }
  acc_0 = acc_0 / i / 2;
  
  Serial.begin(115200);
  Serial.print("# Firmware version: ");
  Serial.println(VERSION);
  Serial.print("# Accelerometry: ADC units, recentered on ");
  Serial.println(acc_0);
  Serial.println("# Temperature: Celsius");
  Serial.println("# Relative humidity: percent");
  Serial.println("# Light level: ADC units (0-1023)");
  Serial.println("# Shinyei: percent");
  
  SPI.begin();
  RTC.begin();
  pinMode(RTC_SQW_PIN, INPUT);
  attachInterrupt(0, ISR0, FALLING);

  #ifdef DEBUG
    pinMode(3, OUTPUT);
    digitalWrite(3, HIGH);
  #endif

  // Wait until the second hand rolls over
  int s0 = seconds; 
  while(seconds == s0) {
    ;
  }
  seconds = 0; 
}

void loop()
{
  unsigned long loop_begin = millis();

  Serial.println("");
  
  // 1. Read the clock and print a timestamp
  printSecondsAndMillis();
  Serial.print("clock\t");
  DateTime now = RTC.now();
  char RTC_buf[30];
  sprintf(RTC_buf, "%4d-%02d-%02d %02d:%02d:%02d GMT", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  Serial.println(RTC_buf);
  
  // 2a. Sample the accelerometer continuously at 10 Hz for 3 seconds
  printSecondsAndMillis();
  Serial.print("accel\t");
  doXYZ();

  // Sample the temperature
  printSecondsAndMillis();
  Serial.print("temp\t");
  Serial.println(getTemperature(), 2);
  
  // Sample the humidity
  printSecondsAndMillis();
  Serial.print("humid\t");
  Serial.println(getHumidity(), 2);

  // Sample the humidity
  printSecondsAndMillis();
  Serial.print("light\t");
  Serial.println(analogRead(LIGHT_PIN));
  
  // Watch the Shinyei for (almost) the rest of the time
  printSecondsAndMillis();
  Serial.print("shinyei\t");
  #ifdef DEBUG 
    digitalWrite(3, LOW);
  #endif
  unsigned long high = 0;
  unsigned long total = 0;
  while(millis() - loop_begin < (LOOP_DURATION - 200)) {    // the 200 is for a little wiggle room at the end
    total += 1;
    high += digitalRead(SHINYEI_PIN);
  }
  #ifdef DEBUG
    digitalWrite(3, HIGH);
  #endif
  float percentage = 100.0 * (total - high)/(float)(total);
  Serial.print(percentage, 2);
  Serial.print("\t# ");
  Serial.print(total - high);
  Serial.print("/");
  Serial.println(total);
  
  // Wait until the second hand turns over
  printSecondsAndMillis();
  Serial.println("wait");
  int s0 = seconds; 
  while(seconds == s0) {
    ;
  }

}

void printSecondsAndMillis() {
  int sec = seconds;
  int ms = millis() - last_ISR0;
  if (ms > 999) ms = 999;
  char buf[12];
  sprintf(buf, "%d.%03d\t", sec, ms);
  Serial.print(buf);
}

void ISR0()
{
  last_ISR0 = millis();
  seconds += 1;
}

