#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- WiFi credentials (change these) ---
const char* ssid     = "YourWiFiName";
const char* password = "YourWiFiPassword";

// --- Bitcoin price API ---
const char* apiUrl = "https://api.coindesk.com/v1/bpi/currentprice/BTC.json";

// --- OLED display setup (SSD1306 128x64 I2C) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  // Start OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display not found");
    while(true); // stop if no display
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Starting...");
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  display.println("Connecting WiFi");
  display.display();

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.display();
  delay(1000);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiUrl);

    int code = http.GET();
    if(code == 200) {
      String data = http.getString();
      Serial.println(data);

      // JSON parsing
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, data);

      String price = doc["bpi"]["USD"]["rate"].as<String>();

      // show on screen
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(1);
      display.println("Bitcoin Price:");
      display.setTextSize(2);
      display.println("$" + price);
      display.display();
    } else {
      Serial.print("HTTP Error: ");
      Serial.println(code);
    }
    http.end();
  }

  delay(60000); // refresh every 60s
}
