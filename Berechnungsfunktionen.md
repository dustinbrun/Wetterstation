# Wetterstation Berechnungen
Hier sind einige nützliche Berechnungsfunktionen und Formeln zu finden, welch ich in meiner Wetterstation verwende. Die Berechnungsfunktionen sind in Javascript zur Verwendung in Node-Red geschieben, können jedoch einfach auch für andere Plattformen angepasst werden (z.B. Arduino).

Die Quellen sind jeweils verlinkt.

# Taupunkt berechnen
Quelle: https://myscope.net/taupunkttemperatur/

Die Berechnung des Taupunktes erfolgt aus den Messwerten Temperatur (°C) und Luftfeuchtigkeit (%).

```
var calcdewpoint = function(celsius, humidity) {

  var a, b;
  if (celsius >= 0) {
    a = 7.5;
    b = 237.3;
  } else if (celsius < 0) {
    a = 7.6;
    b = 240.7;
  }

  // Sättigungsdampfdruck (hPa)
  var sdd = 6.1078 * Math.pow(10, (a * celsius) / (b + celsius));

  // Dampfdruck (hPa)
  var dd = sdd * (humidity / 100);

  // v-Parameter
  var v = Math.log10(dd / 6.1078);

  // Taupunkttemperatur (°C)
  var td = (b * v) / (a - v);
  
  //Runden 1 Nachkommastelle
  td =  Math.round(td * 10) / 10;
  
  return td;
}

msg.payload = calcdewpoint(12.45, 80);

return msg;
```
# Gefühlte Temperatur berechnen
Die Gefühlte Temperatur setzt sich aus zwei Berechnungsarten zusammen. Windchill (Temperatur kleiner als 10°C und Windgeschwindigkeit größer als 4.8km/h) und Hitzeindex (Temperatur größer als 26.7°C und Relative Luftfeuchte größer als 40%).

```

//Quelle:   https://myscope.net/windchill-gefuehlte-temperatur-berechnen/
//          https://de.wikipedia.org/wiki/Windchill
var calcwindchill = function(celsius, windspeed) {
  var windchill = 13.12 + 0.6215 * celsius - 11.37 * Math.pow(windspeed, 0.16) + 0.3965 * celsius * Math.pow(windspeed, 0.16);
  return windchill;
}

//Quelle:   https://myscope.net/hitzeindex-gefuehle-temperatur/
//          https://de.wikipedia.org/wiki/Hitzeindex
var calcheatindex = function(celsius, humidity) {
  return (-8.784695 + 1.61139411 * celsius + 2.338549 * humidity - 0.14611605 * celsius * humidity - 0.012308094 * celsius * celsius - 0.016424828 * humidity * humidity + 0.002211732 * celsius * celsius * humidity + 0.00072546 * celsius * humidity * humidity - 0.000003582 * celsius * celsius * humidity * humidity);
}

var Temperatur_2m = parseFloat(12.34);
var Luftfeuchte_rel = parseFloat(80);
var windkmh = parseFloat(2.45);

if (Temperatur_2m <= 10 && windkmh >= 4.8) 
{
   msg.payload = calcwindchill(Temperatur_2m, windkmh);
   msg.topic = "123";
}
else if (Temperatur_2m >= 26.7 && Luftfeuchte_rel >= 40) 
{
   msg.payload = calcheatindex(Temperatur_2m, Luftfeuchte_rel);
   msg.topic = "246";
}
else //Keine der beiden Formeln ist definiert
{
   //msg.payload = -1;
   msg.payload = Temperatur_2m;
}
 
//Runden 1 Nachkommastelle
msg.payload =  Math.round(msg.payload * 10) / 10;

return msg;
```

# Absolute Luftfeuchtigkeit
Eine Umrechnung von Relativer (in %) in Absolute Luftfeuchtigkeit (in g/m^3).

```
// Relative to absolute humidity
// Based on https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
var absoluteHumidity = function(temperature, humidity) {
  return (13.2471*Math.pow(2.7182818,17.67*temperature/(temperature+243.5))*humidity/(273.15+temperature));
}

var Temperatur_2m = parseFloat(12.34);
var Luftfeuchte_rel = parseFloat(45);

msg.payload = absoluteHumidity(Temperatur_2m, Luftfeuchte_rel);

//Runden 1 Nachkommastelle
msg.payload =  Math.round(msg.payload * 10) / 10;

return msg;
```

# Windgeschwindigkeit Meter pro Sekunde in Kilometer pro Stunde
Eine Umrechnung der Windgeschwindigkeit vom m/s in km/h sowie runden des Ergebnisses auf eine Nachkommastellen.
```
msg.payload =  Math.round(msg.payload * 3.6 * 10) / 10;

return msg;
``` 

# Windgeschwindigkeit von Kilometer pro Stunde in Windstärke/Beaufortskala umrechnen
Referenz: https://wetterkanal.kachelmannwetter.com/woher-kommt-die-beaufortskala/
Die Berechnungen beziehen sich auf die Einheit m/s. 

```
var windmax = parseFloat(msg.payload) / 3.6;

var windstarkewort = {};
windstarkewort.topic = "Windstärke_Wort";

var windstarkebft = {};
windstarkebft.topic = "Windstärke_bft";

if (windmax < 0.3) {
    windstarkebft.payload = 0;
    windstarkewort.payload = "Windstille";
  }
  else if (windmax >= 0.3 && windmax < 1.6) {
    windstarkebft.payload = 1;
    windstarkewort.payload = "leiser Zug";
  }
  else if (windmax >= 1.6 && windmax < 3.4 ) {
    windstarkebft.payload = 2;
    windstarkewort.payload = "leichte Brise";
  }
  else if (windmax >= 3.4 && windmax < 5.5) {
    windstarkebft.payload = 3;
    windstarkewort.payload = "schwache Brise";
  }
  else if (windmax >= 5.5 && windmax < 8.0 ) {
    windstarkebft.payload = 4;
    windstarkewort.payload = "mäßige Brise";
  }
  else if (windmax >= 8.0 && windmax < 10.8 ) {
    windstarkebft.payload = 5;
    windstarkewort.payload = "frische Brise";
  }
  else if (windmax >= 10.8 && windmax < 13.9 ) {
    windstarkebft.payload = 6;
    windstarkewort.payload = "starker Wind";
  }
  else if (windmax >= 13.9 && windmax < 17.2 ) {
    windstarkebft.payload = 7;
    windstarkewort.payload = "steifer Wind";
  }
  else if (windmax >= 17.2 && windmax < 20.8 ) {
    windstarkebft.payload = 8;
    windstarkewort.payload = "stürmischer Wind";
  }
  else if (windmax >= 20.8 && windmax < 24.5 ) {
    windstarkebft.payload = 9;
    windstarkewort.payload = "Sturm";
  }
  else if (windmax >= 24.5 && windmax < 28.5 ) {
    windstarkebft.payload = 10;
    windstarkewort.payload = "schwerer Sturm";
  }
  else if (windmax >= 28.5 && windmax < 32.7 ) {
    windstarkebft.payload = 11;
    windstarkewort.payload = "orkanartiger Sturm";
  }
  else if (windmax >= 32.7) {
    windstarkebft.payload = 12;
    windstarkewort.payload = "Sturm";
  }


return [windstarkewort, windstarkebft];

```

# Windrichtung "Grad in Wort"
Eine Umrechnung einer Windrichtung in Grad (0° entpsricht Nord) in Himmelsrichtungen (z.B. NO, S usw.).

```
var windrichtung = parseInt(msg.payload);

var windrichtungwort = {};
windrichtungwort.topic = "Windrichtung_Wort";


if (windrichtung >= 348 &&  windrichtung < 12) {
    windrichtungwort.payload = "N";
  }
  else if (windrichtung >= 12 && windrichtung < 35) {
    windrichtungwort.payload = "NNO";
  }
  else if (windrichtung >= 35 && windrichtung < 57) {
    windrichtungwort.payload = "NO";
  }
  else if (windrichtung >= 57 && windrichtung < 80) {
    windrichtungwort.payload = "NOO";
  }
  else if (windrichtung >= 80 && windrichtung < 102) {
    windrichtungwort.payload = "O";
  }
  else if (windrichtung >= 102 && windrichtung < 125) {
    windrichtungwort.payload = "SOO";
  }
  else if (windrichtung >= 125 && windrichtung < 147) {
    windrichtungwort.payload = "SO";
  }
  else if (windrichtung >= 147 && windrichtung < 170) {
    windrichtungwort.payload = "SSO";
  }
  else if (windrichtung >= 170 && windrichtung < 192) {
    windrichtungwort.payload = "S";
  }
  else if (windrichtung >= 192 && windrichtung < 215) {
    windrichtungwort.payload = "SSW";
  }
  else if (windrichtung >= 215 && windrichtung < 237) {
    windrichtungwort.payload = "SW";
  }
  else if (windrichtung >= 237 && windrichtung < 260) {
    windrichtungwort.payload = "SWW";
  }
  else if (windrichtung >= 260 && windrichtung < 282) {
    windrichtungwort.payload = "W";
  }
  else if (windrichtung >= 282 && windrichtung < 305) {
    windrichtungwort.payload = "NWW";
  }
  else if (windrichtung >= 305 && windrichtung < 327) {
    windrichtungwort.payload = "NW";
  }
  else if (windrichtung >= 327 && windrichtung < 348) {
    windrichtungwort.payload = "NNW";
  }
  else {
    windrichtungwort.payload = "???";
  }

return windrichtungwort;
```

<br><br>
<p xmlns:dct="http://purl.org/dc/terms/" xmlns:cc="http://creativecommons.org/ns#" class="license-text">This work by <span property="cc:attributionName">Dustin Brunner</span> is licensed under <a rel="license" href="https://creativecommons.org/licenses/by/4.0">CC BY 4.0<img style="height:15px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1" /><img style="height:15px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1" /></a></p>

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons Lizenzvertrag" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />Dieses Werk von <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Dustin Brunner</span> ist lizenziert unter einer <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Namensnennung 4.0 International Lizenz</a>.
