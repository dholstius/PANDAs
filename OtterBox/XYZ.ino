void doXYZ() {

  int i;
  unsigned long u0 = 0;

  u0 = millis();
  acc_x = analogRead(ACC_X_PIN) - acc_0;
  acc_y = analogRead(ACC_Y_PIN) - acc_0;
  acc_z = analogRead(ACC_Z_PIN) - acc_0;
  sprintf(acc_buf, "[(%d,%d,%d)", acc_x, acc_y, acc_z);
  Serial.print(acc_buf);
  delay(100 - (millis() - u0));

  for(i=1; i<acc_samples; i++) {
    u0 = millis();
    acc_x = analogRead(ACC_X_PIN) - acc_0;
    acc_y = analogRead(ACC_Y_PIN) - acc_0;
    acc_z = analogRead(ACC_Z_PIN) - acc_0;
    sprintf(acc_buf, ",(%d,%d,%d)", acc_x, acc_y, acc_z);
    Serial.print(acc_buf);
    delay(100 - (millis() - u0));
  }

  Serial.println("]");
}

