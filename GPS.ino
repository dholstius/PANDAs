char * pollGPS(int timeout) {
  mySerial.flush();
  delay(1000);
  while (!GPS.newNMEAreceived()) {
    char c = GPS.read();
#ifdef DEBUG
    Serial.print(c);
#endif
    if (--timeout == 0) {
      return NULL;
    }
  }
  return GPS.lastNMEA();
}

void processGPRMC(char * NMEAstring) {
  uint32_t millis_start = millis();
  char * buf = strdup(NMEAstring);  // copy string into buffer
  char * p = buf;                   // point to start of buffer
  const int NUM_TOKENS = 11;        // expect 11 tokens
  char * tokens[NUM_TOKENS];        // char array for tokens
  int i;                            // loop counter
  bool valid = true;                // start by assuming it's complete and valid
  for (i = 0; i < NUM_TOKENS; i++) {
    tokens[i] = strsep(&p, ",");    // strsep() overwrites delimiter (in buf) with NULL, *and* advances p
    if (!tokens[i]) {               // NULL result from strsep() means no more delimiters
      valid = false;                // we expected at least one more delimiter, so this isn't a valid GPRMC string
      break;
    }
  }
  if (valid) {

    // permute elements of timestamp and datestamp to form ISO-8601 UTC string (YYYY-mm-dd HH:MM:SS)
    millis_start = millis();
    char * timestamp = tokens[1];
    char * datestamp = tokens[9];
    char YYYYmmdd[11] = {'2', '0', *(datestamp + 4), *(datestamp + 5), '-', *(datestamp + 2), *(datestamp + 3), '-', *datestamp, *(datestamp + 1), '\0'};
    char HHMMSS[9] = {*timestamp, *(timestamp + 1), ':', *(timestamp + 2), *(timestamp + 3), ':', *(timestamp + 4), *(timestamp + 5), '\0'};

    // emit ISO-8601 UTC string
    uint32_t millis_ms = millis() - millis_start;
    emit(EOL, millis_start);
    emit(SEP, "MTK3339.UTC");
    emit(SEP, YYYYmmdd);
    emit(" ", HHMMSS);
    emit(SEP, "ms=", millis_ms);
    
    // emit latitude in decimal degrees
    millis_start = millis();
    char * latitude_repr = tokens[3];
    if (strlen(latitude_repr) > 0) {
      millis_ms = millis() - millis_start;
      emit(EOL, millis_start);
      emit(SEP, "MTK3339.LAT");
      float latitude_value = NMEA_to_decimal(atof(latitude_repr));
      if (*tokens[4] == 'S') latitude_value *= -1;
      emit(SEP, latitude_value, 6);
      emit(SEP, "ms=", millis_ms);
    }

    // emit longitude in decimal degrees
    millis_start = millis();
    char * longitude_repr = tokens[5];
    if (strlen(longitude_repr) > 0) {
      millis_ms = millis() - millis_start;
      emit(EOL, millis_start);
      emit(SEP, "MTK3339.LON");
      float longitude_value = NMEA_to_decimal(atof(longitude_repr));
      if (*tokens[6] == 'W') longitude_value *= -1;
      emit(SEP, longitude_value, 6);
      emit(SEP, "ms=", millis_ms);
    }
  }

  free(buf);   // free the memory consumed by strdup() at the start of this function
}

float NMEA_to_decimal(float NMEA) {
  float whole_degrees = floor(NMEA / 100.0);
  float decimal_minutes = NMEA - (100.0 * whole_degrees);
  return whole_degrees + (decimal_minutes / 60.0);
}

