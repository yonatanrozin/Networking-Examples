/*
Arduino OSC UDP receiver example
Yonatan Rozin

- Enter WiFi network name and password (case sensitive)
- Enter a port #
- Enter an OSC address
- Enter neopixel's data pin number

- Upload sketch, open serial monitor
- Arduino will print its IP address when connected to the WiFi network
- Use this IP address and port to send OSC messages from ZigSim to this Arduino!
- Use "compass" and "gravity" sensors
- Make sure the sending device is on the same WiFi network as this one.
*/

#include <WiFi.h> //use <WiFi.h> or <WiFiNINA.h>, depending on your Arduino model
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Adafruit_NeoPixel.h>

const char WIFI_SSID[] = ""; //WiFi network name
const char WIFI_PASS[] = ""; //WiFi network password

const int port = 4242; //choose a port # (2000+ recommended)

const char OSC_address[] = "/ZIGSIM/*/*"; //choose an OSC address

const int  numLeds = 300;  
const byte dataPin = 28;

WiFiUDP Udp;
Adafruit_NeoPixel leds(numLeds, dataPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  delay(4000);

  connectWiFi();

  Udp.begin(port);

  leds.begin();
  leds.clear();
  leds.show();
}

void loop() {

  connectWiFi();

  int size = Udp.parsePacket();

  if (size > 0) {
    byte buffer[256];
    int len = Udp.read(buffer, min(size, 256));

    if (buffer[0] == '#') {
      OSCBundle bundle;
      for (int i = 0; i < len; i++) bundle.fill(buffer[i]);
      for (int i = 0; i < bundle.size(); i++) {
        routeMessage(*bundle.getOSCMessage(i));
      }
    }
    else {
      OSCMessage msg;
      for (int i = 0; i < len; i++) msg.fill(buffer[i]);
      routeMessage(msg);
    }
  }
}

float fade = 0;

void routeMessage(OSCMessage &msg) {
  if (msg.fullMatch("/ZIGSIM/*/compass")) {
    const float val = msg.getFloat(0); //get float value of 1st argument
    const int center = val / 360.0 * (float)numLeds;
    leds.clear();
    for (int i = -5; i < 5; i++) {
      leds.setPixelColor((center + i) % numLeds, 255 - (255 * fade), 0, 255 * fade);
    }
    leds.show();
  }
  else if (msg.fullMatch("/ZIGSIM/*/gravity")) {
    const float tilt = msg.getFloat(1); //(-1 to 1) float value of 2nd argument
    fade = (tilt + 1.0) / 2.0; //0 to 1
  }
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  digitalWrite(LED_BUILTIN, LOW);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
}