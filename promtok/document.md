# Projekt Dokumentáció

Az alábbi dokumentumban bemutatom az ESP32 mikrokontrollerre írt kódot, amely a DHT11 szenzorról olvasott hőmérséklet- és páratartalom-értékeket megjeleníti egy SSD1306 kijelzőn. Továbbá, a program egy web szervert is futtat, amely lehetővé teszi egy LED ki- és bekapcsolását.

## Résztvevők

**System:** Te egy magyar nyelvű professzionális IoT asszisztens vagy, aki ESP32 környezetben Arduino IDE használatával tud megfelelő kódot generálni.

**User:** Írj egy programot, amely a kijelzőre kiírja a hőmérsékletet és a páratartalmat a DHT11 szenzorból, és egy web szerveren is megjeleníti az adatokat. A programnak lehetővé kell tennie a LED ki- és bekapcsolását a web szerveren keresztül.

**Assistant:** Az alábbiakban bemutatok egy példakódot a kért funkciókkal. Az adatokat a DHT11 szenzor olvassa ki, a kijelzés az SSD1306 kijelzőn történik, és egy web szerver teszi lehetővé a LED vezérlését.

## Kód

```c
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi konfiguráció
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// DHT11 konfiguráció
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// OLED kijelző konfiguráció
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LED pin
const int ledPin = 5;
bool ledState = false;

// WiFi szerver
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // OLED kijelző inicializálása
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // LED pin beállítása
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Csatlakozás WiFi-re
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Web szerver indítása
  server.begin();
}

void loop() {
  // DHT11 kiolvasás
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Adatok megjelenítése a kijelzőn
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.print(temperature);
  display.println(" C");
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");
  display.display();

  // Web szerver kezelése
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    Serial.print("Request: ");
    Serial.println(request);
    client.flush();

    if (request.indexOf("/LED_ON") != -1) {
      digitalWrite(ledPin, HIGH);
      ledState = true;
    }
    if (request.indexOf("/LED_OFF") != -1) {
      digitalWrite(ledPin, LOW);
      ledState = false;
    }

    // HTTP válasz küldése
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta charset=\"utf-8\"><title>ESP32 Web Server</title></head>");
    client.println("<body><h1>ESP32 Web Server</h1>");
    client.println("<p>Temperature: " + String(temperature) + " C</p>");
    client.println("<p>Humidity: " + String(humidity) + " %</p>");
    client.println("<p>LED State: " + String(ledState ? "ON" : "OFF") + "</p>");
    client.println("<p><a href=\"/LED_ON\">Turn LED ON</a></p>");
    client.println("<p><a href=\"/LED_OFF\">Turn LED OFF</a></p>");
    client.println("</body></html>");

    client.println();
    client.stop();
  }

  delay(2000); // 2 másodperces késleltetés
}