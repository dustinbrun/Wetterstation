/*
   WARNING:
   If you are not receiving the Json Messages you probably have to increase the
   MQTT_MAX_PACKET_SIZE in the PubSubClient.h file to at least 1024
   (Located on Windows at C:\Users\*YOUR_USERNAME*\Documents\Arduino\libraries\pubsubclient-master\src\PubSubClient.h)
*/


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> //https://github.com/adafruit/Adafruit_BME280_Library
#include <BH1750.h> //https://github.com/claws/BH1750
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <OneWire.h>
#include <DallasTemperature.h> //https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include "Adafruit_VEML6070.h"  // https://github.com/adafruit/Adafruit_VEML6070



  const char *ssid = "----WIFI_SSID_HERE----";
  const char *password = "----WIFI_PASSWORD_HERE----";

  //MQTT Server
  const char *mqtt_server = "----MQTT_SERVER_IP_HERE----";
  const char *mqtt_user = "----MQTT_USERNAME_HERE----";
  const char *mqtt_pass = "----MQTT_PASSWORD_HERE----";



uint8_t ds18b20_2m[8] = {0x28, 0xB4, 0x2A, 0xA7, 0x4D, 0x20, 0x01, 0x11}; //You need to adapt these addresses
uint8_t ds18b20_0m[8] = {0x28, 0xA9, 0x2F, 0x1C, 0x0B, 0x00, 0x00, 0xA8};

const int hoehe = 437; //Aufstellhoehe in m

//Pins
const int dataled = D8;
const int onewire = D3; //DS18B20
const int regensensorpin = A0;
const int regensensorpower = D5;

#define TEMPERATURE_PRECISION 11 //9-12 Bit for DS18B20 
/* 9-12 Bit
   9 - 0.5 Steps
   10 - 0.25 Steps
   11 - 0.125 Steps <- Best for this Case
   12 - 0.0625 Steps
*/

Adafruit_BME280 bme;
Adafruit_VEML6070 uvMeter = Adafruit_VEML6070();
BH1750 lightMeter;
WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(onewire);
DallasTemperature ds18b20(&oneWire);
StaticJsonBuffer<1024> jsonBuffer;


//Variablen
float temp = 0;
float bodentemp = 0;
float druck_sensorwert = 0;
float druck = 0;
float feuchte = 0;
float lux = 0;
int uv = 0;
float regensensorwert = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("MQTT Wetterstation");

  pinMode(dataled, OUTPUT);
  pinMode(regensensorpin, INPUT_PULLUP);
  pinMode(regensensorpower, OUTPUT);
  digitalWrite(regensensorpower, LOW);

  ledon();
  delay(500);

  ds18b20.begin();
  ds18b20.setResolution(ds18b20_2m, TEMPERATURE_PRECISION);
  ds18b20.setResolution(ds18b20_0m, TEMPERATURE_PRECISION);

  //        SDA, SCL
  Wire.begin(D2, D1);

  if (! bme.begin(0x76))
  {
    Serial.println("Could not find BME280 sensor");
    //while (1) {}
  }
  else
  {
    Serial.println("BME280 initialized.");
  }


  if (! lightMeter.begin())
  {
    Serial.println("Could not find BH1750 sensor");
    //while (1) {}
  }
  else
  {
    Serial.println("BH1750 initialized.");
  }


  uvMeter.begin(VEML6070_4_T);
  /*
      possible integration times: -> adapt the convert_to_risk_level-function accordingly!
      VEML6070_HALF_T ~62.5ms
      VEML6070_1_T ~125ms
      VEML6070_2_T ~250ms
      VEML6070_4_T ~500ms

  */
  uv = uvMeter.readUV();
  if (uv == -1 || uv == 65535)
  {
    Serial.println("Could not find VEML6070 sensor");
    //while (1) {}
  }
  else
  {
    Serial.println("VEML6070 initialized.");
  }



  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  ledblink();
  Serial.println("-----Initialisierung beendet-----");
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();

  delay(100);
}
