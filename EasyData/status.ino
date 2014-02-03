#define SEP "\t"

String formatFloat(float x, int places) {
  long integerPart = long(x);
  long pow = 1000000;
  long fractionalPart = long(pow * (x - integerPart));
  return String(integerPart) + "." + String(fractionalPart);
}

String getStatus() {

  String record = String("");
  // GPS: fix
  //String record = String((int)GPS.fix);
  //record += SEP;

  // Temperature and humidity
  record += int(temperature); 
  record += SEP;
  record += int(humidity);
  record += SEP;

  // Shinyei
  //float percentage = 100.0 * (pulseTotal - pulseHigh) / (float)(pulseTotal);
  record += int(1000 * float(pulseTotal - pulseHigh) / pulseTotal);
  record += SEP;

  /*
  record += pulseTotal;
   record += SEP;
   record += pulseHigh;
   record += SEP;
   */

  // GPS: date and time
  if (GPS.fix != 0) {
    // GPS: latitude, longitude, and altitude
    record += (GPS.lat == 'S' ? "-" : "");
    record += formatFloat(NMEA_to_decimal(GPS.latitude), 6);
    record += SEP;
    record += (GPS.lon == 'W' ? "-" : "");
    record += formatFloat(NMEA_to_decimal(GPS.longitude), 6);
    record += SEP;
    //record += formatFloat(GPS.altitude, 1);
    //record += SEP;  
    record += "20";
    if (GPS.month < 10) record += "0";
    record += GPS.year;
    record += "-";
    if (GPS.month < 10) record += "0";
    record += GPS.month;
    record += "-";
    if (GPS.day < 10) record += "0";
    record += GPS.day;
    record += " "; 
    if (GPS.hour < 10) record += "0";
    record += GPS.hour;
    record += ":";
    if (GPS.minute < 10) record += "0";
    record += GPS.minute;
    record += ":";
    if (GPS.seconds < 10) record += "0";
    record += GPS.seconds;
    record += SEP;


  } 
  else {
    record += SEP;
    record += SEP;
    record += SEP;
    //record += SEP;
  }


  return record;
}



