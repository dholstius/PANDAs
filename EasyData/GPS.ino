float NMEA_to_decimal(float NMEA) {
  float deg = floor(NMEA / 100);
  float minutes = 100.0 * ((NMEA / 100.0) - deg);
  return deg + (minutes / 60.0);
}  

