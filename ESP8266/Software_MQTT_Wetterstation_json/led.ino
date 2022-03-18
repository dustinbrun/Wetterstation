void ledon() {
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    analogWrite(dataled, fadeValue);
    delay(7);
  }
}

void ledoff() {
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    analogWrite(dataled, fadeValue);
    delay(7);
  }
}

void ledblink() {
  ledon();
  delay(200);
  ledoff();
  delay(200);
  ledon();
  delay(200);
  ledoff();
  delay(200);
}
