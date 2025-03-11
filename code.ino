 #include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi settings
#define STASSID "your_wifi_ssid"
#define STAPSK  "your_wifi_password"

// ThingSpeak API
const char* apiKey = "your_thingspeak_api_key";
const char* server = "api.thingspeak.com";

// Sleep time (15 minutes)
#define SLEEP_TIME 900e6

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

float calculateDewPoint(float temp, float hum) {
  float a = 17.27, b = 237.7;
  float alpha = ((a * temp) / (b + temp)) + log(hum / 100);
  return (b * alpha) / (a - alpha);
}

float calculateHeatIndex(float temp, float hum) {
  return -8.7847 + 1.6114 * temp + 2.3385 * hum - 0.1461 * temp * hum + 
         -0.0123 * temp * temp - 0.0164 * hum * hum + 0.0022 * temp * temp * hum + 
         0.0007 * temp * hum * hum - 0.0000036 * temp * temp * hum * hum;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!htu.begin()) {
    Serial.println("Couldn't find HTU21 sensor!");
    ESP.deepSleep(SLEEP_TIME);
  }

  if (!tsl.begin()) {
    Serial.println("Couldn't find TSL2561 sensor!");
    ESP.deepSleep(SLEEP_TIME);
  }

  tsl.setGain(TSL2561_GAIN_1X);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  float temp = htu.readTemperature();
  float hum = htu.readHumidity();
  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = event.light ? event.light : 0.0;
  float dewPoint = calculateDewPoint(temp, hum);
  float heatIndex = calculateHeatIndex(temp, hum);

  Serial.printf("Temp: %.2f°C | Hum: %.2f%% | Lux: %.2f | Dew Point: %.2f°C | Heat Index: %.2f°C\n",
                temp, hum, lux, dewPoint, heatIndex);

  WiFiClient client;
  HTTPClient http;
  String url = String("http://") + server + "/update?api_key=" + apiKey +
               "&field1=" + String(temp) + "&field2=" + String(hum) +
               "&field3=" + String(lux) + "&field4=" + String(dewPoint) +
               "&field5=" + String(heatIndex);

  http.begin(client, url);
  http.GET();
  http.end();

  Serial.println("Deep Sleep...");
  ESP.deepSleep(SLEEP_TIME);
}

void loop() {}
