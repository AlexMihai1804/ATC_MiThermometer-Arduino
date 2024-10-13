#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char *deviceAddress1 = "A4:C1:38:XX:XX:01";
const char *deviceAddress2 = "A4:C1:38:XX:XX:02";

ATC_MiThermometer thermometer1(deviceAddress1, Connection_mode::NOTIFICATION);
ATC_MiThermometer thermometer2(deviceAddress2, Connection_mode::NOTIFICATION);

void setup() {
    Serial.begin(115200);
    NimBLEDevice::init("");

    // Initialize thermometers
    thermometer1.init();
    thermometer2.init();
}

void loop() {
    // Data from thermometer 1
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

    // Data from thermometer 2
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

    delay(5000);
}
