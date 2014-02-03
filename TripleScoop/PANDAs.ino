#define FIRMWARE_VERSION  "v1.21"

#include <cstddef>
#include <string.h>

// For reading the Arduino's SERIAL from the EEPROM
#include <EEPROM.h>

// For the AdaFruit Ultimate GPS Shield
#include <SoftwareSerial.h>
#include <SD.h>
#include <Adafruit_GPS.h>
File SDcard;

#define SEP          "\t"
#define EOL        "\r\n"
#define NA         "-999"
#define INNER_LOOP  10000
#define INTERVAL    30000
#define NUM_SHINYEIS    3
#define temperatureCommand B00000011
//#define DEBUG true
//#define EMULATE_KEYBOARD true

// Create SoftwareSerial port on pins 7 and 8 for the AdaFruit GPS shield
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
#define PMTK_SET_NMEA_UPDATE_RATE  "$PMTK220,1000*1F"  // once every 1000 milliseconds

// Buffer for the SERIAL expected to be found in EEPROM
#define SERIAL_LENGTH 20

// For the SD card
#define CHIP_SELECT_PIN 10

void setup()
{

  Serial.begin(57600);
  Serial.flush();

  // Print the firmware version
  Serial.print(EOL);
  Serial.print("# Firmware: ");
  Serial.println(FIRMWARE_VERSION);

  // Read the SERIAL stored in EEPROM
  char sID[SERIAL_LENGTH + 1];
  for (int i = 0; i < SERIAL_LENGTH; i++) {
    sID[i] = EEPROM.read(i);
  }
  sID[SERIAL_LENGTH] = '\0';

  // Print the SERIAL to the serial port
  Serial.print("# EEPROM: ");
  Serial.println(sID);

  delay(1000);

  // Initialize the SD card and open the log file for writing
  Serial.print("# SD: ");
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  if (!SD.begin(CHIP_SELECT_PIN, 11, 12, 13)) {
    Serial.println("ERR");
    while (1) ; // Wait forever
  }
  SDcard = SD.open("PANDA.log", FILE_WRITE);
  if (!SDcard) {
    Serial.println("ERR");
    while (1) ; // Wait forever
  }
  Serial.println("OK");

  // Write the firmware version and serial number to the log file
  SDcard.print(EOL);
  SDcard.print("# Firmware: ");
  SDcard.println(FIRMWARE_VERSION);
  SDcard.print("# EEPROM: ");
  SDcard.println(sID);
  SDcard.flush();

  delay(1000);

  // Initialize the GPS
  emit("# GPS: ", "");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_RATE);
  GPS.sendCommand(PGCMD_NOANTENNA);
  delay(500);

  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  delay(500);

  mySerial.flush();

  emit("OK", EOL);

  // Digital pins reserved for Shinyeis are expected to start at 2 and go up to 4
  for (int j = 0; j < NUM_SHINYEIS; j++) {
    pinMode(2 + j, INPUT_PULLUP);
  }

  // Analog pin A0 is for the TEMT6000 light sensor
  pinMode(A0, INPUT);

  // Analog pins A4 and A5 are SCL and SDA (I2C)

}

// Variable to track number of records so we can emit T, RH, and GPS
// every Nth record instead of every single time
unsigned int loop_count = 0;

void loop()
{

  // Variables to keep track of elapsed time
  uint32_t millis_start, millis_elapsed;

  // Every so often, sample RH, T, and GPS instead of sampling the Shinyeis
  // (Then fall through after emitting the record---doesn't take long)
  if (loop_count % 10 == 0) {

    // Variables for auxiliary sensors
    float T, RH;
    int L;

    millis_start = millis();
    L = analogRead(A0);
    millis_elapsed = millis() - millis_start;
    emit(EOL, millis_start);
    emit(SEP, "TEMT6000.mV");
    emit(SEP, map(L, 0, 1023, 0, 5000));
    emit(SEP, "ms=", millis_elapsed);

    millis_start = millis();
    T = getTemperature();
    millis_elapsed = millis() - millis_start;
    emit(EOL, millis_start);
    emit(SEP, "SHT15.T.Celsius");
    emit(SEP, T, 2);
    emit(SEP, "ms=", millis_elapsed);

    millis_start = millis();
    RH = getHumidity();
    millis_elapsed = millis() - millis_start;
    emit(EOL, millis_start);
    emit(SEP, "SHT15.RH.percent");
    emit(SEP, RH, 2);
    emit(SEP, "ms=", millis_elapsed);

    SDcard.flush();

    // FIXME
    millis_start = millis();
    char * lastNMEA = pollGPS(10000);
    millis_elapsed = millis() - millis_start;
    if (lastNMEA) {
      emit(EOL, millis_start);
      emit(SEP, "MTK3339.NMEA");
      emit(SEP, lastNMEA);
      emit(SEP, "ms=", millis_elapsed);

      if (strncmp(lastNMEA, "$GPRMC", 6) == 0) {
        processGPRMC(lastNMEA);
      }
    }

    SDcard.flush();

  }
  else {

    // Counters and variables for the Shinyeis
    uint8_t bits;
    unsigned long sample_count = 0;
    unsigned long P1_low[NUM_SHINYEIS];
    float P1_ratio[NUM_SHINYEIS], P1_millis[NUM_SHINYEIS];

    // Reset counters for Shinyeis
    for (int j = 0; j < NUM_SHINYEIS; j++) {
      P1_low[j] = 0;
    }

    // Mark the start time
    millis_start = millis();
    millis_elapsed = 0;

    // This loop runs for at least INTERVAL milliseconds
    for (millis_elapsed = 0; millis_elapsed < INTERVAL; millis_elapsed = millis() - millis_start) {

      // This inner loop gets us a chunk of 10000 parallel samples
      // Saves unnecessary calls to millis() --- don't need more precision
      unsigned long i;
      for (i = 0; i < 10000; i++) {
        bits = PIND;  // Pins D2-D7
        if (bitRead(bits, 2) == 0) P1_low[0]++;
        if (bitRead(bits, 3) == 0) P1_low[1]++;
        if (bitRead(bits, 4) == 0) P1_low[2]++;
      }
      sample_count += i;

    }

    // Calculate and print PWM duty cycle for each Shinyei
    for (int j = 0; j < NUM_SHINYEIS; j++) {
      P1_ratio[j] = (float)P1_low[j] / (float)sample_count;
      emit(EOL, millis_start);
      emit(SEP, "PPD42NS.P1.ratio");
      emit(SEP, P1_ratio[j], 8);
      emit(SEP, "ms=", millis_elapsed);
      emit(SEP, "pin=D", j + 2);
    }

    SDcard.flush();

  }

  //Serial.flush();
  //SDcard.flush();

  // Increment record counter
  loop_count++;

}


