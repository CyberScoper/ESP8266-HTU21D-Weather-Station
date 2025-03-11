#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_HTU21DF.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <ESP8266HTTPClient.h>
#include <math.h>

// Wi-Fi настройки
#define STASSID "ssid"
#define STAPSK  "pass"

// ThingSpeak API
const char* apiKey = "API KEY";
const char* server = "api.thingspeak.com";

// 2 минуты для тестов SLEEP_TIME 120e6 
// 15 минут SLEEP_TIME 900e6
#define SLEEP_TIME 900e6

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

float estimateUVIndex(float lux) {
  if (lux < 500) return 0;
  if (lux < 1000) return 1;
  if (lux < 10000) return 2;
  if (lux < 25000) return 3;
  if (lux < 40000) return 4;
  if (lux < 60000) return 5;
  if (lux < 80000) return 6;
  return 7;
}

float calculateDewPoint(float temp, float hum) {
  float a = 17.27;
  float b = 237.7;
  float alpha = ((a * temp) / (b + temp)) + log(hum / 100.0);
  return (b * alpha) / (a - alpha);
}

float calculateHeatIndex(float temp, float hum) {
  float c1 = -8.78469475556;
  float c2 = 1.61139411;
  float c3 = 2.33854883889;
  float c4 = -0.14611605;
  float c5 = -0.012308094;
  float c6 = -0.0164248277778;
  float c7 = 0.002211732;
  float c8 = 0.00072546;
  float c9 = -0.000003582;

  float heatindex = c1 +
                    (c2 * temp) + (c3 * hum) +
                    (c4 * temp * hum) +
                    (c5 * sq(temp)) +
                    (c6 * sq(hum)) +
                    (c7 * sq(temp) * hum) +
                    (c8 * temp * sq(hum)) +
                    (c9 * sq(temp) * sq(hum));

  return heatindex;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  Wire.begin();
  delay(500);

  if (!htu.begin()) {
    Serial.println("Couldn't find HTU21 sensor!");
    ESP.deepSleep(SLEEP_TIME);
  }

  if (!tsl.begin()) {
    Serial.println("Couldn't find TSL2561 sensor!");
    ESP.deepSleep(SLEEP_TIME);
  }

  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  Serial.print("Connecting");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed. Sleeping...");
    ESP.deepSleep(SLEEP_TIME);
  }

  Serial.println("Connected!");

  float temperature = htu.readTemperature();
  float humidity = htu.readHumidity();

  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = (event.light) ? event.light : 0.0;

  float uvIndex = estimateUVIndex(lux);
  float dewPoint = calculateDewPoint(temperature, humidity);
  float heatIndex = calculateHeatIndex(temperature, humidity);

  Serial.printf("Temperature: %.2f C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Illuminance: %.2f lux\n", lux);
  Serial.printf("Estimated UV Index: %.1f\n", uvIndex);
  Serial.printf("Dew Point: %.2f C\n", dewPoint);
  Serial.printf("Heat Index: %.2f C\n", heatIndex);

  WiFiClient client;
  HTTPClient http;

  String url = String("http://") + server + "/update?api_key=" + apiKey +
               "&field1=" + String(temperature) +
               "&field2=" + String(humidity) +
               "&field3=" + String(lux) +
               "&field4=" + String(uvIndex) +
               "&field5=" + String(dewPoint) +
               "&field6=" + String(heatIndex);

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("ThingSpeak Response: %d\n", httpCode);
  } else {
    Serial.printf("HTTP Request failed: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  Serial.println("Going to deep sleep...");
  delay(100);
  ESP.deepSleep(SLEEP_TIME);
}

void loop() {
  // пусто
}
