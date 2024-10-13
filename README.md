# ATC_MiThermometer Arduino Library

This Arduino library allows interaction with Xiaomi Mijia Bluetooth Thermometers and Hygrometers using alternative firmware modules like ATC, PVVX, and BTHome.

## Features

* Support for different connection modes: Advertising, Notification, and Connection.
* Temperature and humidity readings: Obtain precise data directly from the device.
* Device configuration: Modify thermometer settings such as RF transmission power, advertising interval, temperature unit, and more.
* Notification management: Subscribe to and manage notifications for desired characteristics.
* Compatibility with multiple advertising formats: ATC1441, PVVX, and BTHome.

## Installation

### Using the Arduino Library Manager

1. Open the Arduino IDE.
2. Go to `Sketch` -> `Include Library` -> `Manage Libraries...`
3. In the Library Manager, search for "ATC_MiThermometer".
4. Click on the library, then click `Install`.

### Manual Installation

1. **Download the library:**
    * Clone this repository or download the ZIP archive and extract it.
2. **Add the library to Arduino IDE:**
    * Open the Arduino IDE.
    * Go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`
    * Select the downloaded library folder.

## Dependencies

* **NimBLE-Arduino:** The library uses NimBLE for BLE communication. Ensure you have it installed. You can install it from the Library Manager:
    1. Go to `Sketch` -> `Include Library` -> `Manage Libraries...`
    2. Search for "NimBLE-Arduino" and install it.

## Usage

### Initialization

```cpp
#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char* deviceAddress = "A4:C1:38:XX:XX:XX"; // MAC address of the thermometer

ATC_MiThermometer thermometer(deviceAddress, Connection_mode::ADVERTISING);

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("");

  // Initialize the thermometer
  thermometer.init();
}
```
### Reading Data
```
void loop() {
  // Reading temperature
  float temperature = thermometer.getTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Reading humidity
  float humidity = thermometer.getHumidity();
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Reading battery level
  uint8_t batteryLevel = thermometer.getBatteryLevel();
  Serial.print("Battery Level: ");
  Serial.print(batteryLevel);
  Serial.println(" %");

  delay(5000); // Wait 5 seconds before next reading
}
```
### Modifying Device Settings
```
// Changing RF transmission power
thermometer.setRfTxPower(RF_TX_Power::dBm_0_90);

// Setting temperature unit to Fahrenheit
thermometer.setTempFOrC(true);

// Enabling battery display on the screen
thermometer.setShowBattery(true);

// Saving new settings to the device
ATC_MiThermometer_Settings newSettings = thermometer.getSettings();
thermometer.sendSettings(newSettings);
```
### Reading Device Settings
```
ATC_MiThermometer_Settings settings = thermometer.getSettings();
Serial.print("RF TX Power: ");
Serial.println(static_cast<int>(settings.rfTxPower));

Serial.print("Advertising Interval (ms): ");
Serial.println(thermometer.getAdvertisingIntervalMs());

Serial.print("Temperature Unit: ");
Serial.println(settings.temp_F_or_C ? "Fahrenheit" : "Celsius");
```
### Complete Example
For a complete example, check the examples folder in this repository.

### Multi-Device Support
Use the BLEAdvertisingReader class to manage multiple thermometers simultaneously:
```
#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"
#include "BLEAdvertisingReader.h"

const char* deviceAddress1 = "A4:C1:38:XX:XX:01";
const char* deviceAddress2 = "A4:C1:38:XX:XX:02";

ATC_MiThermometer thermometer1(deviceAddress1, Connection_mode::ADVERTISING);
ATC_MiThermometer thermometer2(deviceAddress2, Connection_mode::ADVERTISING);

BLEAdvertisingReader reader;

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("");

  thermometer1.init();
  thermometer2.init();

  reader.addThermometer(&thermometer1);
  reader.addThermometer(&thermometer2);

  reader.readAdvertising(10); // Scan for 10 seconds
}

void loop() {
  // Process data from thermometers
}
```
## Contributions
Contributions are welcome! Please open an issue or a pull request for improvements or bug fixes.

## License
This library is licensed under the MIT License. See the LICENSE file for more details.
