void readsensors() {
  ledon();
  digitalWrite(regensensorpower, HIGH);
  delay(50);

  temp = 0;
  bodentemp = 0;
  druck_sensorwert = 0;
  feuchte = 0;
  lux = 0;
  regensensorwert = 0;
  uv = 0;

  for (int i = 0; i < 3; ++i) {
    //BME280
    //temp += bme.readTemperature();
    druck_sensorwert += bme.readPressure();
    feuchte += bme.readHumidity();

    //BH1750
    lux += lightMeter.readLightLevel();

    //DS18B20
    ds18b20.requestTemperatures();
    bodentemp += ds18b20.getTempC(ds18b20_0m);
    temp += ds18b20.getTempC(ds18b20_2m);

    //VEML6070
    uv += uvMeter.readUV(); //measurement takes as long as the integration time is set

    //Regensensor
    regensensorwert += analogRead(regensensorpin);

    delay(100);
  }
  temp = temp /  3;
  druck_sensorwert = druck_sensorwert / 3;
  feuchte = feuchte / 3;
  lux = lux / 3;
  bodentemp = bodentemp / 3;
  uv = uv / 3;
  regensensorwert = regensensorwert / 3;
  digitalWrite(regensensorpower, LOW);

  //Rechnen
  //druck = (druck_sensorwert / 100) + (hoehe / 8.5);
  druck = druck_sensorwert / 100;
  druck = bme.seaLevelForAltitude(hoehe, druck);

  ledoff();
}
