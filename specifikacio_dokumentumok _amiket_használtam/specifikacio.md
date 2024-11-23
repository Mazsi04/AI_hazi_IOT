# IoT Megoldás Specifikáció

## Projekt Célja
Az projekt célja egy olyan IoT rendszer megvalósítása, amely egy NodeMCU ESP32S eszköz segítségével méri a környezeti hőmérsékletet és páratartalmat, ezeket az adatokat megjeleníti egy kijelzőn, valamint egy webes interfészen keresztül lehetővé teszi a mért értékek megtekintését és egy LED ki-/bekapcsolását.

## Főbb Funkciók
1. **Hőmérséklet és Páratartalom Mérése**
   - A DHT11 szenzor folyamatos méréseket végez a környezeti hőmérsékletről és páratartalomról.
   - Az adatokat rendszeresen lekéri a NodeMCU ESP32S.

2. **Adatok Megjelenítése Kijelzőn**
   - Egy OLED/LCD kijelzőn folyamatosan megjelennek a mért hőmérséklet- és páratartalom-értékek.

3. **Webes Felület**
   - A NodeMCU ESP32S beépített Wi-Fi modulját használva egy webes szervert futtat.
   - A webes szerveren elérhető egy felhasználói felület, amely megjeleníti a hőmérséklet- és páratartalom-értékeket.
   - A felhasználói felületről lehetőség van a LED ki-/bekapcsolására.

4. **LED Ki-/Bekapcsolása**
   - A webes felületen elérhető gomb segítségével a felhasználó ki- és bekapcsolhatja a LED-et.

## Hardver Összetevők
1. **NodeMCU ESP32S**
   - Mikrovezérlő, amely kezeli a DHT11 méréseit, a kijelző kimeneteit, és a webes szervert.

2. **DHT11 Szenzor**
   - Hőmérséklet- és páratartalom érzékelő szenzor.

3. **OLED/LCD Kijelző**
   - A mért hőmérséklet- és páratartalom-értékek megjelenítésére szolgál.

4. **LED**
   - Webes felületről vezérelhető fényforrás.

## Szoftver Specifikációk
1. **Fejlesztési Környezet**
   - Az alkalmazás fejlesztése az Arduino IDE környezetben történik.

2. **Adatgyűjtés és Megjelenítés**
   - A NodeMCU ESP32S a DHT11 szenzortól lekért adatokat 5 másodpercenként frissíti.
   - Ezeket az adatokat megjeleníti az OLED/LCD kijelzőn.

3. **Webszerver Implementáció**
   - Egy beágyazott webszerver kerül kialakításra az ESP32S-en, amely elérhetővé teszi a szenzorok adatait.
   - A szerver HTTP protokollt használ, és egyszerű HTML/CSS alapú felhasználói felületet biztosít.

4. **LED vezérlés**
   - A felhasználói felületen egy gomb biztosítja a LED ki- és bekapcsolását, amit az ESP32S GPIO portján keresztül vezérel.

## Rendszer Kivitelezés
- Az ESP32S mikrovezérlőt összekötjük a DHT11 szenzorral, a kijelzővel és a LED-del megfelelő GPIO portokon keresztül.
- Tápellátás biztosítása az ESP32S és a perifériák számára.
- Az ESP32S konfigurálása és programozása az Arduino IDE közelben, ahol a szükséges könyvtárakat és funkcionalitásokat hozzuk létre.
- Tesztelés és hibajavítás a rendszer teljes integrációja után.

## Felhasználói Interfészek
1. **Kijelző Felület**
   - Kijelzi az aktuális hőmérséklet és páratartalom értékeket.

2. **Webes Felület**
   - Mutatja az aktuális mért adatok (hőmérséklet, páratartalom) chart-ját vagy táblázatát.
   - Tartalmaz egy gomb interfészt a LED vezérléséhez.

## Fejlesztési Ütemterv
1. Hardware összeszerelés és initial periféria tesztelés.
2. Alapvető firmware írása az adatok olvasására a DHT11-ből és megjelenítése a kijelzőn.
3. Webszerver beállítása és a felhasználói felület kialakítása.
4. LED vezérlési mechanizmus implementálása és integrálása a webes felülettel.
5. Komplett rendszer tesztelése és optimalizálása.

## Kockázatok és Mitigáció
- **Wi-Fi Csatlakozási Problémák:** Biztosítsuk, hogy az ESP32 megfelelő Wi-Fi környezetben működik, és implementáljunk újracsatlakozási mechanizmust.
- **DHT11 Pontosság:** Validáljuk a szenzor kalibrálását és telepítsünk hőpajzsot, ha szükséges.

## Erőforrások és Támogatás
- Online dokumentáció és közösségi fórumok a NodeMCU ESP32S és DHT11 támogatásával kapcsolatban.
- Arduino IDE könyvtárak, mint például a ESPAsyncWebServer és a DHT.