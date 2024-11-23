# Projekt: Hőmérséklet és Páratartalom Kijelzés DHT11 Érzékelővel és Web Szerverrel

**Rendszer:**  
Te egy magyar nyelvű professzionális IOT asszisztens vagy. A szoftverek, amiket ismersz és használsz, ESP32 környezethez Arduino IDE. A kapott feladatokat Nodemcu ESP32s mikrokontrolleren valósítod meg. Az Arduino IDE Adafruit könyvtárakat tudod használni. A kijelző, amit tudsz használni, SSD1306, ami I2C-vel tud működni. Tudnod kell a portokat beállítani, és úgy csinálni, hogy azokon a lábakon működjön. Web szervert is tudsz felhúzni az eszközre.

**Felhasználó:**  
Írj egy programot, ami a kijelzőre kiírja a hőmérsékletet és a páratartalmat, amit a DHT11-ből olvas ki. A kijelző pinjei GPIO21 és GPIO22. A LED a GPIO5-ös pin, a DHT11 pedig a GPIO4-es. A program feladata még az, hogy egy web szerveren megjelenítse a kiolvasott értékeket, és amelyik GPIO-n lévő LED-et ki és be lehessen kapcsolni a web szerveren. A web szerver szöveg megjelenítésében ne használj ékezetes betűket.

**IOT Design Assistant:**  
Az alábbiakban bemutatok egy példakódot, amely az ESP32-re írt programot tartalmazza. A program kiolvassa a hőmérsékletet és páratartalmat a DHT11 szenzorból, majd megjeleníti ezeket az adatokat egy SSD1306 kijelzőn. Ezen kívül a program egy web szervert is indít, amely lehetővé teszi a GPIO 5-ös pinre kötött LED ki- és bekapcsolását. Figyeltem arra, hogy a kód ne használjon ékezetes betűket a webes felületen.

A szükséges könyvtárak:

- **Adafruit Unified Sensor Library**
- **Adafruit DHT Sensor Library**
- **Adafruit SSD1306 Library**

Győződj meg arról, hogy az összes fenti könyvtár telepítve van az Arduino IDE-ben.

### Kód:

```cpp
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