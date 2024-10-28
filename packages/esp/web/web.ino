#include <RestClient.h>
#include <DHT.h>
#include "env.h"

#define DHTPIN 5  // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT11  // DHT 11

char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

RestClient client = RestClient(HOST, PORT);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  client.begin(SSID, PASS);
}

void to_hex(int value, char* buffer, int offset) {
  buffer[offset + 1] = hex_chars[(value >> 0x00) & 0xF];
  buffer[offset + 0] = hex_chars[(value >> 0x04) & 0xF];

  buffer[offset + 3] = hex_chars[(value >> 0x08) & 0xF];
  buffer[offset + 2] = hex_chars[(value >> 0x0C) & 0xF];

  buffer[offset + 5] = hex_chars[(value >> 0x10) & 0xF];
  buffer[offset + 4] = hex_chars[(value >> 0x14) & 0xF];

  buffer[offset + 7] = hex_chars[(value >> 0x18) & 0xF];
  buffer[offset + 6] = hex_chars[(value >> 0x1C) & 0xF];
}

void loop() {
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  char body[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  to_hex(*reinterpret_cast<int*>(&h), body, 0);
  to_hex(*reinterpret_cast<int*>(&t), body, 8);
  to_hex(*reinterpret_cast<int*>(&f), body, 16);
  to_hex(*reinterpret_cast<int*>(&hif), body, 24);
  to_hex(*reinterpret_cast<int*>(&hic), body, 32);

  client.post("/data", body);
}
