#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char *deviceAddress = "A4:C1:38:XX:XX:XX"; // Replace with your device's MAC address

ATC_MiThermometer thermometer(deviceAddress, Connection_mode::CONNECTION);

void setup() {
    Serial.begin(115200);
    NimBLEDevice::init("");

    // Initialize the thermometer
    thermometer.init();
}

void loop() {
    // Read data on demand
    thermometer.readTemperature();
    thermometer.readHumidity();
    thermometer.readBatteryLevel();

    float temperature = thermometer.getTemperature();
    float humidity = thermometer.getHumidity();
    uint8_t batteryLevel = thermometer.getBatteryLevel();

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Battery Level: ");
    Serial.print(batteryLevel);
    Serial.println(" %");

    delay(5000);
}
