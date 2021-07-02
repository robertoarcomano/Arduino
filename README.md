# Sensors_ESPertino
Arduino code for ESPertino: sensors values retrieving and publishing using MQTT

## Overview
Espertino card collects temperatures and humidity and send them to the Raspberry Pi 2 based MQTT broker.

To see data we need to be subscribing to the sensors at the same time (sensors data are never stored)
<img src=schema.jpg>

## Hardware
#### ESPertino-32 card with ESP32-WROOM and Temperature & Humidity sensors
<img src=espertino.jpg>

#### Raspberry Pi 2 with Ubuntu 20.04
<img src=raspberry2.jpg>

## Software
#### Arduino code
```
#include "DHTesp.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "essid_secrets.h"

DHTesp dht;  
WiFiClient espClient;
PubSubClient client(espClient);
struct sensorDataType {
String embeddedTemperature;
String temperature;
String humidity;
};

#define DHTPIN 4
const char* mqttServer = "192.168.10.253";
const int mqttPort = 1883;

void initWifi() {
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());  
}

void resetWifi() {
WiFi.disconnect();
initWifi();
}

void initMqtt() {
client.setServer(mqttServer, mqttPort);
while (!client.connected()) {
Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
      
    }
}
}

void resetMqtt() {
client.disconnect();
initMqtt();
}

void setup() {
Serial.begin(9600);
analogReadResolution(12);
analogSetAttenuation(ADC_11db);
TaskHandle_t tempTaskHandle = NULL;
dht.setup(DHTPIN, DHTesp::DHT11);
// Init wifi
initWifi();
// Init mqtt
initMqtt();
}

sensorDataType getData() {
sensorDataType sensorData;

int digital=analogRead(36);  
float volt=3.3*digital/4096;
float embeddedTemperature=100*volt-40;
TempAndHumidity lastValues = dht.getTempAndHumidity();

sensorData.embeddedTemperature = String(embeddedTemperature,0);
sensorData.temperature = String(lastValues.temperature,0);
sensorData.humidity = String(lastValues.humidity,0);

return sensorData;   
};

void loop() {
sensorDataType sensorData = getData();
Serial.print("Embedded Temperature: " + sensorData.embeddedTemperature);  
Serial.print(" Temperature: " + sensorData.temperature);
Serial.println(" Humidity: " + sensorData.humidity);

client.publish("esp/embeddedTemperature", sensorData.embeddedTemperature.c_str());
client.publish("esp/temperature", sensorData.temperature.c_str());
client.publish("esp/humidity", sensorData.humidity.c_str());
delay(10000);
resetWifi();
resetMqtt();
}
```

#### SW installed on Raspberry
sudo apt install mosquitto

#### Test subscribe
```
berto@clevo:~$ mosquitto_sub -h 192.168.10.253 -t esp/embeddedTemperature -t esp/temperature -t esp/humidity -v
esp/embeddedTemperature 27
esp/temperature 27
esp/humidity 59
```