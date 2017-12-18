#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid     = "net-iot";
const char* password = "seg2357iotmais";

const char* host = "iot.franciscocalaca.com";
const int port = 1883;

WiFiClient client;
PubSubClient MQTT(client);

DHT sensor(D3, DHT11);

unsigned long lastTime = 0;

void setup() {
  pinMode(D5, OUTPUT);
  
  sensor.begin();
  Serial.begin(9600);
  Serial.println("Humidity and temperature\n\n");

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  MQTT.setServer(host, port);
  MQTT.setCallback(callback); 
  
  delay(700);
}

void callback(char* topic, byte* payload, unsigned int length){
  DynamicJsonBuffer jsonBuffer;
  String json = (char*)payload;
  JsonObject& rootRead = jsonBuffer.parseObject(json);
  long s = rootRead["status"];
  Serial.println(json);
  if(s == 1){
    digitalWrite(D5, HIGH);
    Serial.println("...ligar");
  }else{
    digitalWrite(D5, LOW);
    Serial.println("...desligar");
  }
}

void reconnect() {
  while (!MQTT.connected()) {
    if (MQTT.connect("ESP8266-Francisco-Leandro-Aula")) {
      MQTT.subscribe("leandro/acionamento");
    } else {
      Serial.print(".");
      delay(3000);
    }
  }
}

void loop() {
    reconnect();
    MQTT.loop();

    unsigned long time = millis() - lastTime;

    if(time > 2000){
      float h = sensor.readHumidity();
      float t = sensor.readTemperature();     

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["temp"] = t;
      root["humid"] = h;
      String msg;
      root.printTo(msg);    

      Serial.println(msg);    
  
      MQTT.publish("leandro/humid_temp", msg.c_str());
      lastTime = millis();
    }
    
}
