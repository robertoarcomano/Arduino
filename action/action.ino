#include <WiFi.h>
#include <PubSubClient.h>
#include "essid_secrets.h"

#define DELAY 60000   // 1 minute between 2 readings
#define MAX_COUNT 60  // 60 * 1 minute = 1h Reboot every hour
#define RESETPIN 16   // Reset PIN is connected to PIN 16
#define MQTT_NAME "Espertino Commands"
#define MQTT_SUBSCRIBING "esp/relais_cmd"
#define MQTT_PUBLISHING  "esp/relais_state"
#define RL_1 12   // Relay 1
#define RL_2 14   // Relay 2

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqttServer = "192.168.10.253";
const int mqttPort = 1883;
unsigned long count = 0;
struct relays_state_type {
  boolean RL1;
  boolean RL2;
};
relays_state_type relays_state;

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

void callback(char* topic, unsigned char* payload, unsigned int length);

void callback(char* topic, unsigned char* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println("");

       if (messageTemp == "0,0") relays_state.RL1 = LOW;
  else if (messageTemp == "0,1") relays_state.RL1 = HIGH;    
  else if (messageTemp == "1,0") relays_state.RL2 = LOW;
  else if (messageTemp == "1,1") relays_state.RL2 = HIGH;
  else if (messageTemp == "S,0") relays_state.RL1 = !relays_state.RL1;    
  else if (messageTemp == "S,1") relays_state.RL2 = !relays_state.RL2;    
  digitalWrite(RL_1, relays_state.RL1);
  digitalWrite(RL_2, relays_state.RL2);
  
  String out_message;
  if (relays_state.RL1 == LOW)
    out_message = "0";
  else
    out_message = "1";
  if (relays_state.RL2 == LOW)
    out_message = out_message + "0";
  else
    out_message = out_message + "1";
  client.publish(MQTT_PUBLISHING,out_message.c_str());
}

void initRelays() {
  pinMode(RL_1, OUTPUT);
  pinMode(RL_2, OUTPUT);
  relays_state.RL1 = LOW;
  relays_state.RL2 = LOW;
}

void setup() {
  // Configure MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  // Init console
  Serial.begin(9600);

  // Configure Sensor
  // Init Wifi
  initWifi();

  // Init Relays
  initRelays();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(MQTT_NAME)) {
      Serial.println("connected");
      // subscribe values
      client.subscribe(MQTT_SUBSCRIBING);  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);      
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  // Just wait
  client.loop();
  
//  // If we reached 1 hour => Reset
//  if (count > MAX_COUNT) {
//    pinMode(RESETPIN, OUTPUT);
//    digitalWrite(RESETPIN, LOW);
//  } else
//    count = count + 1;

//   Wait for DELAY (1 minute)
//  delay(DELAY);
//  resetWifi();
//  resetMqtt();
}
