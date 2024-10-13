#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char *deviceAddress = "A4:C1:38:XX:XX:XX"; // Replace with your device's MAC address

ATC_MiThermometer thermometer(deviceAddress, Connection_mode::CONNECTION);

void setup() {
    Serial.begin(115200);
    NimBLEDevice::init("");

    // Initialize the thermometer
    thermometer.init();

    // Set the clock to the current time
    // For demonstration, we'll set it to 12:30:00 on January 1, 2024
    uint8_t hours = 12;
    uint8_t minutes = 30;
    uint8_t seconds = 0;
    uint8_t day = 1;
    uint8_t month = 1;
    uint16_t year = 2024;

    thermometer.setClock(hours, minutes, seconds, day, month, year);

    Serial.println("Clock set successfully.");
}

void loop() {
    // Your main code
}
