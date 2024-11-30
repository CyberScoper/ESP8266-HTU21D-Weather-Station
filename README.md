# 📡 ESP8266 & HTU21D Weather Station with ThingSpeak Integration

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/cefc8c36755e48b4aec85943c31b000c)](https://app.codacy.com/gh/CyberScopeToday/ESP8266-HTU21D-Weather-Station?utm_source=github.com&utm_medium=referral&utm_content=CyberScopeToday/ESP8266-HTU21D-Weather-Station&utm_campaign=Badge_Grade)

Measure temperature 🌡️ and humidity 💧 using an ESP8266 microcontroller and an HTU21D sensor, then send the data to ThingSpeak every 15 minutes while conserving power using deep sleep mode 😴.

## 📋 Table of Contents

- [Introduction](#introduction)
- [Components Required](#components-required)
- [Circuit Diagram](#circuit-diagram)
- [Setup Instructions](#setup-instructions)
  - [1. Wiring](#1-wiring)
  - [2. Preparing the ESP8266](#2-preparing-the-esp8266)
  - [3. Setting Up ThingSpeak](#3-setting-up-thingspeak)
  - [4. Programming the ESP8266](#4-programming-the-esp8266)
- [Code Explanation](#code-explanation)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Introduction

This project demonstrates how to build a low-power weather station using the **ESP8266** microcontroller and the **HTU21D** temperature and humidity sensor. The ESP8266 reads data from the sensor and uploads it to **ThingSpeak** every 15 minutes. Between readings, the ESP8266 enters deep sleep mode to conserve power.

## Components Required

- 🧠 **ESP8266** Development Board (e.g., NodeMCU or WeMos D1 Mini)
- 🌡️ **HTU21D** Temperature and Humidity Sensor
- 🔌 Jumper Wires
- 📶 Wi-Fi Network with Internet Access
- 🔋 Power Supply (e.g., USB cable or battery pack)

## Circuit Diagram

🔗 **Connections:**

HTU21D Pin | ESP8266 Pin
---------- | -----------
VCC        | 3.3V
GND        | GND
SDA        | GPIO4 (D2)
SCL        | GPIO5 (D1)

🔌 **Deep Sleep Connection:**

- **GPIO16 (D0)** connected to **RST**

## Setup Instructions

### 1. Wiring

Connect the **HTU21D** sensor to the **ESP8266** as per the connections above.

- Use jumper wires to make the connections.
- Ensure that **GPIO16 (D0)** is connected to **RST** to enable wake-up from deep sleep.

### 2. Preparing the ESP8266

- Install the **Arduino IDE** if you haven't already.
- Add the **ESP8266 board** to the Arduino IDE:
  - Go to **File** > **Preferences**.
  - In **Additional Boards Manager URLs**, add:
    ```
    http://arduino.esp8266.com/stable/package_esp8266com_index.json
    ```
  - Go to **Tools** > **Board** > **Boards Manager**, search for **ESP8266**, and install it.

### 3. Setting Up ThingSpeak

1. Sign up for a free account on [ThingSpeak](https://thingspeak.com/).
2. Create a new **Channel**.
3. Add two fields:
   - **Field 1**: Temperature 🌡️
   - **Field 2**: Humidity 💧
4. Note down your **Channel ID** and **Write API Key**.
   - You'll need these for the code.

### 4. Programming the ESP8266

#### 📦 Libraries Required

Make sure you have the following libraries installed:

- **ESP8266WiFi.h** (comes with ESP8266 core)
- **ThingSpeak.h**
- **Wire.h** (comes with Arduino IDE)
- **SparkFunHTU21D.h**

#### 🔧 Installing Libraries

- **ThingSpeak Library**:
  - Go to **Sketch** > **Include Library** > **Manage Libraries**.
  - Search for **ThingSpeak** and install it.
- **SparkFun HTU21D Library**:
  - Search for **SparkFun HTU21D** and install it.

#### 📝 Code

Replace the placeholders in the code with your Wi-Fi credentials, ThingSpeak Channel ID, and Write API Key.

```cpp
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
```

#### 🚀 Uploading the Code

1. Connect your ESP8266 to your computer via USB.
2. Select the correct **Board** and **Port** from the **Tools** menu.
3. Click the **Upload** button.

## Code Explanation

- **Libraries Included**:
  - `ESP8266WiFi.h`: Handles Wi-Fi connectivity.
  - `ThingSpeak.h`: Interfaces with ThingSpeak API.
  - `Wire.h`: Enables I2C communication.
  - `SparkFunHTU21D.h`: Communicates with the HTU21D sensor.

- **Global Variables**:
  - Wi-Fi credentials (`ssid`, `password`).
  - ThingSpeak configuration (`myChannelNumber`, `myWriteAPIKey`).
  - `HTU21D` object for sensor interaction.

- **`setup()` Function**:
  - Initializes serial communication for debugging.
  - Initializes the HTU21D sensor.
  - Connects to Wi-Fi.
  - Reads temperature and humidity data.
  - Sends data to ThingSpeak.
  - Disconnects Wi-Fi to save power.
  - Puts the ESP8266 into deep sleep for 15 minutes.

- **`loop()` Function**:
  - Left empty because the ESP8266 will reset after waking up from deep sleep, and the code in `setup()` will run again.

## Usage

- Once uploaded, the ESP8266 will:
  1. Wake up from deep sleep.
  2. Connect to Wi-Fi.
  3. Read sensor data.
  4. Send data to ThingSpeak.
  5. Enter deep sleep for 15 minutes.
  6. Repeat the cycle.

- 📈 View your data on ThingSpeak:
  - Log in to your ThingSpeak account.
  - Navigate to your channel to see real-time updates.

## Troubleshooting

- **Cannot Connect to Wi-Fi**:
  - Double-check your SSID and password.
  - Ensure your Wi-Fi network is 2.4 GHz (ESP8266 does not support 5 GHz).

- **No Data on ThingSpeak**:
  - Verify your **Channel ID** and **Write API Key** are correct.
  - Ensure your ThingSpeak channel is set up properly with the correct fields.

- **ESP8266 Not Waking Up**:
  - Make sure **GPIO16 (D0)** is connected to **RST**.
  - Check your power supply.

- **Error Codes from ThingSpeak**:
  - **-301**: Failed to connect to ThingSpeak.
  - **-302**: Failed to update data.
  - **-401**: Unauthorized (check your API Key).

## License

This project is open-source and available under the MIT License.

---

Enjoy your new low-power weather station! 🌤️ If you have any questions or need further assistance, feel free to ask. Happy coding! 🤖
