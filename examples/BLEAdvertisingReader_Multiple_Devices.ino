#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"
#include "BLEAdvertisingReader.h"

const char *deviceAddress1 = "A4:C1:38:XX:XX:01";
const char *deviceAddress2 = "A4:C1:38:XX:XX:02";

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
}

void loop() {
    reader.readAdvertising(5); // Scan for 5 seconds

    // After scanning, get data
    float temp1 = thermometer1.getTemperature();
    float hum1 = thermometer1.getHumidity();
    uint8_t batt1 = thermometer1.getBatteryLevel();

    Serial.println("Thermometer 1:");
    Serial.print("Temperature: ");
    Serial.print(temp1);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(hum1);
    Serial.println(" %");

    Serial.print("Battery Level: ");
    Serial.print(batt1);
    Serial.println(" %");

    float temp2 = thermometer2.getTemperature();
    float hum2 = thermometer2.getHumidity();
    uint8_t batt2 = thermometer2.getBatteryLevel();

    Serial.println("Thermometer 2:");
    Serial.print("Temperature: ");
    Serial.print(temp2);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(hum2);
    Serial.println(" %");

    Serial.print("Battery Level: ");
    Serial.print(batt2);
    Serial.println(" %");

    delay(10000); // Wait before next scan
}
