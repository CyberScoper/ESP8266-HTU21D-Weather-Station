#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>

HTU21D myHTU21D;

const char* ssid = "YOUR_SSID";         // 🔐 Your Wi-Fi SSID
const char* password = "YOUR_PASSWORD"; // 🔑 Your Wi-Fi Password

unsigned long myChannelNumber = YOUR_CHANNEL_NUMBER; // 🆔 Your ThingSpeak Channel ID
const char* myWriteAPIKey = "YOUR_WRITE_API_KEY";    // 📝 Your ThingSpeak Write API Key

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  // 🛠️ Initialize the sensor
  Wire.begin();
  myHTU21D.begin();

  // 📶 Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // 🌐 Initialize ThingSpeak
  ThingSpeak.begin(client);

  // 🌡️💧 Read data from the sensor
  float humidity = myHTU21D.readHumidity();
  float temperature = myHTU21D.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // 📤 Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Data successfully sent to ThingSpeak");
  } else {
    Serial.println("Error sending data: " + String(x));
  }

  // 📴 Disconnect Wi-Fi to save power
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // 😴 Enter deep sleep for 15 minutes
  Serial.println("Entering deep sleep for 15 minutes");
  ESP.deepSleep(15 * 60 * 1000000); // Time in microseconds
}

void loop() {
  // The ESP8266 will restart after deep sleep
}
