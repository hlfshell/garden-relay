/*
This is an MQTT relay board - the target board is the Adafruit Feaether Huzzah ESP8266

I am also adding the OLED featherwing as well for output.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WIFI_SSID "YOUR SSID"
#define WIFI_PASSWORD "YOUR PASSWORD"

#define MQTT_HOST "MQTT HOST IP"
#define MQTT_PORT 1883

#define LIGHT_SUB_TOPIC "garden/lights"
#define PUMP_SUB_TOPIC "garden/pump"

//Sometimes the relays are active low, somtimes active high. Mine's low. To make life easier, abstract it
#define ON 0
#define OFF 1

//Relay pins
#define LIGHT_RELAY_PIN 5
#define PUMP_RELAY_PIN 6

#define PUMP_DELAY 3000

//OLED PINS
#define BUTTON_A 0
#define BUTTON_B 16
#define BUTTON_C 2
#define LED      0

WiFiClient wifi;
PubSubClient client(wifi);

Adafruit_SSD1306 display = Adafruit_SSD1306();

void displayMessage(String msg){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(msg);
  display.println(WiFi.localIP());
  display.display();
}

void pumpCycle(){
  digitalWrite(PUMP_RELAY_PIN, ON);
  displayMessage("Pump it up!");
  delay(PUMP_DELAY);
  digitalWrite(PUMP_RELAY_PIN, OFF);
}

void turnOnLights(){
  digitalWrite(LIGHT_RELAY_PIN, ON);
  displayMessage("Let there be light");
  delay(2000);
}

void turnOffLights(){
  displayMessage("Goodnight, plants");
  display.display();
  delay(2000);
}

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

  displayMessage("Connected! My IP is:");
  delay(2000);
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

  if(strcmp(topic,LIGHT_SUB_TOPIC) == 0){
    // Handle a on or off payload messages
    if (strcmp(command, "off") == 0) {
      Serial.println("Light relay set to off");
      turnOffLights();
    } else if (strcmp(command, "on") == 0) {
      Serial.println("Relay set to on");
      turnOnLights();
    }
  } else if(strcmp(topic, PUMP_SUB_TOPIC) == 0){
    pumpCycle();
  }
}

void reconnect() {
  displayMessage("Connecting to MQTT Server");
  delay(2000);
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected!");
      client.subscribe(LIGHT_SUB_TOPIC);
      client.subscribe(PUMP_SUB_TOPIC);
    } else {
      Serial.print("Failed. State=");
      Serial.print(client.state());
      Serial.println(". Trying again in 5 seconds");
      displayMessage("Failed to connect to MQTT Server. Trying again in 5 seconds... ");
      // Wait 5 seconds before retrying
      delay(5000);
      displayMessage("Connecting to MQTT Server");
      delay(2000);
    }
  }
}

void setup() {
  //pin setup
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  digitalWrite(LIGHT_RELAY_PIN, OFF);

  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, OFF);

  //OLED pins
  display.display();
  delay(1000);
  display.clearDisplay();
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Connecting to WiFi...");
  display.display();
  
  //Serial setup
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

  //Handle button inputs. If it's pressed down, trigger the function!
  if (! digitalRead(BUTTON_A)) turnOnLights();
  if (! digitalRead(BUTTON_B)) turnOffLights();
  if (! digitalRead(BUTTON_C)) pumpCycle();

  displayMessage("<3   Going swell   <3");
}
