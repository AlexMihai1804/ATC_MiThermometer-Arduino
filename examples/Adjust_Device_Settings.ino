#include <NimBLEDevice.h>
#include "ATC_MiThermometer.h"

const char *deviceAddress = "A4:C1:38:XX:XX:XX"; // Replace with your device's MAC address

ATC_MiThermometer thermometer(deviceAddress, Connection_mode::CONNECTION);

void setup() {
    Serial.begin(115200);
    NimBLEDevice::init("");

    // Initialize the thermometer
    thermometer.init();

    // Get current settings
    ATC_MiThermometer_Settings settings = thermometer.getSettings();

    // Modify settings
    settings.advertising_interval = 20; // Set advertising interval steps
    settings.measure_interval = 10;     // Set measure interval steps
    settings.temp_F_or_C = false;       // Set to Celsius
    settings.show_battery = true;       // Show battery level
    settings.smiley = Smiley::SMILEY_HAPPY; // Set happy smiley

    // Send new settings to the device
    thermometer.sendSettings(settings);

    // Optionally, read back the settings to confirm
    settings = thermometer.getSettings();
    Serial.println("New settings applied.");
}

void loop() {
    // Your main code
}
