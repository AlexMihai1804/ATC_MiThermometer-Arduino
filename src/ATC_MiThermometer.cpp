/**
 * @file ATC_MiThermometer.cpp
 * @brief This file contains the implementation of the ATC_MiThermometer class.
 */
#include "ATC_MiThermometer.h"
#include <cmath>
#include <algorithm>
#include <mutex>
#include <map>

static std::mutex bleMutex; /**< Mutex for thread safety during BLE operations. */
/**
 * @brief Constructor for the ATC_MiThermometer class.
 * @param address The MAC address of the thermometer.
 * @param connection_mode The connection mode to use (ADVERTISING, NOTIFICATION, or CONNECTION). Defaults to ADVERTISING.
 */
ATC_MiThermometer::ATC_MiThermometer(const char *address, Connection_mode connection_mode)
        : address(address), pClient(nullptr), environmentService(nullptr), connection_mode(connection_mode),
          batteryService(nullptr), commandService(nullptr), temperatureCharacteristic(nullptr),
          temperaturePreciseCharacteristic(nullptr), humidityCharacteristic(nullptr), batteryCharacteristic(nullptr),
          commandCharacteristic(nullptr), received_settings(false), read_settings(false), started_notify_temp(false),
          started_notify_temp_precise(false), started_notify_humidity(false), started_notify_battery(false),
          temperature(0), temperature_precise(0), humidity(0), battery_mv(0), battery_level(0), time_tracking(false),
          last_read_time(0) {
}

/**
 * @brief Destructor for the ATC_MiThermometer class.  Disconnects from the thermometer.
 */
ATC_MiThermometer::~ATC_MiThermometer() {
    disconnect();
}

/**
 * @brief Connects to the thermometer.  Attempts to connect up to 5 times.
 */
void ATC_MiThermometer::connect() {
    std::lock_guard<std::mutex> lock(bleMutex);
    if (pClient && pClient->isConnected()) {
        pClient->disconnect();
    }
    pClient = NimBLEDevice::createClient();
    if (!pClient) {
        Serial.println("Failed to create BLE client");
        return;
    }
    NimBLEAddress bleAddress(address);
    for (int i = 0; i < 5; i++) {
        if (pClient->connect(bleAddress)) {
            return;
        }
        delay(1000);
    }
    Serial.printf("Failed to connect to %s after 5 attempts\n", address);
}

/**
 * @brief Checks if the thermometer is currently connected.
 * @return True if connected, false otherwise.
 */
bool ATC_MiThermometer::isConnected() const {
    return pClient && pClient->isConnected();
}

/**
 * @brief Connects to the environment service.  Prints an error message if the service is not found.
 */
void ATC_MiThermometer::connectToEnvironmentService() {
    environmentService = pClient->getService("181A"); // Environmental Sensing Service
    if (!environmentService) {
        Serial.printf("Failed to find service %s\n", "181A");
    }
}

/**
 * @brief Connects to the temperature characteristic.  Prints an error message if the characteristic is not found.
 */
void ATC_MiThermometer::connectToTemperatureCharacteristic() {
    if (!environmentService) {
        connectToEnvironmentService();
        if (!environmentService) {
            return;
        }
    }
    temperatureCharacteristic = environmentService->getCharacteristic("2A1F"); // Temperature characteristic UUID
    if (!temperatureCharacteristic) {
        Serial.printf("Failed to find characteristic %s\n", "2A1F");
    }
}

/**
 * @brief Begins notifications for temperature.  Subscribes to the temperature characteristic's notifications.
 *        Prints an error message if the characteristic is not found or cannot notify.
 */
void ATC_MiThermometer::beginNotifyTemp() {
    if (!temperatureCharacteristic) {
        connectToTemperatureCharacteristic();
        if (!temperatureCharacteristic) {
            return;
        }
    }
    if (temperatureCharacteristic->canNotify()) {
        temperatureCharacteristic->subscribe(true, [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                          const uint8_t *pData, size_t length, bool isNotify) {
            this->notifyTempCallback(pBLERemoteCharacteristic, pData, length, isNotify);
        });
        started_notify_temp = true;
    } else {
        Serial.println("Temperature characteristic cannot notify");

    }
}

/**
 * @brief Callback function for temperature notifications.  Updates the internal temperature value.
 *        Prints an error message if invalid data is received.
 * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
 * @param pData Pointer to the notification data.
 * @param length Length of the notification data.
 * @param isNotify True if this is a notification, false otherwise.
 */
void ATC_MiThermometer::notifyTempCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData,
                                           size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t temp = (pData[1] << 8) | pData[0];
        temperature = static_cast<float>(temp) / 10.0f;
        if (time_tracking) {
            last_read_time = time(nullptr);
        }
    } else {
        Serial.println("Received invalid temperature data");
    }
}

/**
 * @brief Connects to the precise temperature characteristic.
 */
void ATC_MiThermometer::connectToTemperaturePreciseCharacteristic() {
    if (!environmentService) {
        connectToEnvironmentService();
        if (!environmentService) {
            return;
        }
    }
    temperaturePreciseCharacteristic = environmentService->getCharacteristic(
            "2A6E"); // Precise Temperature characteristic UUID
    if (!temperaturePreciseCharacteristic) {
        Serial.printf("Failed to find characteristic %s\n", "2A6E");
        return;
    }
}

/**
 * @brief Begins notifications for precise temperature. Subscribes to the precise temperature characteristic's notifications.
 */
void ATC_MiThermometer::beginNotifyTempPrecise() {
    if (!temperaturePreciseCharacteristic) {
        connectToTemperaturePreciseCharacteristic();
        if (!temperaturePreciseCharacteristic) {
            return;
        }
    }
    if (temperaturePreciseCharacteristic->canNotify()) {
        temperaturePreciseCharacteristic->subscribe(true, [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                                 const uint8_t *pData, size_t length, bool isNotify) {
            this->notifyTempPreciseCallback(pBLERemoteCharacteristic, pData, length, isNotify);
        });
        started_notify_temp_precise = true;
    } else {
        Serial.println("Precise Temperature characteristic cannot notify");
    }
}

/**
 * @brief Callback function for precise temperature notifications.  Updates the internal precise temperature value.
 *        Prints an error message if invalid data is received.
 * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
 * @param pData Pointer to the notification data.
 * @param length  Length of the notification data.
 * @param isNotify  True if this is a notification, false otherwise.
 */
void
ATC_MiThermometer::notifyTempPreciseCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData,
                                             size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t temp = (pData[1] << 8) | pData[0];
        temperature_precise = static_cast<float>(temp) / 100.0f;
        if (time_tracking) {
            last_read_time = time(nullptr);
        }
    } else {
        Serial.println("Received invalid precise temperature data");
    }
}

/**
* @brief Connects to the humidity characteristic. Prints an error message if the characteristic is not found.
*/
void ATC_MiThermometer::connectToHumidityCharacteristic() {
    if (!environmentService) {
        connectToEnvironmentService();
        if (!environmentService) {
            return;
        }
    }
    humidityCharacteristic = environmentService->getCharacteristic("2A6F"); // Humidity characteristic UUID
    if (!humidityCharacteristic) {
        Serial.printf("Failed to find characteristic %s\n", "2A6F");
    }
}

/**
 * @brief Begins notifications for humidity. Subscribes to the humidity characteristic's notifications.
 *        Prints an error message if the characteristic is not found or cannot notify.
 */
void ATC_MiThermometer::beginNotifyHumidity() {
    if (!humidityCharacteristic) {
        connectToHumidityCharacteristic();
        if (!humidityCharacteristic) {
            return;
        }
    }
    if (humidityCharacteristic->canNotify()) {
        humidityCharacteristic->subscribe(true,
                                          [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                 const uint8_t *pData, size_t length, bool isNotify) {
                                              this->notifyHumidityCallback(pBLERemoteCharacteristic, pData, length,
                                                                           isNotify);
                                          });
        started_notify_humidity = true;
    } else {
        Serial.println("Humidity characteristic cannot notify");
    }
}

/**
 * @brief Callback function for humidity notifications.  Updates the internal humidity value.
 *      Prints an error message if invalid data is received.
 * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
 * @param pData Pointer to the notification data.
 * @param length Length of the notification data.
 * @param isNotify  True if this is a notification, false otherwise.
 */
void
ATC_MiThermometer::notifyHumidityCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData,
                                          size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t hum = (pData[1] << 8) | pData[0];
        humidity = static_cast<float>(hum) / 100.0f;
        if (time_tracking) {
            last_read_time = time(nullptr);
        }
    } else {
        Serial.println("Received invalid humidity data");
    }
}

/**
 * @brief  Connects to the battery service.
 */
void ATC_MiThermometer::connectToBatteryService() {
    batteryService = pClient->getService("180F"); // Battery Service UUID
    if (!batteryService) {
        Serial.printf("Failed to find service %s\n", "180F");

        return;
    }
}

/**
 * @brief Connects to the battery characteristic.  Prints an error message if the characteristic is not found.
 */
void ATC_MiThermometer::connectToBatteryCharacteristic() {
    if (!batteryService) {
        connectToBatteryService();
        if (!batteryService) {
            return;
        }
    }
    batteryCharacteristic = batteryService->getCharacteristic("2A19"); // Battery Level characteristic UUID
    if (!batteryCharacteristic) {
        Serial.printf("Failed to find characteristic %s\n", "2A19");
    }
}

/**
 * @brief  Begins notifications for battery level.  Subscribes to the battery characteristic's notifications.
 *         Prints an error message if the characteristic is not found or cannot notify.
 */
void ATC_MiThermometer::beginNotifyBattery() {
    if (!batteryCharacteristic) {
        connectToBatteryCharacteristic();
        if (!batteryCharacteristic) {
            return;
        }
    }
    if (batteryCharacteristic->canNotify()) {
        batteryCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                const uint8_t *pData, size_t length, bool isNotify) {
                                             this->notifyBatteryCallback(pBLERemoteCharacteristic, pData, length,
                                                                         isNotify);
                                         });
        started_notify_battery = true;
    } else {
        Serial.println("Battery characteristic cannot notify");

    }
}

/**
 * @brief Callback function for battery level notifications.  Updates the internal battery level value.
 *        Prints an error message if invalid data is received.
 * @param pBLERemoteCharacteristic  Pointer to the characteristic that triggered the notification.
 * @param pData Pointer to the notification data.
 * @param length Length of the notification data.
 * @param isNotify  True if this is a notification, false otherwise.
 */
void
ATC_MiThermometer::notifyBatteryCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData,
                                         size_t length, bool isNotify) {
    if (length >= 1) {
        battery_level = pData[0];
        if (time_tracking) {
            last_read_time = time(nullptr);
        }
    } else {
        Serial.println("Received invalid battery level data");
    }
}

/**
 * @brief Connects to the command service. Prints an error message if the service is not found.
 */
void ATC_MiThermometer::connectToCommandService() {
    commandService = pClient->getService("1F10"); // Command Service UUID
    if (!commandService) {
        Serial.printf("Failed to find service %s\n", "1F10");
    }
}

/**
 * @brief Connects to the command characteristic.  Prints an error message if the characteristic is not found.
 */
void ATC_MiThermometer::connectToCommandCharacteristic() {
    if (!commandService) {
        connectToCommandService();
        if (!commandService) {
            return;
        }
    }
    commandCharacteristic = commandService->getCharacteristic("1F1F"); // Command characteristic UUID
    if (!commandCharacteristic) {
        Serial.printf("Failed to find characteristic %s\n", "1F1F");
    }
}

/**
 * @brief Reads the settings from the thermometer.  Connects to the device, subscribes to notifications
 * from the command characteristic, sends a read settings command (0x55), waits for the settings data,
 * then unsubscribes from notifications.  Prints error messages if connection or reading settings fails.
 */
void ATC_MiThermometer::readSettings() {
    std::lock_guard<std::mutex> lock(bleMutex);
    int attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (!commandService) {
        connectToCommandService();
        if (!commandService) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (!commandCharacteristic) {
        connectToCommandCharacteristic();
        if (!commandCharacteristic) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    received_settings = false;
    if (commandCharacteristic->canNotify()) {
        commandCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                const uint8_t *pData, size_t length, bool isNotify) {
                                             this->notifySettingsCallback(pBLERemoteCharacteristic, pData, length,
                                                                          isNotify);
                                         });
    } else {
        Serial.println("Command characteristic cannot notify");
        return;
    }
    delay(1000); // Delay to ensure connection is stable.
    std::vector<uint8_t> data = {0x55}; // Read settings command
    sendCommand(data);
    uint32_t start = millis();
    while (!received_settings && millis() - start < 5000) { // Timeout after 5 seconds
        delay(100);
        yield(); // Allow other tasks to run
    }
    if (!received_settings) {
        Serial.println("Failed to read settings");
    }
    commandCharacteristic->unsubscribe();
}

/**
 * @brief  Callback function for settings notifications.  Parses the settings data received from the thermometer
 *          and stores it in the settings member variable. Prints error messages if invalid data is received.
 * @param pBLERemoteCharacteristic  Pointer to the characteristic that triggered the notification.
 * @param pData  Pointer to the notification data.
 * @param length Length of the notification data.
 * @param isNotify  True if this is a notification, false otherwise.
 */
void
ATC_MiThermometer::notifySettingsCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData,
                                          size_t length, bool isNotify) {
    if (!pData || length == 0) {
        Serial.println("Received empty data in notifySettingsCallback");
        return;
    }
    read_settings = true;
    received_settings = true;
    if (length < 13) { // Check if data length is sufficient
        Serial.println("Invalid settings length");
        return;
    }
    if (length >= 13) { // Parse the settings data
        settings.lp_measures = (pData[2] & 0x80) != 0;
        settings.tx_measures = (pData[2] & 0x40) != 0;
        settings.show_battery = (pData[2] & 0x20) != 0;
        settings.temp_F_or_C = (pData[2] & 0x10) != 0;
        settings.blinking_time_smile = (pData[2] & 0x08) != 0;
        settings.comfort_smiley = (pData[2] & 0x04) != 0;
        settings.advertising_type = static_cast<Advertising_Type>(pData[2] & 0x03);
        settings.screen_off = (pData[3] & 0x80) != 0;
        settings.long_range = (pData[3] & 0x40) != 0;
        settings.bt5phy = (pData[3] & 0x20) != 0;
        settings.adv_flags = (pData[3] & 0x10) != 0;
        settings.adv_crypto = (pData[3] & 0x08) != 0;
        settings.smiley = static_cast<Smiley>(pData[3] & 0x07);
        settings.temp_offset = static_cast<float>(static_cast<int8_t>(pData[4])) / 10.0f;
        settings.humidity_offset = static_cast<float>(static_cast<int8_t>(pData[5])) / 10.0f;
        settings.advertising_interval = pData[6];
        settings.measure_interval = pData[7];
        settings.rfTxPower = static_cast<RF_TX_Power>(pData[8]);
        settings.connect_latency = pData[9];
        settings.lcd_update_interval = pData[10];
        settings.hw_version = static_cast<HW_VERSION_ID>(pData[11]);
        settings.averaging_measurements = pData[12];
    }
}

/**
 * @brief Sends a command to the thermometer.  Prints an error message if sending the command fails.
 * @param data  The command data to send.
 */
void ATC_MiThermometer::sendCommand(const std::vector<uint8_t> &data) {
    if (!commandCharacteristic) {
        connectToCommandCharacteristic();
        if (!commandCharacteristic) {
            Serial.println("Command characteristic not found, cannot send command");
            return;
        }
    }
    bool success = commandCharacteristic->writeValue(data.data(), data.size(), true); // Write value with response.
    if (!success) {
        Serial.println("Failed to send command");
    }
}

/**
 * @brief Disconnects from the thermometer and resets all service and characteristic pointers.
 */
void ATC_MiThermometer::disconnect() {
    std::lock_guard<std::mutex> lock(bleMutex);
    if (pClient && pClient->isConnected()) {
        pClient->disconnect();
    }
    pClient = nullptr;
    environmentService = nullptr;
    batteryService = nullptr;
    commandService = nullptr;
    temperatureCharacteristic = nullptr;
    temperaturePreciseCharacteristic = nullptr;
    humidityCharacteristic = nullptr;
    batteryCharacteristic = nullptr;
    commandCharacteristic = nullptr;
}

/**
 * @brief Connects to all available services (environment, battery, and command).
 */
void ATC_MiThermometer::connectToAllServices() {
    connectToEnvironmentService();
    connectToBatteryService();
    connectToCommandService();
}

/**
 * @brief Connects to all available characteristics (temperature, precise temperature, humidity, battery, and command).
 */
void ATC_MiThermometer::connectToAllCharacteristics() {
    connectToTemperatureCharacteristic();
    connectToTemperaturePreciseCharacteristic();
    connectToHumidityCharacteristic();
    connectToBatteryCharacteristic();
    connectToCommandCharacteristic();
}

/**
 * @brief Begins notifications for all available characteristics (temperature, precise temperature, humidity, and battery).
 */
void ATC_MiThermometer::beginNotify() {
    beginNotifyTemp();
    beginNotifyTempPrecise();
    beginNotifyHumidity();
    beginNotifyBattery();
}

/**
 * @brief Gets the temperature, handling different advertising types and connection modes.
 *  If in ADVERTISING mode, returns the temperature from advertising data.
 *  If in NOTIFICATION or CONNECTION mode, reads the temperature if notifications haven't been started.
 * @return The temperature in degrees Celsius.
 */
float ATC_MiThermometer::getTemperature() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return temperature;
        } else {
            return round(temperature_precise * 10.f) / 10.0f; // Round to one decimal place
        }
    } else {
        if (!started_notify_temp) {
            readTemperature();
        }
        return temperature;
    }
}

/**
 * @brief Reads the temperature from the temperature characteristic.  Uses a connection if necessary.
 *       Prints an error message if reading fails or insufficient data is received.
 */
void ATC_MiThermometer::readTemperature() {
    if (!temperatureCharacteristic) {
        connectToTemperatureCharacteristic();
        if (!temperatureCharacteristic) {
            Serial.println("Temperature characteristic not found, cannot read temperature");
            return;
        }
    }
    readCharacteristicValue(temperatureCharacteristic, [this](const std::string &value) {
        if (value.length() >= 2) {
            uint16_t temp = (value[1] << 8) | value[0];
            temperature = static_cast<float>(temp) / 10.0f;
            if (time_tracking) {
                last_read_time = time(nullptr);
            }
        } else {
            Serial.println("Failed to read temperature, insufficient data");
        }
    });
}

/**
 * @brief Gets the precise temperature, handling different advertising types and connection modes.
 * If in ADVERTISING mode, returns the precise temperature from advertising data.
 * If in NOTIFICATION or CONNECTION mode, reads the precise temperature if notifications haven't been started.
 * @return The precise temperature in degrees Celsius.
 */
float ATC_MiThermometer::getTemperaturePrecise() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return temperature;
        } else {
            return temperature_precise;
        }
    } else {
        if (!started_notify_temp_precise) {
            readTemperaturePrecise();
        }
        return temperature_precise;
    }
}

/**
 * @brief Reads the precise temperature from the precise temperature characteristic.  Uses a connection if necessary.
 *        Prints an error message if reading fails or insufficient data is received.
 */
void ATC_MiThermometer::readTemperaturePrecise() {
    if (!temperaturePreciseCharacteristic) {
        connectToTemperaturePreciseCharacteristic();
        if (!temperaturePreciseCharacteristic) {
            Serial.println("Precise temperature characteristic not found, cannot read precise temperature");
            return;
        }
    }
    readCharacteristicValue(temperaturePreciseCharacteristic, [this](const std::string &value) {
        if (value.length() >= 2) {
            uint16_t temp = (value[1] << 8) | value[0];
            temperature_precise = static_cast<float>(temp) / 100.0f;
            if (time_tracking) {
                last_read_time = time(nullptr);
            }
        } else {
            Serial.println("Failed to read precise temperature, insufficient data");
        }
    });
}

/**
 * @brief Gets the humidity. If in ADVERTISING mode, returns humidity from advertising data.
 * If in NOTIFICATION or CONNECTION mode, reads humidity if notifications haven't been started.
 * @return The humidity in percentage.
 */
float ATC_MiThermometer::getHumidity() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        return humidity;
    } else {
        if (!started_notify_humidity) {
            readHumidity();
        }
        return humidity;
    }
}

/**
 * @brief Reads the humidity from the humidity characteristic. Uses a connection if necessary.
 *       Prints an error message if reading fails or insufficient data is received.
 */
void ATC_MiThermometer::readHumidity() {
    if (!humidityCharacteristic) {
        connectToHumidityCharacteristic();
        if (!humidityCharacteristic) {
            Serial.println("Humidity characteristic not found, cannot read humidity");
            return;
        }
    }
    readCharacteristicValue(humidityCharacteristic, [this](const std::string &value) {
        if (value.length() >= 2) {
            uint16_t hum = (value[1] << 8) | value[0];
            humidity = static_cast<float>(hum) / 100.0f;
            if (time_tracking) {
                last_read_time = time(nullptr);
            }
        } else {
            Serial.println("Failed to read humidity, insufficient data");
        }
    });
}

/**
 * @brief Gets the battery level. If in ADVERTISING mode, returns the battery level from advertising data.
 * If in NOTIFICATION or CONNECTION mode, reads the battery level if notifications haven't been started.
 * @return The battery level in percentage.
 */
uint8_t ATC_MiThermometer::getBatteryLevel() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        return battery_level;
    } else {
        if (!started_notify_battery) {
            readBatteryLevel();
        }
        return battery_level;
    }
}

/**
 * @brief Reads the battery level from the battery characteristic. Uses a connection if necessary.
 *       Prints an error message if reading fails or insufficient data is received.
 */
void ATC_MiThermometer::readBatteryLevel() {
    if (!batteryCharacteristic) {
        connectToBatteryCharacteristic();
        if (!batteryCharacteristic) {
            Serial.println("Battery characteristic not found, cannot read battery level");
            return;
        }
    }
    readCharacteristicValue(batteryCharacteristic, [this](const std::string &value) {
        if (!value.empty()) {
            battery_level = static_cast<uint8_t>(value[0]);
            if (time_tracking) {
                last_read_time = time(nullptr);
            }
        } else {
            Serial.println("Failed to read battery level, insufficient data");
        }
    });
}

/**
 * @brief Gets the advertising type. Reads settings if they haven't been read yet.
 * @return The advertising type.
 */
Advertising_Type ATC_MiThermometer::getAdvertisingType() {
    if (!read_settings) {
        readSettings();
    }
    return settings.advertising_type;
}

/**
 * @brief Gets the MAC address of the thermometer.
 * @return The MAC address.
 */
const char *ATC_MiThermometer::getAddress() const {
    return address;
}

/**
 * @brief Parses the advertising data based on the advertising type.
 * Reads settings if they haven't been read yet. Disconnects if in ADVERTISING mode after reading settings.
 * @param data The advertising data.
 * @param length The length of the advertising data.
 */
void ATC_MiThermometer::parseAdvertisingData(const uint8_t *data, size_t length) {
    if (!read_settings) {
        readSettings();
        if (connection_mode == Connection_mode::ADVERTISING) {
            disconnect();
        }
        return;
    }
    switch (settings.advertising_type) {
        case Advertising_Type::BTHOME:
            parseAdvertisingDataBTHOME(data, length);
            break;
        case Advertising_Type::PVVX:
            parseAdvertisingDataPVVX(data, length);
            break;
        case Advertising_Type::ATC1441:
            parseAdvertisingDataATC1441(data, length);
            break;
        default:
            Serial.println("Unknown advertising type");
            break;
    }
}

/**
 * @brief Parses advertising data in ATC1441 format.
 * Extracts temperature, humidity, battery level, and battery voltage.
 * Prints an error message if the packet is too short.
 * @param data The advertising data.
 * @param length The length of the advertising data.
 */
void ATC_MiThermometer::parseAdvertisingDataATC1441(const uint8_t *data, size_t length) {
    if (length < 18) {
        Serial.println("Packet too short!");
        return;
    }
    int16_t temperatureRaw = (data[10] << 8) | data[11];
    temperature = static_cast<float>(temperatureRaw) * 0.1f;
    humidity = data[12];
    battery_level = data[13];
    battery_mv = (data[14] << 8) | data[15];
    if (time_tracking) {
        last_read_time = time(nullptr);
    }
}

/**
 * @brief Parses advertising data in PVVX format. Extracts precise temperature, humidity, battery voltage, and battery level.
 * Prints error messages if the packet is too short, has an incorrect size, or an incorrect UUID.
 * @param data The advertising data.
 * @param length The length of the advertising data.
 */
void ATC_MiThermometer::parseAdvertisingDataPVVX(const uint8_t *data, size_t length) {
    if (length < 19) {
        Serial.println("Packet too short!");
        return;
    }
    uint8_t size = data[0];
    if (size != 18) {
        Serial.println("Incorrect packet size!");
        return;
    }
    uint8_t uid = data[1];
    if (uid != 0x16) {
        Serial.println("Incorrect UID, not Service Data with 16-bit UUID!");
        return;
    }
    uint16_t uuid = data[2] | (data[3] << 8);
    if (uuid != 0x181A) {
        Serial.println("Incorrect UUID, not 0x181A!");
        return;
    }
    int16_t temperatureRaw = data[10] | (data[11] << 8);
    temperature_precise = static_cast<float>(temperatureRaw) * 0.01f;
    uint16_t humidityRaw = data[12] | (data[13] << 8);
    humidity = static_cast<float>(humidityRaw) * 0.01f;
    battery_mv = data[14] | (data[15] << 8);
    battery_level = data[16];
    if (time_tracking) {
        last_read_time = time(nullptr);
    }
}

/**
 * @brief Parses advertising data in BTHome format.  Extracts battery level, temperature, humidity, and voltage.
 * Prints error messages if the packet is too short, the AD element length exceeds the packet size,
 * the service data is too short, or the UUID is unknown.
 * @param data The advertising data.
 * @param length The length of the advertising data.
 */
void ATC_MiThermometer::parseAdvertisingDataBTHOME(const uint8_t *data, size_t length) {
    if (length < 6) {
        Serial.println("Packet too short!");
        return;
    }
    size_t index = 0;
    while (index < length) {
        uint8_t element_length = data[index];
        if (element_length == 0) {
            break;
        }
        if (index + 1 + element_length > length) {
            Serial.println("AD element length exceeds packet size!");
            break;
        }
        uint8_t ad_type = data[index + 1];
        const uint8_t *ad_data = &data[index + 2];
        uint8_t ad_data_length = element_length - 1;
        if (ad_type == 0x16) { // Service Data - 16-bit UUID
            if (ad_data_length < 3) {
                Serial.println("Service Data too short!");
                break;
            }
            uint16_t uuid = ad_data[0] | (ad_data[1] << 8);
            if (uuid != 0xFCD2) { // BTHome UUID
                Serial.println("Unknown UUID for BTHome!");
                break;
            }
            size_t dataIndex = 3;
            while (dataIndex < ad_data_length) {
                if (dataIndex >= ad_data_length) {
                    break;
                }
                uint8_t objectId = ad_data[dataIndex++];
                switch (objectId) {
                    case 0x00: { // Packet ID
                        if (dataIndex >= ad_data_length) {
                            Serial.println("Missing data for Packet ID!");
                            break;
                        }
                        uint8_t packetId = ad_data[dataIndex++];
                        break;
                    }
                    case 0x01: { // Battery Level
                        if (dataIndex >= ad_data_length) {
                            Serial.println("Missing data for Battery Level!");
                            break;
                        }
                        battery_level = ad_data[dataIndex++];
                        break;
                    }
                    case 0x02: { // Temperature
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Missing data for Temperature!");
                            break;
                        }
                        uint16_t temperatureRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        temperature_precise = static_cast<float>(temperatureRaw) * 0.01f;
                        dataIndex += 2;
                        break;
                    }
                    case 0x03: { // Humidity
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Missing data for Humidity!");
                            break;
                        }
                        uint16_t humidityRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        humidity = static_cast<float>(humidityRaw) * 0.01f;
                        dataIndex += 2;
                        break;
                    }
                    case 0x0C: { // Voltage
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Missing data for Voltage!");
                            break;
                        }
                        uint16_t voltageRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        battery_mv = voltageRaw;
                        dataIndex += 2;
                        break;
                    }
                    default:
                        dataIndex = ad_data_length; // Skip unknown object IDs
                        break;
                }
            }
        }
        index += 1 + element_length;
    }
    if (time_tracking) {
        last_read_time = time(nullptr);
    }
}

/**
 * @brief Initializes the thermometer based on the connection mode.
 * Connects to the device, reads settings, and disconnects if in ADVERTISING mode.
 * Subscribes to notifications if in NOTIFICATION mode. Reads data on demand if in CONNECTION mode.
 * Prints error messages if connection or settings reading fails.
 */
void ATC_MiThermometer::init() {
    int attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    attempts = 0;
    while (!read_settings && attempts < 5) {
        readSettings();
        attempts++;
        yield();
        if (!read_settings) {
            disconnect();
            environmentService = nullptr;
            batteryService = nullptr;
            commandService = nullptr;
            temperatureCharacteristic = nullptr;
            temperaturePreciseCharacteristic = nullptr;
            humidityCharacteristic = nullptr;
            batteryCharacteristic = nullptr;
            commandCharacteristic = nullptr;
        }
    }
    if (!read_settings) {
        Serial.println("Failed to read settings after multiple attempts");
        return;
    }
    if (connection_mode == Connection_mode::ADVERTISING) {
        disconnect();
        return;
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        connectToAllServices();
        connectToAllCharacteristics();
        beginNotify();
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readTemperature();
        readTemperaturePrecise();
        readHumidity();
        readBatteryLevel();
    }
}

/**
 * @brief Returns whether the settings have been successfully read from the device.
 * @return True if settings have been read, false otherwise.
 */
bool ATC_MiThermometer::getReadSettings() const {
    return read_settings;
}

/**
 * @brief  Gets the battery voltage. If in ADVERTISING mode, returns the parsed battery voltage.
 *          If in CONNECTION or NOTIFICATION mode, estimates the voltage based on the battery level.
 * @return The battery voltage in millivolts.
 */
uint16_t ATC_MiThermometer::getBatteryVoltage() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        return battery_mv;
    } else {
        if (!started_notify_battery) {
            readBatteryLevel();
        }
        // Estimate voltage based on battery percentage (assuming a linear relationship between 2000mV and 3000mV)
        return 2000 + (battery_level * (3000 - 2000) / 100);
    }
}

/**
 * @brief Gets the RF TX Power setting from the device. Reads the settings if they haven't been read already.
 * @return The RF TX Power as an RF_TX_Power enum.
 */
RF_TX_Power ATC_MiThermometer::getRfTxPower() {
    if (!read_settings) {
        readSettings();
    }
    return settings.rfTxPower;
}

/**
 * @brief  Gets the Low Power Measures setting from the device. Reads the settings if they haven't been read already.
 * @return  True if Low Power Measures are enabled, false otherwise.
 */
bool ATC_MiThermometer::getLowPowerMeasures() {
    if (!read_settings) {
        readSettings();
    }
    return settings.lp_measures;
}

/**
 * @brief Gets the Transmit Measures setting from the device. Reads the settings if they haven't been read already.
 * @return  True if Transmit Measures are enabled, false otherwise.
 */
bool ATC_MiThermometer::getTransmitMeasures() {
    if (!read_settings) {
        readSettings();
    }
    return settings.tx_measures;
}

/**
 * @brief Gets the Show Battery setting from the device. Reads the settings if they haven't been read already.
 * @return True if Show Battery is enabled, false otherwise.
 */
bool ATC_MiThermometer::getShowBattery() {
    if (!read_settings) {
        readSettings();
    }
    return settings.show_battery;
}

/**
 * @brief Gets the temperature unit setting (Fahrenheit or Celsius). Reads the settings if they haven't been read already.
 * @return  True if set to Fahrenheit, false if set to Celsius.
 */
bool ATC_MiThermometer::getTempFOrC() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_F_or_C;
}

/**
 * @brief Gets the Blinking Time Smile setting. Reads the settings if they haven't been read already.
 * @return True if Blinking Time Smile is enabled, false otherwise.
 */
bool ATC_MiThermometer::getBlinkingTimeSmile() {
    if (!read_settings) {
        readSettings();
    }
    return settings.blinking_time_smile;
}

/**
 * @brief Gets the Comfort Smiley setting. Reads the settings if they haven't been read already.
 * @return True if Comfort Smiley is enabled, false otherwise.
 */
bool ATC_MiThermometer::getComfortSmiley() {
    if (!read_settings) {
        readSettings();
    }
    return settings.comfort_smiley;
}

/**
 * @brief Gets the Adv Crypto setting. Reads the settings if they haven't been read already.
 * @return True if Adv Crypto is enabled, false otherwise.
 */
bool ATC_MiThermometer::getAdvCrypto() {
    if (!read_settings) {
        readSettings();
    }
    return settings.adv_crypto;
}

/**
 * @brief Gets the Adv Flags setting. Reads the settings if they haven't been read already.
 * @return True if Adv Flags is enabled, false otherwise.
 */
bool ATC_MiThermometer::getAdvFlags() {
    if (!read_settings) {
        readSettings();
    }
    return settings.adv_flags;
}

/**
 * @brief Gets the current Smiley setting. Reads the settings if they haven't been read already.
 * @return The current Smiley as a Smiley enum.
 */
Smiley ATC_MiThermometer::getSmiley() {
    if (!read_settings) {
        readSettings();
    }
    return settings.smiley;
}

/**
 * @brief  Gets the BT5 PHY setting. Reads the settings if they haven't been read already.
 * @return True if BT5 PHY is enabled, false otherwise.
 */
bool ATC_MiThermometer::getBT5PHY() {
    if (!read_settings) {
        readSettings();
    }
    return settings.bt5phy;
}

/**
 * @brief Gets the Long Range setting.  Reads the settings if they haven't been read already.
 * @return  True if Long Range is enabled, false otherwise.
 */
bool ATC_MiThermometer::getLongRange() {
    if (!read_settings) {
        readSettings();
    }
    return settings.long_range;
}

/**
 * @brief  Gets the Screen Off setting.  Reads the settings if they haven't been read already.
 * @return True if Screen Off is enabled, false otherwise.
 */
bool ATC_MiThermometer::getScreenOff() {
    if (!read_settings) {
        readSettings();
    }
    return settings.screen_off;
}

/**
 * @brief  Gets the temperature offset. Reads the settings if they haven't been read already.
 * @return  The temperature offset.
 */
float ATC_MiThermometer::getTempOffset() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_offset;
}

/**
 * @brief Gets the humidity offset. Reads the settings if they haven't been read already.
 * @return  The humidity offset.
 */
float ATC_MiThermometer::getHumidityOffset() {
    if (!read_settings) {
        readSettings();
    }
    return settings.humidity_offset;
}

/**
 * @brief Gets the calibrated temperature offset.  Reads the settings if they haven't been read already.
 * @return The calibrated temperature offset.
 */
int8_t ATC_MiThermometer::getTempOffsetCal() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_offset_cal;
}

/**
 * @brief  Gets the calibrated humidity offset.  Reads the settings if they haven't been read already.
 * @return  The calibrated humidity offset.
 */
int8_t ATC_MiThermometer::getHumidityOffsetCal() {
    if (!read_settings) {
        readSettings();
    }
    return settings.humidity_offset_cal;
}

/**
 * @brief Gets the advertising interval in steps. Reads the settings if they haven't been read already.
 * @return The advertising interval in steps.
 */
uint8_t ATC_MiThermometer::getAdvertisingIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.advertising_interval;
}

/**
 * @brief Gets the measure interval in steps. Reads the settings if they haven't been read already.
 * @return The measure interval in steps.
 */
uint8_t ATC_MiThermometer::getMeasureIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.measure_interval;
}

/**
 * @brief  Gets the connect latency in steps.  Reads the settings if they haven't been read already.
 * @return The connect latency in steps.
 */
uint8_t ATC_MiThermometer::getConnectLatencySteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.connect_latency;
}

/**
 * @brief Gets the LCD update interval in steps. Reads the settings if they haven't been read already.
 * @return The LCD update interval in steps.
 */
uint8_t ATC_MiThermometer::getLcdUpdateIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.lcd_update_interval;
}

/**
 * @brief Gets the hardware version ID.  Reads the settings if they haven't been read already.
 * @return The HW_VERSION_ID enum representing the hardware version.
 */
HW_VERSION_ID ATC_MiThermometer::getHwVersion() {
    if (!read_settings) {
        readSettings();
    }
    return settings.hw_version;
}

/**
 * @brief Gets the averaging measurements setting in steps.  Reads the settings if they haven't been read already.
 * @return The averaging measurements setting in steps.
 */
uint8_t ATC_MiThermometer::getAveragingMeasurementsSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.averaging_measurements;
}

/**
 * @brief Gets the RF TX Power in dBm. Uses a map to convert from the RF_TX_Power enum to a float value.
 * @return The RF TX Power in dBm.  Returns 0.0f if the power level is not found in the map.
 */
float ATC_MiThermometer::getRfTxPowerdBm() {
    static const std::map<RF_TX_Power, float> powerMap = {
            {RF_TX_Power::dBm_3_01,   3.01f},
            {RF_TX_Power::dBm_2_81,   2.81f},
            {RF_TX_Power::dBm_2_61,   2.61f},
            {RF_TX_Power::dBm_2_39,   2.39f},
            {RF_TX_Power::dBm_1_99,   1.99f},
            {RF_TX_Power::dBm_1_73,   1.73f},
            {RF_TX_Power::dBm_1_45,   1.45f},
            {RF_TX_Power::dBm_1_17,   1.17f},
            {RF_TX_Power::dBm_0_90,   0.90f},
            {RF_TX_Power::dBm_0_58,   0.58f},
            {RF_TX_Power::dBm_0_04,   0.04f},
            {RF_TX_Power::dBm_n0_14,  -0.14f},
            {RF_TX_Power::dBm_n0_97,  -0.97f},
            {RF_TX_Power::dBm_n1_42,  -1.42f},
            {RF_TX_Power::dBm_n1_89,  -1.89f},
            {RF_TX_Power::dBm_n2_48,  -2.48f},
            {RF_TX_Power::dBm_n3_03,  -3.03f},
            {RF_TX_Power::dBm_n3_61,  -3.61f},
            {RF_TX_Power::dBm_n4_26,  -4.26f},
            {RF_TX_Power::dBm_n5_03,  -5.03f},
            {RF_TX_Power::dBm_n5_81,  -5.81f},
            {RF_TX_Power::dBm_n6_67,  -6.67f},
            {RF_TX_Power::dBm_n7_65,  -7.65f},
            {RF_TX_Power::dBm_n8_65,  -8.65f},
            {RF_TX_Power::dBm_n9_89,  -9.89f},
            {RF_TX_Power::dBm_n11_4,  -11.4f},
            {RF_TX_Power::dBm_n13_29, -13.29f},
            {RF_TX_Power::dBm_n15_88, -15.88f},
            {RF_TX_Power::dBm_n19_27, -19.27f},
            {RF_TX_Power::dBm_n25_18, -25.18f},
            {RF_TX_Power::dBm_n30,    -30.0f},
            {RF_TX_Power::dBm_n50,    -50.0f},
            {RF_TX_Power::dBm_10_46,  10.46f},
            {RF_TX_Power::dBm_10_29,  10.29f},
            {RF_TX_Power::dBm_10_01,  10.01f},
            {RF_TX_Power::dBm_9_81,   9.81f},
            {RF_TX_Power::dBm_9_48,   9.48f},
            {RF_TX_Power::dBm_9_24,   9.24f},
            {RF_TX_Power::dBm_8_97,   8.97f},
            {RF_TX_Power::dBm_8_73,   8.73f},
            {RF_TX_Power::dBm_8_44,   8.44f},
            {RF_TX_Power::dBm_8_13,   8.13f},
            {RF_TX_Power::dBm_7_79,   7.79f},
            {RF_TX_Power::dBm_7_41,   7.41f},
            {RF_TX_Power::dBm_7_02,   7.02f},
            {RF_TX_Power::dBm_6_60,   6.60f},
            {RF_TX_Power::dBm_6_14,   6.14f},
            {RF_TX_Power::dBm_5_65,   5.65f},
            {RF_TX_Power::dBm_5_13,   5.13f},
            {RF_TX_Power::dBm_4_57,   4.57f},
            {RF_TX_Power::dBm_3_94,   3.94f},
            {RF_TX_Power::dBm_3_23,   3.23f}
    };
    auto it = powerMap.find(getRfTxPower());
    if (it != powerMap.end()) {
        return it->second;
    }
    return 0.0f;
}

/**
 * @brief  Gets the advertising interval in milliseconds.
 * @return The advertising interval in milliseconds.
 */
uint16_t ATC_MiThermometer::getAdvertisingIntervalMs() {
    return static_cast<uint16_t>(static_cast<float>(getAdvertisingIntervalSteps()) * advertising_interval_step_time_ms);
}

/**
 * @brief Gets the measurement interval in milliseconds.
 * @return The measurement interval in milliseconds.
 */
uint32_t ATC_MiThermometer::getMeasureIntervalMs() {
    return getMeasureIntervalSteps() * getAdvertisingIntervalMs();
}

/**
 * @brief Gets the connection latency in milliseconds.
 * @return The connection latency in milliseconds.
 */
uint16_t ATC_MiThermometer::getConnectLatencyMs() {
    return getConnectLatencySteps() * connect_latency_step_time_ms;
}

/**
 * @brief  Gets the LCD update interval in milliseconds.
 * @return  The LCD update interval in milliseconds.
 */
uint16_t ATC_MiThermometer::getLcdUpdateIntervalMs() {
    return getLcdUpdateIntervalSteps() * lcd_update_interval_step_time_ms;
}

/**
 * @brief Gets the averaging measurement time in milliseconds.
 * @return  The averaging measurement time in milliseconds.
 */
uint32_t ATC_MiThermometer::getAveragingMeasurementsMs() {
    return getMeasureIntervalMs() * getAveragingMeasurementsSteps();
}

/**
 * @brief  Gets the averaging measurement time in seconds.
 * @return The averaging measurement time in seconds.
 */
uint16_t ATC_MiThermometer::getAveragingMeasurementsSec() {
    return static_cast<uint16_t>(getAveragingMeasurementsMs() / 1000);
}

/**
 * @brief  Parses the provided settings struct into a byte vector that can be sent to the device as a command.
 * @param settingsToParse The settings struct to parse.
 * @return A byte vector representing the settings, ready to be sent to the device.
 */
std::vector<uint8_t> ATC_MiThermometer::parseSettings(const ATC_MiThermometer_Settings &settingsToParse) {
    std::vector<uint8_t> data(12);
    data[0] = 0x55; // Command header
    data[1] = 0x0A; // Command length
    data[2] = (settingsToParse.lp_measures << 7) | (settingsToParse.tx_measures << 6) |
              (settingsToParse.show_battery << 5) |
              (settingsToParse.temp_F_or_C << 4) | (settingsToParse.blinking_time_smile << 3) |
              (settingsToParse.comfort_smiley << 2) |
              static_cast<uint8_t>(settingsToParse.advertising_type);
    data[3] = (settingsToParse.screen_off << 7) | (settingsToParse.long_range << 6) | (settingsToParse.bt5phy << 5) |
              (settingsToParse.adv_flags << 4) |
              (settingsToParse.adv_crypto << 3) | static_cast<uint8_t>(settingsToParse.smiley);
    data[4] = static_cast<uint8_t>(settingsToParse.temp_offset * 10);
    data[5] = static_cast<uint8_t>(settingsToParse.humidity_offset * 10);
    data[6] = settingsToParse.advertising_interval;
    data[7] = settingsToParse.measure_interval;
    data[8] = static_cast<uint8_t>(settingsToParse.rfTxPower);
    data[9] = settingsToParse.connect_latency;
    data[10] = settingsToParse.lcd_update_interval;
    data[11] = settingsToParse.averaging_measurements;
    return data;
}

/**
 * @brief Sends the given settings to the thermometer. Connects to the device,
 * subscribes to notifications, sends the settings command, waits for confirmation, and then unsubscribes.
 * Prints error messages if connection or settings sending fails.
 * @param newSettings The new settings to apply to the thermometer.
 */
void ATC_MiThermometer::sendSettings(const ATC_MiThermometer_Settings &newSettings) {
    std::lock_guard<std::mutex> lock(bleMutex);
    int attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (!commandService) {
        connectToCommandService();
        if (!commandService) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (!commandCharacteristic) {
        connectToCommandCharacteristic();
        if (!commandCharacteristic) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    received_settings = false;
    if (commandCharacteristic->canNotify()) {
        commandCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                const uint8_t *pData, size_t length, bool isNotify) {
                                             this->notifySettingsCallback(pBLERemoteCharacteristic, pData, length,
                                                                          isNotify);
                                         });
    } else {
        Serial.println("Command characteristic cannot notify");
        return;
    }
    std::vector<uint8_t> data = parseSettings(newSettings);
    sendCommand(data);
    uint32_t start = millis();
    while (!received_settings && millis() - start < 5000) {
        delay(100);
        yield();
    }
    if (!received_settings) {
        Serial.println("Failed to send settings");
    }
    commandCharacteristic->unsubscribe();
}

/**
 * @brief Sets the RF TX Power.
 * @param power The RF TX power to set (as an RF_TX_Power enum).
 */
void ATC_MiThermometer::setRfTxPower(RF_TX_Power power) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.rfTxPower = power;
    sendSettings(newSettings);
}

/**
 * @brief  Gets the current settings of the thermometer.
 * @return The current settings as an ATC_MiThermometer_Settings struct.
 */
ATC_MiThermometer_Settings ATC_MiThermometer::getSettings() {
    if (!read_settings) {
        readSettings();
    }
    return settings;
}

/**
 * @brief  Sets the RF TX Power in dBm. Finds the closest matching enum value and sets it.
 * @param power  The desired RF TX Power in dBm (as a float).
 */
void ATC_MiThermometer::setRfTxPowerdBm(float power) {
    static const std::map<float, RF_TX_Power> powerMap = {
            {3.01f,   RF_TX_Power::dBm_3_01},
            {2.81f,   RF_TX_Power::dBm_2_81},
            {2.61f,   RF_TX_Power::dBm_2_61},
            {2.39f,   RF_TX_Power::dBm_2_39},
            {1.99f,   RF_TX_Power::dBm_1_99},
            {1.73f,   RF_TX_Power::dBm_1_73},
            {1.45f,   RF_TX_Power::dBm_1_45},
            {1.17f,   RF_TX_Power::dBm_1_17},
            {0.90f,   RF_TX_Power::dBm_0_90},
            {0.58f,   RF_TX_Power::dBm_0_58},
            {0.04f,   RF_TX_Power::dBm_0_04},
            {-0.14f,  RF_TX_Power::dBm_n0_14},
            {-0.97f,  RF_TX_Power::dBm_n0_97},
            {-1.42f,  RF_TX_Power::dBm_n1_42},
            {-1.89f,  RF_TX_Power::dBm_n1_89},
            {-2.48f,  RF_TX_Power::dBm_n2_48},
            {-3.03f,  RF_TX_Power::dBm_n3_03},
            {-3.61f,  RF_TX_Power::dBm_n3_61},
            {-4.26f,  RF_TX_Power::dBm_n4_26},
            {-5.03f,  RF_TX_Power::dBm_n5_03},
            {-5.81f,  RF_TX_Power::dBm_n5_81},
            {-6.67f,  RF_TX_Power::dBm_n6_67},
            {-7.65f,  RF_TX_Power::dBm_n7_65},
            {-8.65f,  RF_TX_Power::dBm_n8_65},
            {-9.89f,  RF_TX_Power::dBm_n9_89},
            {-11.4f,  RF_TX_Power::dBm_n11_4},
            {-13.29f, RF_TX_Power::dBm_n13_29},
            {-15.88f, RF_TX_Power::dBm_n15_88},
            {-19.27f, RF_TX_Power::dBm_n19_27},
            {-25.18f, RF_TX_Power::dBm_n25_18},
            {-30.0f,  RF_TX_Power::dBm_n30},
            {-50.0f,  RF_TX_Power::dBm_n50},
            {10.46f,  RF_TX_Power::dBm_10_46},
            {10.29f,  RF_TX_Power::dBm_10_29},
            {10.01f,  RF_TX_Power::dBm_10_01},
            {9.81f,   RF_TX_Power::dBm_9_81},
            {9.48f,   RF_TX_Power::dBm_9_48},
            {9.24f,   RF_TX_Power::dBm_9_24},
            {8.97f,   RF_TX_Power::dBm_8_97},
            {8.73f,   RF_TX_Power::dBm_8_73},
            {8.44f,   RF_TX_Power::dBm_8_44},
            {8.13f,   RF_TX_Power::dBm_8_13},
            {7.79f,   RF_TX_Power::dBm_7_79},
            {7.41f,   RF_TX_Power::dBm_7_41},
            {7.02f,   RF_TX_Power::dBm_7_02},
            {6.60f,   RF_TX_Power::dBm_6_60},
            {6.14f,   RF_TX_Power::dBm_6_14},
            {5.65f,   RF_TX_Power::dBm_5_65},
            {5.13f,   RF_TX_Power::dBm_5_13},
            {4.57f,   RF_TX_Power::dBm_4_57},
            {3.94f,   RF_TX_Power::dBm_3_94},
            {3.23f,   RF_TX_Power::dBm_3_23}
    };
    auto closest = std::min_element(powerMap.begin(), powerMap.end(),
                                    [power](const std::pair<float, RF_TX_Power> &a,
                                            const std::pair<float, RF_TX_Power> &b) {
                                        return std::abs(a.first - power) < std::abs(b.first - power);
                                    });
    setRfTxPower(closest->second);
}

/**
 * @brief Sets the Low Power Measures setting.
 * @param lowPowerMeasures True to enable low power measures, false to disable.
 */
void ATC_MiThermometer::setLowPowerMeasures(bool lowPowerMeasures) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.lp_measures = lowPowerMeasures;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Transmit Measures setting.
 * @param transmitMeasures True to enable transmit measures, false to disable.
 */
void ATC_MiThermometer::setTransmitMeasures(bool transmitMeasures) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.tx_measures = transmitMeasures;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Show Battery setting.
 * @param showBattery True to show battery level, false to hide.
 */
void ATC_MiThermometer::setShowBattery(bool showBattery) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.show_battery = showBattery;
    sendSettings(newSettings);
}

/**
 * @brief Sets the temperature unit.
 * @param tempFOrC True for Fahrenheit, false for Celsius.
 */
void ATC_MiThermometer::setTempFOrC(bool tempFOrC) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_F_or_C = tempFOrC;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Blinking Time Smile setting.
 * @param blinkingTimeSmile True to enable blinking time smile, false to disable.
 */
void ATC_MiThermometer::setBlinkingTimeSmile(bool blinkingTimeSmile) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.blinking_time_smile = blinkingTimeSmile;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Comfort Smiley setting.
 * @param comfortSmiley True to enable comfort smiley, false to disable.
 */
void ATC_MiThermometer::setComfortSmiley(bool comfortSmiley) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.comfort_smiley = comfortSmiley;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Adv Crypto setting.
 * @param advCrypto True to enable Adv Crypto, false to disable.
 */
void ATC_MiThermometer::setAdvCrypto(bool advCrypto) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.adv_crypto = advCrypto;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Adv Flags setting.
 * @param advFlags True to enable Adv Flags, false to disable.
 */
void ATC_MiThermometer::setAdvFlags(bool advFlags) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.adv_flags = advFlags;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Smiley setting.
 * @param smiley The Smiley to set (as a Smiley enum).
 */
void ATC_MiThermometer::setSmiley(Smiley smiley) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.smiley = smiley;
    sendSettings(newSettings);
}

/**
 * @brief Sets the BT5 PHY setting.
 * @param BT5PHY True to enable BT5 PHY, false to disable.
 */
void ATC_MiThermometer::setBT5PHY(bool BT5PHY) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.bt5phy = BT5PHY;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Long Range setting.
 * @param longRange True to enable long range, false to disable.
 */
void ATC_MiThermometer::setLongRange(bool longRange) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.long_range = longRange;
    sendSettings(newSettings);
}

/**
 * @brief Sets the Screen Off setting.
 * @param screenOff True to turn the screen off, false to turn it on.
 */
void ATC_MiThermometer::setScreenOff(bool screenOff) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.screen_off = screenOff;
    sendSettings(newSettings);
}

/**
 * @brief Sets the temperature offset.
 * @param tempOffset The temperature offset to set.
 */
void ATC_MiThermometer::setTempOffset(float tempOffset) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_offset = tempOffset;
    sendSettings(newSettings);
}

/**
 * @brief Sets the humidity offset.
 * @param humidityOffset The humidity offset to set.
 */
void ATC_MiThermometer::setHumidityOffset(float humidityOffset) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.humidity_offset = humidityOffset;
    sendSettings(newSettings);
}

/**
 * @brief Sets the calibrated temperature offset.
 * @param tempOffsetCal The calibrated temperature offset to set.
 */
void ATC_MiThermometer::setTempOffsetCal(int8_t tempOffsetCal) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_offset_cal = tempOffsetCal;
    sendSettings(newSettings);
}

/**
 * @brief Sets the calibrated humidity offset.
 * @param humidityOffsetCal The calibrated humidity offset to set.
 */
void ATC_MiThermometer::setHumidityOffsetCal(int8_t humidityOffsetCal) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.humidity_offset_cal = humidityOffsetCal;
    sendSettings(newSettings);
}

/**
 * @brief Sets the advertising interval in steps.
 * @param advertisingIntervalSteps The advertising interval in steps to set.
 */
void ATC_MiThermometer::setAdvertisingIntervalSteps(uint8_t advertisingIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.advertising_interval = advertisingIntervalSteps;
    sendSettings(newSettings);
}

/**
 * @brief Sets the advertising interval in milliseconds.
 * @param advertisingIntervalMs The advertising interval in milliseconds to set.
 */
void ATC_MiThermometer::setAdvertisingIntervalMs(uint16_t advertisingIntervalMs) {
    setAdvertisingIntervalSteps(
            static_cast<uint8_t>(static_cast<float>(advertisingIntervalMs) / advertising_interval_step_time_ms));
}

/**
 * @brief Sets the measure interval in milliseconds.
 * @param measureIntervalMs The measure interval in milliseconds to set.
 */
void ATC_MiThermometer::setMeasureIntervalMs(uint32_t measureIntervalMs) {
    setMeasureIntervalSteps(static_cast<uint8_t>(measureIntervalMs / getAdvertisingIntervalMs()));
}

/**
 * @brief Sets the measure interval in steps.
 * @param measureIntervalSteps The measure interval in steps to set.
 */
void ATC_MiThermometer::setMeasureIntervalSteps(uint8_t measureIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.measure_interval = measureIntervalSteps;
    sendSettings(newSettings);
}

/**
 * @brief Sets the connect latency in steps.
 * @param connectLatencySteps The connect latency in steps to set.
 */
void ATC_MiThermometer::setConnectLatencySteps(uint8_t connectLatencySteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.connect_latency = connectLatencySteps;
    sendSettings(newSettings);
}

/**
 * @brief Sets the connect latency in milliseconds.
 * @param connectLatencyMs The connect latency in milliseconds to set.
 */
void ATC_MiThermometer::setConnectLatencyMs(uint16_t connectLatencyMs) {
    setConnectLatencySteps(static_cast<uint8_t>(connectLatencyMs / connect_latency_step_time_ms));
}

/**
 * @brief Sets the LCD update interval in steps.
 * @param lcdUpdateIntervalSteps The LCD update interval in steps to set.
 */
void ATC_MiThermometer::setLcdUpdateIntervalSteps(uint8_t lcdUpdateIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.lcd_update_interval = lcdUpdateIntervalSteps;
    sendSettings(newSettings);
}

/**
 * @brief Sets the LCD update interval in milliseconds.
 * @param lcdUpdateIntervalMs The LCD update interval in milliseconds to set.
 */
void ATC_MiThermometer::setLcdUpdateIntervalMs(uint16_t lcdUpdateIntervalMs) {
    setLcdUpdateIntervalSteps(static_cast<uint8_t>(lcdUpdateIntervalMs / lcd_update_interval_step_time_ms));
}

/**
 * @brief Sets the number of averaging measurements in steps.
 * @param averagingMeasurementsSteps The number of averaging measurements in steps to set.
 */
void ATC_MiThermometer::setAveragingMeasurementsSteps(uint8_t averagingMeasurementsSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.averaging_measurements = averagingMeasurementsSteps;
    sendSettings(newSettings);
}

/**
 * @brief Sets the number of averaging measurements in milliseconds.
 * @param averagingMeasurementsMs The number of averaging measurements in milliseconds to set.
 */
void ATC_MiThermometer::setAveragingMeasurementsMs(uint32_t averagingMeasurementsMs) {
    setAveragingMeasurementsSteps(static_cast<uint8_t>(averagingMeasurementsMs / getMeasureIntervalMs()));
}

/**
 * @brief Sets the number of averaging measurements in seconds.
 * @param averagingMeasurementsSec The number of averaging measurements in seconds to set.
 */
void ATC_MiThermometer::setAveragingMeasurementsSec(uint16_t averagingMeasurementsSec) {
    setAveragingMeasurementsMs(averagingMeasurementsSec * 1000);
}

/**
 * @brief Resets the thermometer's settings to their default values.  Sends the reset command (0x56) to the device.
 *        Resets the internal flags for read_settings and received_settings and then reads the settings again.
 */
void ATC_MiThermometer::resetSettings() {
    std::vector<uint8_t> data = {0x56}; // Reset settings command.
    sendCommand(data);
    read_settings = false;
    received_settings = false;
    readSettings();
}

/**
 * @brief Sets the clock on the thermometer using a time_t value.  Connects to the device, sends the set clock command
 * and the time data.  Prints error messages if connection or command sending fails.
 * @param time  The time to set, as a time_t value.
 */
void ATC_MiThermometer::setClock(time_t time) {
    std::lock_guard<std::mutex> lock(bleMutex);
    int attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (!commandService) {
        connectToCommandService();
        if (!commandService) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (!commandCharacteristic) {
        connectToCommandCharacteristic();
        if (!commandCharacteristic) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    std::vector<uint8_t> data(5);
    data[0] = 0x23; // Set clock command
    data[1] = static_cast<uint8_t>(time & 0xFF);
    data[2] = static_cast<uint8_t>((time >> 8) & 0xFF);
    data[3] = static_cast<uint8_t>((time >> 16) & 0xFF);
    data[4] = static_cast<uint8_t>((time >> 24) & 0xFF);
    sendCommand(data);
}

/**
 * @brief Sets the clock on the thermometer. Converts the provided time components to a time_t value and calls the
 * overloaded setClock function.
 * @param hours The hour to set (0-23).
 * @param minutes The minute to set (0-59).
 * @param seconds The second to set (0-59).
 * @param day The day of the month to set (1-31).
 * @param month The month to set (1-12).
 * @param year The year to set (e.g., 2024).
 */
void ATC_MiThermometer::setClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month,
                                 uint16_t year) {
    tm timeStruct{};
    timeStruct.tm_hour = hours;
    timeStruct.tm_min = minutes;
    timeStruct.tm_sec = seconds;
    timeStruct.tm_mday = day;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_year = year - 1900;
    setClock(mktime(&timeStruct));
}

/**
 * @brief Gets the current connection mode.
 * @return The current connection mode as a Connection_mode enum.
 */
Connection_mode ATC_MiThermometer::getConnectionMode() const {
    return connection_mode;
}

/**
 * @brief Sets the connection mode, managing connections, notifications, and data reads as needed.
 * @param new_connection_mode The new connection mode to set.
 */
void ATC_MiThermometer::setConnectionMode(Connection_mode new_connection_mode) {
    if (connection_mode == new_connection_mode) {
        return;
    }
    if (connection_mode == Connection_mode::ADVERTISING) {
        connect();
        if (new_connection_mode == Connection_mode::NOTIFICATION) {
            beginNotify();
        } else if (new_connection_mode == Connection_mode::CONNECTION) {
            connect();
            readBatteryLevel();
            readHumidity();
            readTemperature();
            readTemperaturePrecise();
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        stopNotify();
        if (new_connection_mode == Connection_mode::ADVERTISING) {
            disconnect();
        } else if (new_connection_mode == Connection_mode::CONNECTION) {
            connect();
            readBatteryLevel();
            readHumidity();
            readTemperature();
            readTemperaturePrecise();
        }
    } else if (connection_mode == Connection_mode::CONNECTION) {
        if (new_connection_mode == Connection_mode::ADVERTISING) {
            disconnect();
        } else if (new_connection_mode == Connection_mode::NOTIFICATION) {
            beginNotify();
        }
    }
    connection_mode = new_connection_mode;
}

/**
 * @brief Stops temperature notifications. Unsubscribes from the temperature characteristic's notifications.
 */
void ATC_MiThermometer::stopNotifyTemp() {
    if (temperatureCharacteristic) {
        temperatureCharacteristic->unsubscribe();
        started_notify_temp = false;
    }
}

/**
 * @brief Stops precise temperature notifications. Unsubscribes from the precise temperature characteristic's notifications.
 */
void ATC_MiThermometer::stopNotifyTempPrecise() {
    if (temperaturePreciseCharacteristic) {
        temperaturePreciseCharacteristic->unsubscribe();
        started_notify_temp_precise = false;
    }
}

/**
 * @brief  Stops humidity notifications. Unsubscribes from the humidity characteristic's notifications.
 */
void ATC_MiThermometer::stopNotifyHumidity() {
    if (humidityCharacteristic) {
        humidityCharacteristic->unsubscribe();
        started_notify_humidity = false;
    }
}

/**
 * @brief Stops battery level notifications.  Unsubscribes from the battery level characteristic's notifications.
 */
void ATC_MiThermometer::stopNotifyBattery() {
    if (batteryCharacteristic) {
        batteryCharacteristic->unsubscribe();
        started_notify_battery = false;
    }
}

/**
 * @brief  Stops all notifications. Unsubscribes from all characteristic notifications.
 */
void ATC_MiThermometer::stopNotify() {
    stopNotifyTemp();
    stopNotifyTempPrecise();
    stopNotifyHumidity();
    stopNotifyBattery();
}

/**
 * @brief Reads the value of the specified characteristic and passes it to the callback function.
 * Prints an error if the characteristic is null.
 * @param characteristic A pointer to the characteristic to read.
 * @param callback The function to call with the read value (as a string).
 */
void ATC_MiThermometer::readCharacteristicValue(NimBLERemoteCharacteristic *characteristic,
                                                std::function<void(const std::string &)> callback) {
    if (!characteristic) {
        Serial.println("Characteristic is null, cannot read value");
        return;
    }
    std::string value = characteristic->readValue();
    callback(value);
}

bool ATC_MiThermometer::getTimeTracking() const {
    return time_tracking;
}

void ATC_MiThermometer::setTimeTracking(bool timeTracking) {
    time_tracking = timeTracking;
}

time_t ATC_MiThermometer::getLastReadTime() const {
    return last_read_time;
}
