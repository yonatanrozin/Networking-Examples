/*
  Simple Arduino MQTT Sender Example
  By Yonatan Rozin

  - Enter WiFi network name and password (case sensitive)
  - Enter MQTT broker address and port
  - Enter a unique device ID for this Arduino
  - Use a different MQTT broker hostname,  port and credentials if needed

  This sketch sends an incrementing number every second to the MQTT broker
  using the topic "/YR_Demo/Arduino/number"
  Subscribe to this topic from another MQTT client to receive the numbers
*/

#include <WiFi.h> //or <WiFiNina.h>, depending on your board
#include <ArduinoMqttClient.h>

const char WIFI_SSID[] = ""; //WiFi network name
const char WIFI_PASS[] = ""; //WiFi network password

const char broker[] = "public.cloud.shiftr.io";
int        port     = 1883;
const char device_id[] = ""; //any unique identifier for the device
const char MQTT_USER[] = "public"; //MQTT broker username (leave empty if not required)
const char MQTT_PASS[] = "public"; //MQTT broker password (leave empty if not required)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

int count = 0;

void setup() {
  
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi(); 

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {

  mqttClient.poll(); //check for incoming message and keep the connection alive

  unsigned long currentMillis = millis();
  
  mqttClient.beginMessage("/YR_Demo/Arduino/number");
  mqttClient.print(count);
  mqttClient.endMessage();
  count++;

  delay(1000);

}

void connectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    WiFi.noLowPowerMode(); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      Serial.print(".");
    }
    Serial.print("\nIP: ");
    Serial.println(WiFi.localIP());
  }

  if (!mqttClient.connected()) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);
    mqttClient.setId(device_id);

    mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);

    while (!mqttClient.connect(broker, port)) {
      Serial.print(".");
      delay(2000);
    }
  }

  digitalWrite(LED_BUILTIN, HIGH);
}
