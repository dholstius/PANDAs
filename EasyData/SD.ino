
#define CHIP_SELECT_PIN 10

void initSD() {

  //Serial.print("initSD(): ");

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(CHIP_SELECT_PIN, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIP_SELECT_PIN, 11, 12, 13)) {
    // don't do anything more:
    while (1) ;
  } else {
    //Serial.println("OK");
  }
  
  // Open up the file we're going to log to!
  dataFile = SD.open("log.tsv", FILE_WRITE);

  if (!dataFile) {
    // Wait forever since we cant write data
    while (1) ;
  }

}

void writeToSD(char *stringptr) {
  uint8_t stringsize = strlen(stringptr);
  if (stringsize != dataFile.write((uint8_t *)stringptr, stringsize))
    Serial.println("ERR");
  if (strstr(stringptr, "RMC"))   
    dataFile.flush();
}

