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
<a href=sensors/sensors.ino>sensors</a>

<a href=action/action.ino>action</a>