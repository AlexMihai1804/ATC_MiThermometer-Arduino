/**
 * @file BLEAdvertisingReader.h
 * @brief This file contains the declaration of the BLEAdvertisingReader class,
 * which is designed to scan for BLE advertising packets and process them for
 * multiple ATC_MiThermometer instances.
 */
#ifndef BLE_ADVERTISING_READER_H
#define BLE_ADVERTISING_READER_H

#include "ATC_MiThermometer.h"
#include <vector>

/**
 * @class BLEAdvertisingReader
 * @brief This class handles scanning for BLE advertisements and parsing the data
 * for registered ATC_MiThermometer objects.
 */
class BLEAdvertisingReader {
public:
    /**
     * @brief Constructor for the BLEAdvertisingReader class.
     * Sets up the BLE scanner and advertisement callbacks.
     */
    BLEAdvertisingReader();

    /**
     * @brief Initiates a BLE scan for a specified duration.
     * @param durationSeconds The duration of the scan in seconds.
     */
    void readAdvertising(uint16_t durationSeconds);

    /**
     * @brief Adds a MiThermometer to the reader's list for data parsing.
     * @param thermometer A pointer to the ATC_MiThermometer to add.
     */
    void addThermometer(ATC_MiThermometer *thermometer);

    /**
     * @brief Removes a MiThermometer from the reader's list.
     * @param thermometer A pointer to the ATC_MiThermometer to remove.
     */
    void removeThermometer(ATC_MiThermometer *thermometer);

    /**
     * @brief Operator overload to add a MiThermometer using '+'.
     * @param thermometer A pointer to the ATC_MiThermometer to add.
     */
    void operator+(ATC_MiThermometer *thermometer);

    /**
     * @brief Operator overload to remove a MiThermometer using '-'.
     * @param thermometer A pointer to the ATC_MiThermometer to remove.
     */
    void operator-(ATC_MiThermometer *thermometer);

    void initAllThermometers();

private:
    NimBLEScan *pBLEScan; /**< Pointer to the NimBLE scan object. */
    std::vector<ATC_MiThermometer *> thermometers; /**< Vector of pointers to ATC_MiThermometer instances. */
    /**
     * @class AdvertisedDeviceCallbacks
     * @brief Nested class to handle callbacks for advertised device events.
     */
    class AdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    public:
        /**
         * @brief Constructor for the AdvertisedDeviceCallbacks class.
         * @param reader Reference to the parent BLEAdvertisingReader instance.
         */
        explicit AdvertisedDeviceCallbacks(BLEAdvertisingReader &reader);

        /**
         * @brief Callback function called when a BLE advertisement is received.
         * This function filters advertisements based on MAC address prefix and
         * calls the parseAdvertisingData() method of the appropriate ATC_MiThermometer instance.
         * @param advertisedDevice A pointer to the received advertisement data.
         */
        void onResult(NimBLEAdvertisedDevice *advertisedDevice) override;

    private:
        BLEAdvertisingReader &parentReader; /**< Reference to the parent BLEAdvertisingReader instance. */
    };
};

#endif // BLE_ADVERTISING_READER_H