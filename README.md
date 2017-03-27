# garden-relay
Arduino/ESP8266/Adafruit Feather Huzzah relay controller via MQTT and WiFi.

This is a quick sketch designed to control my fiance's garden lights for her indoor garden. The Feather board is connected with a single relay controlling the lights on pin 15 (easily configurable), connects to my house's local MQTT network (I'm using [Mosca](https://github.com/mcollina/mosca), and, upon receiving an "on"/"off" command to its subscribed topic, switches the relay on/off.

It also uses an Adafruit OLED Featherwing.

To be paired with [MQTT Scheduler](https://github.com/hlfshell/mqtt-scheduler).
