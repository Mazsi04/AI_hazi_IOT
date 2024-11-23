#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// WiFi beállítások
const char* ssid = "fhazinet300";
const char* password = "Mazsola04";

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