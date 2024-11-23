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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// WiFi beállítások
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// SSD1306 OLED kijelző méret
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED reset pin (nem használjuk)
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT szenzor
#define DHTPIN 4 // Állítsd be megfelelő GPIO-ra
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED GPIO port
#define LED_PIN 5

// Webszerver inicializálása
WiFiServer server(80);

void setup() {
  // Soros port inicializálása
  Serial.begin(115200);

  // LED kimeneti mód beállítása
  pinMode(LED_PIN, OUTPUT);

  // WiFi csatlakozás
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Szerver indítása
  server.begin();

  // I2C busz inicializálása GPIO21 és GPIO22 lábakon
  Wire.begin(21, 22);

  // OLED kijelző inicializálása
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 kijelző nem található"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  // DHT szenzor inicializálása
  dht.begin();
}

void loop() {
  // Hőmérséklet és páratartalom olvasása
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Ellenőrizze, ha az olvasás sikertelen
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Hiba a DHT szenzor olvasásakor"));
    return;
  }
  
  // Adatok megjelenítése a kijelzőn
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");

  display.setCursor(0, 40);
  display.print("Humidity: ");
  display.print(h);
  display.print(" %");
  
  display.display();
  
  // Várja a kliens csatlakozást
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Weboldalon megjelenített hőmérséklet és páratartalom, LED vezérlés
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><title>ESP32 LED vezerles</title></head><body>");
            client.print("Homerseklet: ");
            client.print(t);
            client.println(" &#8451;<br>");
            client.print("Paratartalom: ");
            client.print(h);
            client.println(" %<br><br>");
            // LED kapcsolását lehetővé tevő linkek
            client.println("<a href=\"/LED=ON\">LED ON</a><br>");
            client.println("<a href=\"/LED=OFF\">LED OFF</a><br>");
            client.println("</body></html>");

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Link elemzés a LED kapcsolása érdekében
        if (currentLine.endsWith("GET /LED=ON")) {
          digitalWrite(LED_PIN, HIGH);
        }
        if (currentLine.endsWith("GET /LED=OFF")) {
          digitalWrite(LED_PIN, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }

  delay(100);
}
```
Megjegyzés:
Kérlek, cseréld le a your_SSID és your_PASSWORD értékeket a WiFi hálózatod nevére és jelszavára. Ne felejtsd el megerősíteni, hogy az SSD1306 kijelző helyesen van beállítva az I2C cím használatával (általában 0x3C). Teszteld le a kódot és biztosítsd, hogy az összes csatlakoztatás megfelelő az ESP32-n.