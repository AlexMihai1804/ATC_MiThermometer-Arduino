/**
 * @file BLEAdvertisingReader.cpp
 * @brief This file contains the implementation for the BLEAdvertisingReader class,
 * which scans for BLE advertisements and parses data for registered ATC_MiThermometer instances.
 */
#include "BLEAdvertisingReader.h"
#include <Arduino.h>
#include <algorithm>
#include <cctype>

/**
 * @brief Constructor for the BLEAdvertisingReader class. Initializes the BLE scan object and sets the callback function.
 * Sets the scan to active mode with a specific interval and window.
 */
BLEAdvertisingReader::BLEAdvertisingReader() {
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(*this));
    pBLEScan->setActiveScan(true); // Active scan uses more power, but get more information.
    pBLEScan->setInterval(100); // Scan interval in milliseconds
    pBLEScan->setWindow(99); // Scan window in milliseconds. Less or equal than interval
}

/**
 * @brief Starts a BLE scan for a specified duration. Clears previous scan results before starting.
 * @param durationSeconds The duration of the scan in seconds.
 */
void BLEAdvertisingReader::readAdvertising(uint16_t durationSeconds) {
    pBLEScan->start(durationSeconds, false); // The second parameter is for duplicate filtering.
    pBLEScan->clearResults(); // Clear any previous scan results.
}

/**
 * @brief Adds a thermometer to the list of thermometers to monitor.
 * Avoids adding duplicates.
 * @param thermometer A pointer to the ATC_MiThermometer instance to add.
 */
void BLEAdvertisingReader::addThermometer(ATC_MiThermometer *thermometer) {
    if (std::find(thermometers.begin(), thermometers.end(), thermometer) == thermometers.end()) {
        thermometers.push_back(thermometer);
    }
}

/**
 * @brief Removes a thermometer from the list of thermometers to monitor.
 * @param thermometer  A pointer to the ATC_MiThermometer instance to remove.
 */
void BLEAdvertisingReader::removeThermometer(ATC_MiThermometer *thermometer) {
    auto it = std::remove(thermometers.begin(), thermometers.end(), thermometer);
    if (it != thermometers.end()) {
        thermometers.erase(it, thermometers.end());
    }
}

/**
 * @brief Overload the + operator to add a thermometer.
 * @param thermometer A pointer to the ATC_MiThermometer to add.
 */
void BLEAdvertisingReader::operator+(ATC_MiThermometer *thermometer) {
    addThermometer(thermometer);
}

/**
 * @brief Overload the - operator to remove a thermometer.
 * @param thermometer  A pointer to the ATC_MiThermometer to remove.
 */
void BLEAdvertisingReader::operator-(ATC_MiThermometer *thermometer) {
    removeThermometer(thermometer);
}

void BLEAdvertisingReader::initAllThermometers() {
    for (ATC_MiThermometer *thermometer: thermometers) {
        if (thermometer->getReadSettings())
            continue;
        thermometer->init();
    }
}

/**
 * @brief Constructor for the AdvertisedDeviceCallbacks class.
 * Stores a reference to the parent BLEAdvertisingReader.
 * @param reader  A reference to the parent BLEAdvertisingReader instance.
 */
BLEAdvertisingReader::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(BLEAdvertisingReader &reader)
        : parentReader(reader) {}

/**
 * @brief Callback function for when a BLE advertisement is received.  Checks if the device address starts with "A4"
 * (indicating a Xiaomi device) and then calls parseAdvertisingData on the matching ATC_MiThermometer instance.
 * @param advertisedDevice  A pointer to the NimBLEAdvertisedDevice object representing the advertising device.
 */
void BLEAdvertisingReader::AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice *advertisedDevice) {
    std::string deviceAddress = advertisedDevice->getAddress().toString();
    // Simple filter to reduce processing time. Checks if MAC address starts with "A4" (case-insensitive).
    // Most Xiaomi devices have MAC addresses starting with "A4:C1:38".
    if (deviceAddress.size() < 2) {
        return;
    }
    if (!(std::tolower(deviceAddress[0]) == 'a' && std::tolower(deviceAddress[1]) == '4')) {
        return;
    }
    for (ATC_MiThermometer *thermometer: parentReader.thermometers) {
        if (!thermometer)
            continue;

        std::string thermometerAddress = thermometer->getAddress();

        if (deviceAddress.length() != thermometerAddress.length()) {
            continue;
        }
        // Case-insensitive comparison of the addresses
        if (std::equal(deviceAddress.begin(), deviceAddress.end(), thermometerAddress.begin(),
                       [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
            const uint8_t *payload = advertisedDevice->getPayload();
            size_t payloadLength = advertisedDevice->getPayloadLength();
            thermometer->parseAdvertisingData(payload, payloadLength);
            return;
        }
    }
}