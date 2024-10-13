#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char *deviceAddress = "A4:C1:38:XX:XX:XX"; // Replace with your device's MAC address

ATC_MiThermometer thermometer(deviceAddress, Connection_mode::CONNECTION);

void setup() {
    Serial.begin(115200);
    NimBLEDevice::init("");

    // Initialize the thermometer
    thermometer.init();

    // Reset settings to defaults
    thermometer.resetSettings();

    Serial.println("Device settings have been reset to default.");
}

void loop() {
    // Your main code
}
