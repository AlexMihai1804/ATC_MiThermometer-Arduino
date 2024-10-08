#ifndef BLE_ADVERTISING_READER_H
#define BLE_ADVERTISING_READER_H

#include "ATC_MiThermometer.h"
#include <vector>

class BLEAdvertisingReader {
public:
    BLEAdvertisingReader();

    void readAdvertising(uint16_t durationSeconds);

    void addThermometer(ATC_MiThermometer *thermometer);

    void removeThermometer(ATC_MiThermometer *thermometer);

    void operator+(ATC_MiThermometer *thermometer);

    void operator-(ATC_MiThermometer *thermometer);

private:
    NimBLEScan *pBLEScan;
    std::vector<ATC_MiThermometer *> thermometers;

    class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    public:
        AdvertisedDeviceCallbacks(BLEAdvertisingReader &reader);

        void onResult(NimBLEAdvertisedDevice *advertisedDevice) override;

    private:
        BLEAdvertisingReader &parentReader;
    };
};

#endif // BLE_ADVERTISING_READER_H
