void emit(char * sep, float val, int digits) {
  Serial.print(sep); Serial.print(val, digits);
  SDcard.print(sep); SDcard.print(val, digits);
}

void emit(char * sep, char * str, float val, int digits) {
  Serial.print(sep); Serial.print(str); Serial.print(val, digits);
  SDcard.print(sep); SDcard.print(str); SDcard.print(val, digits);
}

void emit(char * sep, char * str) {
  Serial.print(sep); Serial.print(str);
  SDcard.print(sep); SDcard.print(str);
}

void emit(char * sep, char * str, int val) {
  Serial.print(sep); Serial.print(str); Serial.print(val);
  SDcard.print(sep); SDcard.print(str); SDcard.print(val);
}

void emit(char * sep, uint32_t val) {
  Serial.print(sep); Serial.print(val);
  SDcard.print(sep); SDcard.print(val);
}

void emit(uint32_t val) {
  Serial.print(val);
  SDcard.print(val);
}

void emit(char * sep, uint32_t val, char * eol) {
  Serial.print(sep); Serial.print(val); Serial.print(eol);
  SDcard.print(sep); SDcard.print(val); SDcard.print(eol);
}
