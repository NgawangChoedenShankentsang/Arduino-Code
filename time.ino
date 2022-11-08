#include <Wire.h>   // for I2C with RTC module
#include "RTClib.h" // Ausgabe Zeit 
#include "DHT.h"
#include <PubSubClient.h>
#define DHTPIN 4
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
// WIFI connection
const char* ssid     = "xxxxxxx";
const char* password = "xxxxxxx";
const char *mqttServer = "xxxxxxx"; //Use your local IP not 127.0.0.1 or localhost
const int mqttPort = 1883;

  char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
  char monthOfTheYear[12][12] =  {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", 
                                "September", "Oktober", "November", "Dezember"};

WiFiClient espClient;
PubSubClient client(espClient);   

  void setup() { 
    Serial.begin(9600);
    delay(500); 
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (! rtc.begin()) { 
      Serial.println("kein RTC IC gefunden"); 
      while (1); 
    } rtc.adjust(DateTime(__DATE__, __TIME__)); 

    Serial.println ("Setup erledigt"); 
    Serial.println(F("DHTxx test!"));

      //  client.setCallback(callback);
    dht.begin();
  } 
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Ready");

  client.setServer(mqttServer, mqttPort);
//  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    WiFi.mode(WIFI_STA);
    if (client.connect("ESP8266Client" )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  void loop() { 

    DateTime now= rtc.now();   
    Serial.print("WOCHE: "); 
    Serial.println(daysOfTheWeek[now.dayOfTheWeek()]);  
    Serial.print("STUNDEN: ");    
    Serial.print(now.hour(), DEC); 
    Serial.print (":"); 
    Serial.print(now.minute(), DEC); 
    Serial.print (":"); 
    Serial.println(now.second(), DEC); 
    Serial.print ("DATUM: "); 
    Serial.print(now.day(), DEC); 
    Serial.print (":");
    Serial.print(monthOfTheYear[now.month()]);  
    Serial.print (":");
    Serial.println(now.year(), DEC);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%"));
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.println(F("°F"));
    Serial.print(F("Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    Serial.println("--------------------------");
    Serial.println("");
    delay(5000); 
    client.publish("esp/welcome", unitsToSend); 
  } 