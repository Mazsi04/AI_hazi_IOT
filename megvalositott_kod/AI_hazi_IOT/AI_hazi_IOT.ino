#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// SSD1306 OLED kijelző méret
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED reset pin (nem használjuk)
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT szenzor
#define DHTPIN 4 // Állítsd be a megfelelő GPIO-ra
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Soros port inicializálása
  Serial.begin(115200);

  // I2C busz inicializálása GPIO21 és GPIO22 lábakon
  Wire.begin(21, 22);

  // OLED kijelző inicializálása
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 kijelző nem található"));
    for (;;); // Ne folytassa, ha nem talál kijelzőt
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
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("Temp:");
  display.print(t);
  //display.print("C");
  display.setCursor(0, 40);
  display.print("Para:");
  display.print(h);
  //display.print("%");

  
  display.display();
  
  delay(2000); // Frissítés 2 másodpercenként
}
