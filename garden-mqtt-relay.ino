/*
This is an MQTT relay board - the target board was the esp8266 nodemcu board
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID "Your SSID"
#define WIFI_PASSWORD "Your super secret secure password"

#define MQTT_HOST "Host address or IP"
#define MQTT_PORT 1883

#define SUB_TOPIC "topic/to/subscribe/to"

//Sometimes the relays are active low, somtimes active high. Mine's low. To make life easier, abstract it
#define ON 0
#define OFF 1

//Relay pin
#define RELAY_PIN 15

WiFiClient wifi;
PubSubClient client(wifi);

void connect_to_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    for(int i = 0; i<500; i++){
      delay(1);
    }
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi is connected!");
  Serial.print("My IP address is ");
  Serial.println(WiFi.localIP());
}

void handleMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print(">> [");
  Serial.print(topic);
  Serial.print("] - ");
  char command[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    command[i] = (char) payload[i];
  }
  Serial.println();

  // Handle a on or off payload messages
  if (strcmp(command, "off") == 0) {
    Serial.println("Relay set to off");
    digitalWrite(RELAY_PIN, OFF);
  } else if (strcmp(command, "on") == 0) {
    Serial.println("Relay set to on");
    digitalWrite(RELAY_PIN, ON);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected!");
      client.subscribe(SUB_TOPIC);
    } else {
      Serial.print("Failed. State=");
      Serial.print(client.state());
      Serial.println(". Trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, OFF);

  Serial.begin(9600);
  connect_to_wifi(); 
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(handleMessage);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
