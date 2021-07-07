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

#define DELAY 60000   // 1 minute between 2 readings
#define DHTPIN 4      // Pin 4 where DHT sensor is connected
#define MQTT_NAME "Espertino Sensors"

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

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT Broker...");
    if (client.connect(MQTT_NAME)) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);      
    }
  }
}

sensorDataType getData() {
  sensorDataType sensorData;

  // Read data
  int digital=analogRead(36);  
  float volt=3.3*digital/4096;
  float embeddedTemperature=100*volt-40;
  TempAndHumidity lastValues = dht.getTempAndHumidity();

  // Build the record
  sensorData.embeddedTemperature = String(embeddedTemperature,0);
  sensorData.temperature = String(lastValues.temperature,0);
  sensorData.humidity = String(lastValues.humidity,0);
  
  return sensorData;   
};

void setup() {
  // Configure Mqtt
  client.setServer(mqttServer, mqttPort);
  
  // Init console
  Serial.begin(9600);

  // Configure Sensor
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); 
  dht.setup(DHTPIN, DHTesp::DHT11);
  
  // Init Wifi
  initWifi();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  sensorDataType sensorData = getData();
  
  // Show values
  Serial.print("Embedded Temperature: " + sensorData.embeddedTemperature);  
  Serial.print(" Temperature: " + sensorData.temperature);
  Serial.println(" Humidity: " + sensorData.humidity);

  // Publish values
  client.publish("esp/embeddedTemperature", sensorData.embeddedTemperature.c_str());
  client.publish("esp/temperature", sensorData.temperature.c_str());
  client.publish("esp/humidity", sensorData.humidity.c_str());

  // Wait for DELAY (1 minute)
  delay(DELAY);
}
