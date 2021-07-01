
#include "DHTesp.h"
#define DHTPIN 4
DHTesp dht;  

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); 
  TaskHandle_t tempTaskHandle = NULL;
  dht.setup(DHTPIN, DHTesp::DHT11);
}
void loop() {
  int digital=analogRead(36);  
  float volt=3.3*digital/4096;
  float embedded_temperature=100*volt-40;
  Serial.print("Embedded Temperature: " + String(embedded_temperature));  
  TempAndHumidity lastValues = dht.getTempAndHumidity();
  Serial.print(" Temperature: " + String(lastValues.temperature,0));
  Serial.println(" Humidity: " + String(lastValues.humidity,0));
  delay(2000);
}
