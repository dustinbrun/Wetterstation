#include <Wire.h>
#include "Adafruit_VEML6070.h"

Adafruit_VEML6070 uv = Adafruit_VEML6070();

void setup() {
  Serial.begin(9600);
  Serial.println("VEML6070 Test");
  uv.begin(VEML6070_2_T);  // pass in the integration time constant
  /*
  possible integration times: -> adapt the convert_to_risk_level-function accordingly!
    VEML6070_HALF_T ~62.5ms
    VEML6070_1_T ~125ms
    VEML6070_2_T ~250ms
    VEML6070_4_T ~500ms

   */
}


//Source: https://github.com/adafruit/Adafruit_CircuitPython_VEML6070/blob/main/adafruit_veml6070.py
String convert_to_risk_level(int reading) 
{
  int integration_time = 4;   //available for Integration-Time 1, 2, 4
                              // MUST be adjusted according to the set integration time
  reading = reading / integration_time;

  String risk_level;

  if(reading <= 560)
      risk_level = "LOW (UV 0-2)";
  else if(reading > 560 && reading <= 1120)
      risk_level = "Moderate (UV 3-5)";
  else if(reading > 1120 && reading <= 1494)
      risk_level = "High (UV 6-7)";
  else if(reading > 1494 && reading <= 2054)
      risk_level = "Very High (UV 8-10)";
  else if(reading > 2054 && reading <= 9999)
      risk_level = "Extreme (UV >10)";
  else
      risk_level = "ERROR";

  return risk_level;
}

/*
// German equivalent
String convert_to_risk_level(int reading) 
{
  int integration_time = 4;   //available for Integration-Time 1, 2, 4
                              // MUST be adjusted according to the set integration time
  reading = reading / integration_time;

  String risk_level;

  if(reading <= 560)
      risk_level = "Niedrig (UV 0-2)";
  else if(reading > 560 && reading <= 1120)
      risk_level = "Mittel (UV 3-5)";
  else if(reading > 1120 && reading <= 1494)
      risk_level = "Hoch (UV 6-7)";
  else if(reading > 1494 && reading <= 2054)
      risk_level = "Sehr Hoch (UV 8-10)";
  else if(reading > 2054 && reading <= 9999)
      risk_level = "Extrem (UV >10)";
  else
      risk_level = "ERROR";

  return risk_level;
}
*/

void loop() {
  int reading = uv.readUV();
  
  Serial.print("UV light level: "); 
  Serial.print(reading);
  Serial.print("  Risk level is: ");
  Serial.println(convert_to_risk_level(reading));
  
  delay(1000);
}
