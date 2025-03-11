> **Language:**  
> [English (current)](./README.md) | [Русский (Russian)](./README_ru.md)

# ☀️ ESP8266 Weather Station (HTU21D + TSL2561) with Solar Power and ThingSpeak

This project is an **autonomous outdoor weather station** built around the **ESP8266 (ESP-12E)** microcontroller. It measures **temperature, humidity, illuminance (lux), dew point, and heat index**, then sends the data to **ThingSpeak** every **15 minutes**.

**⚡️ Key Highlights**  
- Solar-powered (using TP4057 and a LiPo 2000 mAh battery)  
- Deep Sleep for energy efficiency (15 minutes)  
- Sensors:
  - **HTU21D** for temperature and humidity
  - **TSL2561** for illuminance
- Calculates **dew point** and **heat index**  
- Uses **AMS1117** (3.3 V) as a linear regulator (simple but not very power-efficient)

## ThingSpeak Channel
- **👉 Live data**: [ThingSpeak link](https://thingspeak.mathworks.com/channels/2749663)

## 3D-Printed Enclosure
- 👉 We use a **3D-printed case** to protect the device:  
  [Thingiverse Model](https://www.thingiverse.com/thing:5810438)

---

## 🧷 Features

1. **ESP8266 (ESP-12E)** – Wi-Fi connectivity and data transmission  
2. **HTU21D** – temperature & humidity measurements  
3. **TSL2561** – illuminance measurement (lux)  
4. **Dew Point & Heat Index** – computed from sensor data  
5. **Data Upload to ThingSpeak** – via simple HTTP GET requests  
6. **Solar Power** – recharges a LiPo battery through **TP4057**  
7. **Deep Sleep (15 min)** – minimizes power consumption  
8. **AMS1117** – used between the LiPo and the ESP8266 (simple, but draws ~5–10 mA quiescent current)

---

## 🛠 Hardware Components

| Component               | Purpose                                                                          |
|-------------------------|----------------------------------------------------------------------------------|
| **ESP-12E (ESP8266)**   | Main microcontroller with Wi-Fi                                                  |
| **HTU21D**              | Temperature & humidity sensor                                                    |
| **TSL2561**             | Illuminance (lux) sensor                                                         |
| **TP4057**              | LiPo charging module                                                             |
| **LiPo 2000 mAh**       | Rechargeable battery                                                             |
| **AMS1117 (3.3 V)**     | Linear regulator (simple but not the most efficient)                             |
| **5V Solar Panel**      | Charges the battery via TP4057                                                  |
| **3D-printed enclosure**| Protects the device from weather (Thingiverse design)                            |

> **Note on AMS1117**: It is easy to use but has a relatively high quiescent current (5–10 mA), which reduces battery life. For better efficiency, consider a low-dropout regulator (e.g., MCP1700) or a DC-DC converter.

---

## 🔌Wiring

### 1. Sensors (I²C)

**HTU21D** → ESP8266
```
VCC -> 3.3V
GND -> GND
SDA -> GPIO4 (D2)
SCL -> GPIO5 (D1)
```

**TSL2561** → ESP8266
```
VCC -> 3.3V
GND -> GND
SDA -> GPIO4 (D2) [shared with HTU21D]
SCL -> GPIO5 (D1) [shared with HTU21D]
```

### 🔋 2. Power (Solar + TP4057 + LiPo + AMS1117)

- **Solar Panel (5V)** → **TP4057 IN**  
- **TP4057 OUT** → **LiPo battery** (3.7 V nominal)  
- **LiPo** → **AMS1117 VIN**  
- **AMS1117 VOUT** → **3.3 V** to power the ESP8266  

### 3. ESP8266 Pins

| Pin       | Connection                                  |
|-----------|---------------------------------------------|
| **VCC**   | 3.3V (from AMS1117)                         |
| **GND**   | GND                                         |
| **EN**    | 3.3V via 10kΩ resistor (CH_PD)              |
| **GPIO16**| Connect to RST (for Deep Sleep)             |
| **GPIO0** | GND during flashing, then 3.3V (via 10kΩ)    |
| **GPIO2,15** | Standard ESP8266 boot configuration       |

---

## 🖥 Arduino IDE Setup

1. **Install Arduino IDE** from the [official website](https://www.arduino.cc/en/software).  
2. In Arduino IDE, go to:  
   ```
   File > Preferences > Additional Boards Manager URLs
   ```
   and add:  
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. **Open Boards Manager** (`Tools > Board > Boards Manager`), search for **esp8266**, and install **"ESP8266 by ESP8266 Community"**.  
4. Choose **Generic ESP8266 Module** (or **NodeMCU 1.0**) in the **Tools > Board** menu.  

---

## 📚 Required Libraries

- **ESP8266WiFi** *(bundled with ESP8266 platform)*  
- **Wire** *(standard I²C library)*  
- **Adafruit HTU21DF**  
- **Adafruit Unified Sensor**  
- **Adafruit TSL2561**  
- **ESP8266HTTPClient** *(bundled with ESP8266 platform)*  

Install them via **Sketch > Include Library > Manage Libraries** in Arduino IDE.

---

## 💡 Flashing the ESP8266

1. **Connect** ESP8266 to your PC via a USB-UART adapter (e.g., CH340, FTDI).  
2. **Enter flashing mode**:  
   - **GPIO0 → GND**  
   - **RESET** (toggle) or power-cycle  
   - Now the ESP8266 is ready for programming  
3. **Select** the correct board (e.g., Generic ESP8266) and **serial port** in Arduino IDE.  
4. **Upload** the sketch.  
5. After successful flashing, **disconnect GPIO0** from GND (pull it up to 3.3V), then **reset** the module.

---

## 🕐 Adjusting the Data Send Interval

The code uses **Deep Sleep** for power saving. The default sleep time is **15 minutes**, set in microseconds:

```cpp
#define SLEEP_TIME 900e6  // 15 minutes (900,000,000 µs)
```

- **1 second** = `1e6` µs  
- **1 minute** = `60 * 1e6` µs  
- Example: 5 minutes = `300e6`, 30 minutes = `1800e6`

**Important**: Connect **GPIO16** to **RST** so the ESP8266 can wake from Deep Sleep.

---

## ThingSpeak Fields

| Field   | Description    | Unit |
|---------|---------------|------|
| Field1  | Temperature   | °C   |
| Field2  | Humidity      | %    |
| Field3  | Illuminance   | lux  |
| Field4  | Dew Point     | °C   |
| Field5  | Heat Index    | °C   |

Set your **Wi-Fi** credentials and **ThingSpeak API key** in the sketch:

```cpp
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

```

---

## Power Consumption Notes

- Using **AMS1117** is straightforward, but it has a relatively high quiescent current (5–10 mA).  
- This can limit battery life in long-term operation.  
- For better efficiency, use a **low-quiescent-current LDO** (e.g., MCP1700, HT7333) or a **DC-DC converter**.  

---

## 🏁 Future Plans
- Add a **UV sensor** (e.g., VEML6075)  
- Integrate **MQTT** for Home Assistant or Node-RED  
- Create a **local web interface** for real-time data visualization  

---

## 📜 License
This project is released under the **MIT License**. Feel free to use, modify, and distribute!

If you have any questions or suggestions, open an [Issue](https://github.com/CyberScopeToday/ESP8266-HTU21D-Weather-Station/issues).
