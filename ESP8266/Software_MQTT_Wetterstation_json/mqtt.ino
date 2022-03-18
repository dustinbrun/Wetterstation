/*
  Json format:
  {
  "temp_2m": 22.45,
  "temp_0m": 12.45,
  "druck": 1234.56,
  "feuchte": 45.4,
  "helligkeit": 12345.56,
  "regensensor": "256",
  "uvsensor" : "1234"
  }
*/

void sendmqtt() {

  JsonObject& json = jsonBuffer.createObject();
  json["temp_2m"] = temp;
  json["temp_0m"] = bodentemp;
  json["druck"] = druck;
  json["feuchte"] = feuchte;
  json["helligkeit"] = lux;
  json["regensensor"] = regensensorwert;
  json["uvsensor"] = uv;

  char output[1025];
  json.printTo(output, sizeof(output));

  client.publish("wetter_außen/get", output);
  client.publish("wetter_außen/update", "1");

  jsonBuffer.clear();
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT Verbindungsversuch...");
    if (client.connect("ESP8266_Wetterstation_außen", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      ledoff();
      client.subscribe("wetter_außen/debug/#");
      client.subscribe("wetter_außen/set/#");
    }
    else {
      Serial.print("fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println("Erneuter Versuch in 5 Sekunden");
      ledon();
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));

  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  String topicStr = topic;

  if (topicStr.startsWith("wetter_außen/set"))
  {
    readsensors();
    sendmqtt();
    return;
  }

  if (topicStr.startsWith("wetter_außen/debug/reset")) {
    Serial.println("MQTT RESET!");
    Serial.flush();
    ESP.restart();
  }

  if (topicStr.startsWith("wetter_außen/debug/ping")) {
    Serial.println("PING");
    client.publish("wetter_außen/debug/pong", message, false);
    return;
  }

  if (topicStr.startsWith("wetter_außen/debug/wifi/ping")) {
    //@author Marvin Roger - https://github.com/marvinroger/homie-esp8266/blob/ad876b2cd0aaddc7bc30f1c76bfc22cd815730d9/src/Homie/Utils/Helpers.cpp#L12
    int32_t rssi;
    rssi = WiFi.RSSI();
    uint8_t quality;
    if (rssi <= -100) {
      quality = 0;
    } else if (rssi >= -50) {
      quality = 100;
    } else {
      quality = 2 * (rssi + 100);
    }
    char qualityString[8];
    dtostrf(quality, 1, 1, qualityString);
    client.publish("wetter_außen/debug/wifi/pong", qualityString);
    return;
  }
}
