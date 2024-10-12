#include "BLEAdvertisingReader.h"
#include <Arduino.h>
#include <algorithm>
#include <cctype>

BLEAdvertisingReader::BLEAdvertisingReader() {
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(*this));
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void BLEAdvertisingReader::readAdvertising(uint16_t durationSeconds) {
    Serial.printf("Starting BLE scan for %d seconds...\n", durationSeconds);
    pBLEScan->start(durationSeconds, false);
    Serial.printf("Found %d devices\n", pBLEScan->getResults().getCount());
    pBLEScan->clearResults();
}

void BLEAdvertisingReader::addThermometer(ATC_MiThermometer *thermometer) {
    if (std::find(thermometers.begin(), thermometers.end(), thermometer) == thermometers.end()) {
        thermometers.push_back(thermometer);
        Serial.printf("Added thermometer with address: %s\n", thermometer->getAddress());
    }
}

void BLEAdvertisingReader::removeThermometer(ATC_MiThermometer *thermometer) {
    auto it = std::remove(thermometers.begin(), thermometers.end(), thermometer);
    if (it != thermometers.end()) {
        thermometers.erase(it, thermometers.end());
        Serial.printf("Removed thermometer with address: %s\n", thermometer->getAddress());
    }
}

void BLEAdvertisingReader::operator+(ATC_MiThermometer *thermometer) {
    addThermometer(thermometer);
}

void BLEAdvertisingReader::operator-(ATC_MiThermometer *thermometer) {
    removeThermometer(thermometer);
}

BLEAdvertisingReader::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(BLEAdvertisingReader &reader)
        : parentReader(reader) {}

void BLEAdvertisingReader::AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice *advertisedDevice) {
    std::string deviceAddress = advertisedDevice->getAddress().toString();
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

        if (std::equal(deviceAddress.begin(), deviceAddress.end(), thermometerAddress.begin(),
                       [](char a, char b) { return std::tolower(a) == std::tolower(b); })) {
            const uint8_t *payload = advertisedDevice->getPayload();
            size_t payloadLength = advertisedDevice->getPayloadLength();
            thermometer->parseAdvertisingData(payload, payloadLength);
            return;
        }
    }
}
