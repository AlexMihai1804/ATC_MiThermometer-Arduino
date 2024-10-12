#include "ATC_MiThermometer.h"
#include <cmath>

ATC_MiThermometer::ATC_MiThermometer(const char *address, Connection_mode connection_mode)
        : address(address), pClient(nullptr), environmentService(nullptr), connection_mode(connection_mode),
          batteryService(nullptr), commandService(nullptr), temperatureCharacteristic(nullptr),
          temperaturePreciseCharacteristic(nullptr), humidityCharacteristic(nullptr), batteryCharacteristic(nullptr),
          commandCharacteristic(nullptr), received_settings(false), read_settings(false), started_notify_temp(false),
          started_notify_temp_precise(false), started_notify_humidity(false), started_notify_battery(false),
          temperature(0), temperature_precise(0), humidity(0), battery_mv(0), battery_level(0) {
}

void ATC_MiThermometer::connect() {
    if (pClient != nullptr) {
        if (pClient->isConnected()) {
            pClient->disconnect();
        }
        NimBLEDevice::deleteClient(pClient);
        pClient = nullptr;
    }
    pClient = NimBLEDevice::createClient();
    if (pClient == nullptr) {
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

bool ATC_MiThermometer::isConnected() {
    return pClient && pClient->isConnected();
}

void ATC_MiThermometer::connect_to_environment_service() {
    environmentService = pClient->getService("181A");
    if (environmentService == nullptr) {
        Serial.printf("Failed to find service %s\n", "181A");
        return;
    }
}

void ATC_MiThermometer::connect_to_temperature_characteristic() {
    if (environmentService == nullptr) {
        connect_to_environment_service();
        if (environmentService == nullptr) {
            return;
        }
    }
    temperatureCharacteristic = environmentService->getCharacteristic("2A1F");
    if (temperatureCharacteristic == nullptr) {
        Serial.printf("Failed to find characteristic %s\n", "2A1F");
        return;
    }
}

void ATC_MiThermometer::begin_notify_temp() {
    if (temperatureCharacteristic == nullptr) {
        connect_to_temperature_characteristic();
        if (temperatureCharacteristic == nullptr) {
            return;
        }
    }
    if (temperatureCharacteristic->canNotify()) {
        temperatureCharacteristic->subscribe(true, [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                          uint8_t *pData, size_t length, bool isNotify) {
            this->notifyTempCallback(pBLERemoteCharacteristic, pData, length, isNotify);
        });
        started_notify_temp = true;
    }
}

void ATC_MiThermometer::notifyTempCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                           size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t temp = (pData[1] << 8) | pData[0];
        temperature = (float) temp / 10.0f;
    } else {
        Serial.println("Received invalid temperature data");
    }
}

void ATC_MiThermometer::connect_to_temperature_precise_characteristic() {
    if (environmentService == nullptr) {
        connect_to_environment_service();
        if (environmentService == nullptr) {
            return;
        }
    }
    temperaturePreciseCharacteristic = environmentService->getCharacteristic("2A6E");
    if (temperaturePreciseCharacteristic == nullptr) {
        return;
    }
}

void ATC_MiThermometer::begin_notify_temp_precise() {
    if (temperaturePreciseCharacteristic == nullptr) {
        connect_to_temperature_precise_characteristic();
        if (temperaturePreciseCharacteristic == nullptr) {
            return;
        }
    }
    if (temperaturePreciseCharacteristic->canNotify()) {
        temperaturePreciseCharacteristic->subscribe(true, [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
                                                                 uint8_t *pData, size_t length, bool isNotify) {
            this->notifyTempPreciseCallback(pBLERemoteCharacteristic, pData, length, isNotify);
        });
        started_notify_temp_precise = true;
    }
}

void ATC_MiThermometer::notifyTempPreciseCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                  size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t temp = (pData[1] << 8) | pData[0];
        temperature_precise = (float) temp / 100.0f;
    } else {
        Serial.println("Received invalid precise temperature data");
    }
}

void ATC_MiThermometer::connect_to_humidity_characteristic() {
    if (environmentService == nullptr) {
        connect_to_environment_service();
        if (environmentService == nullptr) {
            return;
        }
    }
    humidityCharacteristic = environmentService->getCharacteristic("2A6F");
    if (humidityCharacteristic == nullptr) {
        Serial.printf("Failed to find characteristic %s\n", "2A6F");
        return;
    }
}

void ATC_MiThermometer::begin_notify_humidity() {
    if (humidityCharacteristic == nullptr) {
        connect_to_humidity_characteristic();
        if (humidityCharacteristic == nullptr) {
            return;
        }
    }
    if (humidityCharacteristic->canNotify()) {
        humidityCharacteristic->subscribe(true,
                                          [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                 size_t length, bool isNotify) {
                                              this->notifyHumidityCallback(pBLERemoteCharacteristic, pData, length,
                                                                           isNotify);
                                          });
        started_notify_humidity = true;
    }
}

void ATC_MiThermometer::notifyHumidityCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                               size_t length, bool isNotify) {
    if (length >= 2) {
        uint16_t hum = (pData[1] << 8) | pData[0];
        humidity = (float) hum / 100.0f;
    } else {
        Serial.println("Received invalid humidity data");
    }
}

void ATC_MiThermometer::connect_to_battery_service() {
    batteryService = pClient->getService("180F");
    if (batteryService == nullptr) {
        return;
    }
}

void ATC_MiThermometer::connect_to_battery_characteristic() {
    if (batteryService == nullptr) {
        connect_to_battery_service();
        if (batteryService == nullptr) {
            return;
        }
    }
    batteryCharacteristic = batteryService->getCharacteristic("2A19");
    if (batteryCharacteristic == nullptr) {
        Serial.printf("Failed to find characteristic %s\n", "2A19");
        return;
    }
}

void ATC_MiThermometer::begin_notify_battery() {
    if (batteryCharacteristic == nullptr) {
        connect_to_battery_characteristic();
        if (batteryCharacteristic == nullptr) {
            return;
        }
    }
    if (batteryCharacteristic->canNotify()) {
        batteryCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                size_t length, bool isNotify) {
                                             this->notifyBatteryCallback(pBLERemoteCharacteristic, pData, length,
                                                                         isNotify);
                                         });
        started_notify_battery = true;
    }
}

void ATC_MiThermometer::notifyBatteryCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                              size_t length, bool isNotify) {
    if (length >= 1) {
        battery_level = pData[0];
    } else {
        Serial.println("Received invalid battery level data");
    }
}

void ATC_MiThermometer::connect_to_command_service() {
    commandService = pClient->getService("1F10");
    if (commandService == nullptr) {
        Serial.printf("Failed to find service %s\n", "1F10");
        return;
    }
}

void ATC_MiThermometer::connect_to_command_characteristic() {
    if (commandService == nullptr) {
        connect_to_command_service();
        if (commandService == nullptr) {
            return;
        }
    }
    commandCharacteristic = commandService->getCharacteristic("1F1F");
    if (commandCharacteristic == nullptr) {
        Serial.printf("Failed to find characteristic %s\n", "1F1F");
        return;
    }
}

void ATC_MiThermometer::readSettings() {
    int attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
        if (!isConnected()) {
        }
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (commandService == nullptr) {
        connect_to_command_service();
        if (commandService == nullptr) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (commandCharacteristic == nullptr) {
        connect_to_command_characteristic();
        if (commandCharacteristic == nullptr) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    received_settings = false;
    if (commandCharacteristic->canNotify()) {
        commandCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                size_t length, bool isNotify) {
                                             this->notifySettingsCallback(pBLERemoteCharacteristic, pData, length,
                                                                          isNotify);
                                         });
    } else {
        Serial.println("Command characteristic cannot notify");
        return;
    }
    delay(1000);
    uint8_t data[1] = {0x55};
    sendCommand(data, 1);
    uint32_t start = millis();
    while (!received_settings && millis() - start < 5000) {
        delay(100);
        yield();
    }
    if (!received_settings) {
        Serial.println("Failed to read settings");
    }
    commandCharacteristic->unsubscribe();
}

void ATC_MiThermometer::notifySettingsCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                               size_t length, bool isNotify) {
    if (pData == nullptr || length == 0) {
        Serial.println("Received empty data in notifySettingsCallback");
        return;
    }
    read_settings = true;
    received_settings = true;
    if (length < 13) {
        Serial.println("Invalid settings length");
        return;
    }
    if (length >= 13) {
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
        settings.temp_offset = (float) static_cast<int8_t>(pData[4]) / 10.0f;
        settings.humidity_offset = (float) static_cast<int8_t>(pData[5]) / 10.0f;
        settings.advertising_interval = pData[6];
        settings.measure_interval = pData[7];
        settings.rfTxPower = static_cast<RF_TX_Power>(pData[8]);
        settings.connect_latency = pData[9];
        settings.lcd_update_interval = pData[10];
        settings.hw_version = static_cast<HW_VERSION_ID>(pData[11]);
        settings.averaging_measurements = pData[12];
    }
}

void ATC_MiThermometer::sendCommand(uint8_t *data, size_t length) {
    if (commandCharacteristic == nullptr) {
        connect_to_command_characteristic();
        if (commandCharacteristic == nullptr) {
            Serial.println("Command characteristic not found, cannot send command");
            return;
        }
    }
    bool success = commandCharacteristic->writeValue(data, length, true);
    if (!success) {
        Serial.println("Failed to send command");
    }
}

void ATC_MiThermometer::disconnect() {
    if (pClient != nullptr) {
        if (pClient->isConnected()) {
            pClient->disconnect();
        }
        NimBLEDevice::deleteClient(pClient);
        pClient = nullptr;
    }
    environmentService = nullptr;
    batteryService = nullptr;
    commandService = nullptr;
    temperatureCharacteristic = nullptr;
    temperaturePreciseCharacteristic = nullptr;
    humidityCharacteristic = nullptr;
    batteryCharacteristic = nullptr;
    commandCharacteristic = nullptr;
}

void ATC_MiThermometer::connect_to_all_services() {
    connect_to_environment_service();
    connect_to_battery_service();
    connect_to_command_service();
}

void ATC_MiThermometer::connect_to_all_characteristics() {
    connect_to_temperature_characteristic();
    connect_to_temperature_precise_characteristic();
    connect_to_humidity_characteristic();
    connect_to_battery_characteristic();
    connect_to_command_characteristic();
}

void ATC_MiThermometer::begin_notify() {
    begin_notify_temp();
    begin_notify_temp_precise();
    begin_notify_humidity();
    begin_notify_battery();
}

float ATC_MiThermometer::getTemperature() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return temperature;
        } else if (getAdvertisingType() == Advertising_Type::PVVX) {
            return round(temperature_precise * 10.0f) / 10.0f;
        } else if (getAdvertisingType() == Advertising_Type::BTHOME) {
            return round(temperature_precise * 10.0f) / 10.0f;
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        if (!started_notify_temp) {
            readTemperature();
        }
        return temperature;
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readTemperature();
        return temperature;
    }
    return 0;
}

void ATC_MiThermometer::readTemperature() {
    if (temperatureCharacteristic == nullptr) {
        connect_to_temperature_characteristic();
        if (temperatureCharacteristic == nullptr) {
            Serial.println("Temperature characteristic not found, cannot read temperature");
            return;
        }
    }
    std::string value = temperatureCharacteristic->readValue();
    if (value.length() >= 2) {
        uint16_t temp = (value[1] << 8) | value[0];
        temperature = (float) temp / 10.0f;
    } else {
        Serial.println("Failed to read temperature, insufficient data");
    }
}

float ATC_MiThermometer::getTemperaturePrecise() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return temperature;
        } else if (getAdvertisingType() == Advertising_Type::PVVX) {
            return temperature_precise;
        } else if (getAdvertisingType() == Advertising_Type::BTHOME) {
            return temperature_precise;
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        if (!started_notify_temp_precise) {
            readTemperaturePrecise();
        }
        return temperature_precise;
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readTemperaturePrecise();
        return temperature_precise;
    }
    return 0;
}

void ATC_MiThermometer::readTemperaturePrecise() {
    if (temperaturePreciseCharacteristic == nullptr) {
        connect_to_temperature_precise_characteristic();
        if (temperaturePreciseCharacteristic == nullptr) {
            Serial.println("Precise temperature characteristic not found, cannot read precise temperature");
            return;
        }
    }
    std::string value = temperaturePreciseCharacteristic->readValue();
    if (value.length() >= 2) {
        uint16_t temp = (value[1] << 8) | value[0];
        temperature_precise = (float) temp / 100.0f;
    } else {
        Serial.println("Failed to read precise temperature, insufficient data");
    }
}

float ATC_MiThermometer::getHumidity() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return humidity;
        } else if (getAdvertisingType() == Advertising_Type::PVVX) {
            return humidity;
        } else if (getAdvertisingType() == Advertising_Type::BTHOME) {
            return humidity;
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        if (!started_notify_humidity) {
            readHumidity();
        }
        return humidity;
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readHumidity();
        return humidity;
    }
    return 0;
}

void ATC_MiThermometer::readHumidity() {
    if (humidityCharacteristic == nullptr) {
        connect_to_humidity_characteristic();
        if (humidityCharacteristic == nullptr) {
            Serial.println("Humidity characteristic not found, cannot read humidity");
            return;
        }
    }
    std::string value = humidityCharacteristic->readValue();
    if (value.length() >= 2) {
        uint16_t hum = (value[1] << 8) | value[0];
        humidity = (float) hum / 100.0f;
    } else {
        Serial.println("Failed to read humidity, insufficient data");
    }
}

uint8_t ATC_MiThermometer::getBatteryLevel() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return battery_level;
        } else if (getAdvertisingType() == Advertising_Type::PVVX) {
            return battery_level;
        } else if (getAdvertisingType() == Advertising_Type::BTHOME) {
            return battery_level;
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        if (!started_notify_battery) {
            readBatteryLevel();
        }
        return battery_level;
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readBatteryLevel();
        return battery_level;
    }
    return 0;
}

void ATC_MiThermometer::readBatteryLevel() {
    if (batteryCharacteristic == nullptr) {
        connect_to_battery_characteristic();
        if (batteryCharacteristic == nullptr) {
            Serial.println("Battery characteristic not found, cannot read battery level");
            return;
        }
    }
    std::string value = batteryCharacteristic->readValue();
    if (!value.empty()) {
        battery_level = (uint8_t) value[0];
    } else {
        Serial.println("Failed to read battery level, insufficient data");
    }
}

Advertising_Type ATC_MiThermometer::getAdvertisingType() {
    if (!read_settings) {
        readSettings();
    }
    return settings.advertising_type;
}

char *ATC_MiThermometer::getAddress() {
    return (char *) address;
}

void ATC_MiThermometer::parseAdvertisingData(uint8_t *data, size_t length) {
    if (!read_settings) {
        readSettings();
        if (connection_mode == ADVERTISING) {
            disconnect();
        }
        return;
    }
    if (getAdvertisingType() == Advertising_Type::BTHOME) {
        parseAdvertisingDataBTHOME(data, length);
    } else if (settings.advertising_type == Advertising_Type::PVVX) {
        parseAdvertisingDataPVVX(data, length);
    } else if (settings.advertising_type == Advertising_Type::ATC1441) {
        parseAdvertisingDataATC1441(data, length);
    } else {
        Serial.println("Unknown advertising type");
    }
}

void ATC_MiThermometer::parseAdvertisingDataATC1441(uint8_t *data, size_t length) {
    if (length < 18) {
        Serial.println("Pachetul este prea scurt!");
        return;
    }
    int16_t temperatureRaw = (data[10] << 8) | data[11];
    temperature = (float) temperatureRaw * 0.1f;
    humidity = data[12];
    battery_level = data[13];
    battery_mv = (data[14] << 8) | data[15];
}

void ATC_MiThermometer::parseAdvertisingDataPVVX(uint8_t *data, size_t length) {
    if (length < 19) {
        Serial.println("Pachetul este prea scurt!");
        return;
    }
    uint8_t size = data[0];
    if (size != 18) {
        Serial.println("Mărimea pachetului este incorectă!");
        return;
    }
    uint8_t uid = data[1];
    if (uid != 0x16) {
        Serial.println("UID incorect, nu este Service Data cu UUID de 16 biți!");
        return;
    }
    uint16_t uuid = data[2] | (data[3] << 8);
    if (uuid != 0x181A) {
        Serial.println("UUID incorect, nu este 0x181A!");
        return;
    }
    int16_t temperatureRaw = data[10] | (data[11] << 8);
    temperature_precise = (float) temperatureRaw * 0.01f;
    uint16_t humidityRaw = data[12] | (data[13] << 8);
    humidity = (float) humidityRaw * 0.01f;
    battery_mv = data[14] | (data[15] << 8);
    battery_level = data[16];
}

void ATC_MiThermometer::parseAdvertisingDataBTHOME(uint8_t *data, size_t length) {
    if (length < 6) {
        Serial.println("Pachetul este prea scurt!");
        return;
    }
    size_t index = 0;
    while (index < length) {
        uint8_t element_length = data[index];
        if (element_length == 0) {
            break;
        }
        if (index + 1 + element_length > length) {
            Serial.println("Lungimea elementului AD depășește dimensiunea pachetului!");
            break;
        }
        uint8_t ad_type = data[index + 1];
        uint8_t *ad_data = &data[index + 2];
        uint8_t ad_data_length = element_length - 1;
        if (ad_type == 0x16) {
            if (ad_data_length < 3) {
                Serial.println("Service Data prea scurt!");
                break;
            }
            uint16_t uuid = ad_data[0] | (ad_data[1] << 8);
            if (uuid != 0xFCD2) {
                Serial.println("UUID necunoscut pentru BTHome!");
                break;
            }
            size_t dataIndex = 3;
            while (dataIndex < ad_data_length) {
                uint8_t objectId = ad_data[dataIndex++];
                switch (objectId) {
                    case 0x00: {
                        if (dataIndex >= ad_data_length) {
                            Serial.println("Lipsă date pentru Packet ID!");
                            break;
                        }
                        uint8_t packetId = ad_data[dataIndex++];
                        break;
                    }
                    case 0x01: {
                        if (dataIndex >= ad_data_length) {
                            Serial.println("Lipsă date pentru Nivel baterie!");
                            break;
                        }
                        uint8_t batteryLevel = ad_data[dataIndex++];
                        this->battery_level = batteryLevel;
                        break;
                    }
                    case 0x02: {
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Lipsă date pentru Temperatură!");
                            break;
                        }
                        uint16_t temperatureRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        temperature_precise = (float) temperatureRaw * 0.01f;
                        dataIndex += 2;
                        break;
                    }
                    case 0x03: {
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Lipsă date pentru Umiditate!");
                            break;
                        }
                        uint16_t humidityRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        humidity = (float) humidityRaw * 0.01f;
                        dataIndex += 2;
                        break;
                    }
                    case 0x0C: {
                        if (dataIndex + 1 >= ad_data_length) {
                            Serial.println("Lipsă date pentru Voltage!");
                            break;
                        }
                        uint16_t voltageRaw = ad_data[dataIndex] | (ad_data[dataIndex + 1] << 8);
                        battery_mv = voltageRaw;
                        dataIndex += 2;
                        break;
                    }
                    default:
                        dataIndex = ad_data_length;
                        break;
                }
            }
        }
        index += 1 + element_length;
    }
}

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
        connect_to_all_services();
        connect_to_all_characteristics();
        begin_notify();
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readTemperature();
        readTemperaturePrecise();
        readHumidity();
        readBatteryLevel();
    }
}

bool ATC_MiThermometer::get_read_settings() const {
    return read_settings;
}

uint16_t ATC_MiThermometer::getBatteryVoltage() {
    if (connection_mode == Connection_mode::ADVERTISING) {
        if (getAdvertisingType() == Advertising_Type::ATC1441) {
            return battery_mv;
        } else if (getAdvertisingType() == Advertising_Type::PVVX) {
            return battery_mv;
        } else if (getAdvertisingType() == Advertising_Type::BTHOME) {
            return battery_mv;
        }
    } else if (connection_mode == Connection_mode::NOTIFICATION) {
        if (!started_notify_battery) {
            readBatteryLevel();
        }
        return 2000 + (battery_level * (3000 - 2000) / 100);
    } else if (connection_mode == Connection_mode::CONNECTION) {
        readBatteryLevel();
        return 2000 + (battery_level * (3000 - 2000) / 100);
    }
    return 0;
}

RF_TX_Power ATC_MiThermometer::getRfTxPower() {
    if (!read_settings) {
        readSettings();
    }
    return settings.rfTxPower;
}

bool ATC_MiThermometer::getLowPowerMeasures() {
    if (!read_settings) {
        readSettings();
    }
    return settings.lp_measures;
}

bool ATC_MiThermometer::getTransmitMeasures() {
    if (!read_settings) {
        readSettings();
    }
    return settings.tx_measures;
}

bool ATC_MiThermometer::getShowBattery() {
    if (!read_settings) {
        readSettings();
    }
    return settings.show_battery;
}

bool ATC_MiThermometer::getTempFOrC() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_F_or_C;
}

bool ATC_MiThermometer::getBlinkingTimeSmile() {
    if (!read_settings) {
        readSettings();
    }
    return settings.blinking_time_smile;
}

bool ATC_MiThermometer::getComfortSmiley() {
    if (!read_settings) {
        readSettings();
    }
    return settings.comfort_smiley;
}

bool ATC_MiThermometer::getAdvCrypto() {
    if (!read_settings) {
        readSettings();
    }
    return settings.adv_crypto;
}

bool ATC_MiThermometer::getAdvFlags() {
    if (!read_settings) {
        readSettings();
    }
    return settings.adv_flags;
}

Smiley ATC_MiThermometer::getSmiley() {
    if (!read_settings) {
        readSettings();
    }
    return settings.smiley;
}

bool ATC_MiThermometer::getBT5PHY() {
    if (!read_settings) {
        readSettings();
    }
    return settings.bt5phy;
}

bool ATC_MiThermometer::getLongRange() {
    if (!read_settings) {
        readSettings();
    }
    return settings.long_range;
}

bool ATC_MiThermometer::getScreenOff() {
    if (!read_settings) {
        readSettings();
    }
    return settings.screen_off;
}

float ATC_MiThermometer::getTempOffset() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_offset;
}

float ATC_MiThermometer::getHumidityOffset() {
    if (!read_settings) {
        readSettings();
    }
    return settings.humidity_offset;
}

int8_t ATC_MiThermometer::getTempOffsetCal() {
    if (!read_settings) {
        readSettings();
    }
    return settings.temp_offset_cal;
}

int8_t ATC_MiThermometer::getHumidityOffsetCal() {
    if (!read_settings) {
        readSettings();
    }
    return settings.humidity_offset_cal;
}

uint8_t ATC_MiThermometer::getAdvertisingIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.advertising_interval;
}

uint8_t ATC_MiThermometer::getMeasureIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.measure_interval;
}

uint8_t ATC_MiThermometer::getConnectLatencySteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.connect_latency;
}

uint8_t ATC_MiThermometer::getLcdUpdateIntervalSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.lcd_update_interval;
}

HW_VERSION_ID ATC_MiThermometer::getHwVersion() {
    if (!read_settings) {
        readSettings();
    }
    return settings.hw_version;
}

uint8_t ATC_MiThermometer::getAveragingMeasurementsSteps() {
    if (!read_settings) {
        readSettings();
    }
    return settings.averaging_measurements;
}

float ATC_MiThermometer::getRfTxPowerdBm() {
    switch (getRfTxPower()) {
        case RF_TX_Power::dBm_3_01:
            return 3.01;
        case RF_TX_Power::dBm_2_81:
            return 2.81;
        case RF_TX_Power::dBm_2_61:
            return 2.61;
        case RF_TX_Power::dBm_2_39:
            return 2.39;
        case RF_TX_Power::dBm_1_99:
            return 1.99;
        case RF_TX_Power::dBm_1_73:
            return 1.73;
        case RF_TX_Power::dBm_1_45:
            return 1.45;
        case RF_TX_Power::dBm_1_17:
            return 1.17;
        case RF_TX_Power::dBm_0_90:
            return 0.90;
        case RF_TX_Power::dBm_0_58:
            return 0.58;
        case RF_TX_Power::dBm_0_04:
            return 0.04;
        case RF_TX_Power::dBm_n0_14:
            return -0.14;
        case RF_TX_Power::dBm_n0_97:
            return -0.97;
        case RF_TX_Power::dBm_n1_42:
            return -1.42;
        case RF_TX_Power::dBm_n1_89:
            return -1.89;
        case RF_TX_Power::dBm_n2_48:
            return -2.48;
        case RF_TX_Power::dBm_n3_03:
            return -3.03;
        case RF_TX_Power::dBm_n3_61:
            return -3.61;
        case RF_TX_Power::dBm_n4_26:
            return -4.26;
        case RF_TX_Power::dBm_n5_03:
            return -5.03;
        case RF_TX_Power::dBm_n5_81:
            return -5.81;
        case RF_TX_Power::dBm_n6_67:
            return -6.67;
        case RF_TX_Power::dBm_n7_65:
            return -7.65;
        case RF_TX_Power::dBm_n8_65:
            return -8.65;
        case RF_TX_Power::dBm_n9_89:
            return -9.89;
        case RF_TX_Power::dBm_n11_4:
            return -11.4;
        case RF_TX_Power::dBm_n13_29:
            return -13.29;
        case RF_TX_Power::dBm_n15_88:
            return -15.88;
        case RF_TX_Power::dBm_n19_27:
            return -19.27;
        case RF_TX_Power::dBm_n25_18:
            return -25.18;
        case RF_TX_Power::dBm_n30:
            return -30;
        case RF_TX_Power::dBm_n50:
            return -50;
        case RF_TX_Power::dBm_10_46:
            return 10.46;
        case RF_TX_Power::dBm_10_29:
            return 10.29;
        case RF_TX_Power::dBm_10_01:
            return 10.01;
        case RF_TX_Power::dBm_9_81:
            return 9.81;
        case RF_TX_Power::dBm_9_48:
            return 9.48;
        case RF_TX_Power::dBm_9_24:
            return 9.24;
        case RF_TX_Power::dBm_8_97:
            return 8.97;
        case RF_TX_Power::dBm_8_73:
            return 8.73;
        case RF_TX_Power::dBm_8_44:
            return 8.44;
        case RF_TX_Power::dBm_8_13:
            return 8.13;
        case RF_TX_Power::dBm_7_79:
            return 7.79;
        case RF_TX_Power::dBm_7_41:
            return 7.41;
        case RF_TX_Power::dBm_7_02:
            return 7.02;
        case RF_TX_Power::dBm_6_60:
            return 6.60;
        case RF_TX_Power::dBm_6_14:
            return 6.14;
        case RF_TX_Power::dBm_5_65:
            return 5.65;
        case RF_TX_Power::dBm_5_13:
            return 5.13;
        case RF_TX_Power::dBm_4_57:
            return 4.57;
        case RF_TX_Power::dBm_3_94:
            return 3.94;
        case RF_TX_Power::dBm_3_23:
            return 3.23;
        default:
            return 0;
    }
}

uint16_t ATC_MiThermometer::getAdvertisingIntervalMs() {
    return (uint16_t) ((float) getAdvertisingIntervalSteps() * advertising_interval_step_time_ms);
}

uint32_t ATC_MiThermometer::getMeasureIntervalMs() {
    return getMeasureIntervalSteps() * getAdvertisingIntervalMs();
}

uint16_t ATC_MiThermometer::getConnectLatencyMs() {
    return getConnectLatencySteps() * connect_latency_step_time_ms;
}

uint16_t ATC_MiThermometer::getLcdUpdateIntervalMs() {
    return getLcdUpdateIntervalSteps() * lcd_update_interval_step_time_ms;
}

uint32_t ATC_MiThermometer::getAveragingMeasurementsMs() {
    return getMeasureIntervalMs() * getAveragingMeasurementsSteps();
}

uint16_t ATC_MiThermometer::getAveragingMeasurementsSec() {
    return getAveragingMeasurementsMs() / 1000;
}

uint8_t *ATC_MiThermometer::parseSettings(ATC_MiThermometer_Settings settingsToParse) {
    auto *data = new uint8_t[12];
    data[0] = 0x55;
    data[1] = 0x0A;
    data[2] = settingsToParse.lp_measures << 7 | settingsToParse.tx_measures << 6 | settingsToParse.show_battery << 5 |
              settingsToParse.temp_F_or_C << 4 | settingsToParse.blinking_time_smile << 3 |
              settingsToParse.comfort_smiley << 2 |
              settingsToParse.advertising_type;
    data[3] = settingsToParse.screen_off << 7 | settingsToParse.long_range << 6 | settingsToParse.bt5phy << 5 |
              settingsToParse.adv_flags << 4 |
              settingsToParse.adv_crypto << 3 | settingsToParse.smiley;
    data[4] = (uint8_t) settingsToParse.temp_offset * 10;
    data[5] = (uint8_t) settingsToParse.humidity_offset * 10;
    data[6] = settingsToParse.advertising_interval;
    data[7] = settingsToParse.measure_interval;
    data[8] = static_cast<uint8_t>(settingsToParse.rfTxPower);
    data[9] = settingsToParse.connect_latency;
    data[10] = settingsToParse.lcd_update_interval;
    data[11] = settingsToParse.averaging_measurements;
    return data;
}

void ATC_MiThermometer::sendSettings(ATC_MiThermometer_Settings newSettings) {
    uint8_t attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (commandService == nullptr) {
        connect_to_command_service();
        if (commandService == nullptr) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (commandCharacteristic == nullptr) {
        connect_to_command_characteristic();
        if (commandCharacteristic == nullptr) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    received_settings = false;
    if (commandCharacteristic->canNotify()) {
        commandCharacteristic->subscribe(true,
                                         [this](NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData,
                                                size_t length, bool isNotify) {
                                             this->notifySettingsCallback(pBLERemoteCharacteristic, pData, length,
                                                                          isNotify);
                                         });
    } else {
        Serial.println("Command characteristic cannot notify");
        return;
    }
    uint8_t *data = parseSettings(newSettings);
    sendCommand(data, 12);
    delete[] data;
    uint32_t start = millis();
    while (!received_settings && millis() - start < 5000) {
        delay(100);
        yield();
    }
    if (!received_settings) {
        Serial.println("Failed to read settings");
    }
    commandCharacteristic->unsubscribe();
}

void ATC_MiThermometer::setRfTxPower(RF_TX_Power power) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.rfTxPower = power;
    sendSettings(newSettings);
}

ATC_MiThermometer_Settings ATC_MiThermometer::getSettings() {
    return settings;
}

void ATC_MiThermometer::setRfTxPowerdBm(float power) {
    float min_diff = 1000;
    RF_TX_Power closest_power = RF_TX_Power::dBm_3_01;
    if (abs(power - 3.01) < min_diff) {
        min_diff = abs(power - 3.01f);
        closest_power = RF_TX_Power::dBm_3_01;
    }
    if (abs(power - 2.81) < min_diff) {
        min_diff = abs(power - 2.81f);
        closest_power = RF_TX_Power::dBm_2_81;
    }
    if (abs(power - 2.61) < min_diff) {
        min_diff = abs(power - 2.61f);
        closest_power = RF_TX_Power::dBm_2_61;
    }
    if (abs(power - 2.39) < min_diff) {
        min_diff = abs(power - 2.39f);
        closest_power = RF_TX_Power::dBm_2_39;
    }
    if (abs(power - 1.99) < min_diff) {
        min_diff = abs(power - 1.99f);
        closest_power = RF_TX_Power::dBm_1_99;
    }
    if (abs(power - 1.73) < min_diff) {
        min_diff = abs(power - 1.73f);
        closest_power = RF_TX_Power::dBm_1_73;
    }
    if (abs(power - 1.45) < min_diff) {
        min_diff = abs(power - 1.45f);
        closest_power = RF_TX_Power::dBm_1_45;
    }
    if (abs(power - 1.17) < min_diff) {
        min_diff = abs(power - 1.17f);
        closest_power = RF_TX_Power::dBm_1_17;
    }
    if (abs(power - 0.90) < min_diff) {
        min_diff = abs(power - 0.90f);
        closest_power = RF_TX_Power::dBm_0_90;
    }
    if (abs(power - 0.58) < min_diff) {
        min_diff = abs(power - 0.58f);
        closest_power = RF_TX_Power::dBm_0_58;
    }
    if (abs(power - 0.04) < min_diff) {
        min_diff = abs(power - 0.04f);
        closest_power = RF_TX_Power::dBm_0_04;
    }
    if (abs(power + 0.14) < min_diff) {
        min_diff = abs(power + 0.14f);
        closest_power = RF_TX_Power::dBm_n0_14;
    }
    if (abs(power + 0.97) < min_diff) {
        min_diff = abs(power + 0.97f);
        closest_power = RF_TX_Power::dBm_n0_97;
    }
    if (abs(power + 1.42) < min_diff) {
        min_diff = abs(power + 1.42f);
        closest_power = RF_TX_Power::dBm_n1_42;
    }
    if (abs(power + 1.89) < min_diff) {
        min_diff = abs(power + 1.89f);
        closest_power = RF_TX_Power::dBm_n1_89;
    }
    if (abs(power + 2.48) < min_diff) {
        min_diff = abs(power + 2.48f);
        closest_power = RF_TX_Power::dBm_n2_48;
    }
    if (abs(power + 3.03) < min_diff) {
        min_diff = abs(power + 3.03f);
        closest_power = RF_TX_Power::dBm_n3_03;
    }
    if (abs(power + 3.61) < min_diff) {
        min_diff = abs(power + 3.61f);
        closest_power = RF_TX_Power::dBm_n3_61;
    }
    if (abs(power + 4.26) < min_diff) {
        min_diff = abs(power + 4.26f);
        closest_power = RF_TX_Power::dBm_n4_26;
    }
    if (abs(power + 5.03) < min_diff) {
        min_diff = abs(power + 5.03f);
        closest_power = RF_TX_Power::dBm_n5_03;
    }
    if (abs(power + 5.81) < min_diff) {
        min_diff = abs(power + 5.81f);
        closest_power = RF_TX_Power::dBm_n5_81;
    }
    if (abs(power + 6.67) < min_diff) {
        min_diff = abs(power + 6.67f);
        closest_power = RF_TX_Power::dBm_n6_67;
    }
    if (abs(power + 7.65) < min_diff) {
        min_diff = abs(power + 7.65f);
        closest_power = RF_TX_Power::dBm_n7_65;
    }
    if (abs(power + 8.65) < min_diff) {
        min_diff = abs(power + 8.65f);
        closest_power = RF_TX_Power::dBm_n8_65;
    }
    if (abs(power + 9.89) < min_diff) {
        min_diff = abs(power + 9.89f);
        closest_power = RF_TX_Power::dBm_n9_89;
    }
    if (abs(power + 11.4) < min_diff) {
        min_diff = abs(power + 11.4f);
        closest_power = RF_TX_Power::dBm_n11_4;
    }
    if (abs(power + 13.29) < min_diff) {
        min_diff = abs(power + 13.29f);
        closest_power = RF_TX_Power::dBm_n13_29;
    }
    if (abs(power + 15.88) < min_diff) {
        min_diff = abs(power + 15.88f);
        closest_power = RF_TX_Power::dBm_n15_88;
    }
    if (abs(power + 19.27) < min_diff) {
        min_diff = abs(power + 19.27f);
        closest_power = RF_TX_Power::dBm_n19_27;
    }
    if (abs(power + 25.18) < min_diff) {
        min_diff = abs(power + 25.18f);
        closest_power = RF_TX_Power::dBm_n25_18;
    }
    if (abs(power + 30) < min_diff) {
        min_diff = abs(power + 30);
        closest_power = RF_TX_Power::dBm_n30;
    }
    if (abs(power + 50) < min_diff) {
        min_diff = abs(power + 50);
        closest_power = RF_TX_Power::dBm_n50;
    }
    if (abs(power - 10.46) < min_diff) {
        min_diff = abs(power - 10.46f);
        closest_power = RF_TX_Power::dBm_10_46;
    }
    if (abs(power - 10.29) < min_diff) {
        min_diff = abs(power - 10.29f);
        closest_power = RF_TX_Power::dBm_10_29;
    }
    if (abs(power - 10.01) < min_diff) {
        min_diff = abs(power - 10.01f);
        closest_power = RF_TX_Power::dBm_10_01;
    }
    if (abs(power - 9.81) < min_diff) {
        min_diff = abs(power - 9.81f);
        closest_power = RF_TX_Power::dBm_9_81;
    }
    if (abs(power - 9.48) < min_diff) {
        min_diff = abs(power - 9.48f);
        closest_power = RF_TX_Power::dBm_9_48;
    }
    if (abs(power - 9.24) < min_diff) {
        min_diff = abs(power - 9.24f);
        closest_power = RF_TX_Power::dBm_9_24;
    }
    if (abs(power - 8.97) < min_diff) {
        min_diff = abs(power - 8.97f);
        closest_power = RF_TX_Power::dBm_8_97;
    }
    if (abs(power - 8.73) < min_diff) {
        min_diff = abs(power - 8.73f);
        closest_power = RF_TX_Power::dBm_8_73;
    }
    if (abs(power - 8.44) < min_diff) {
        min_diff = abs(power - 8.44f);
        closest_power = RF_TX_Power::dBm_8_44;
    }
    if (abs(power - 8.13) < min_diff) {
        min_diff = abs(power - 8.13f);
        closest_power = RF_TX_Power::dBm_8_13;
    }
    if (abs(power - 7.79) < min_diff) {
        min_diff = abs(power - 7.79f);
        closest_power = RF_TX_Power::dBm_7_79;
    }
    if (abs(power - 7.41) < min_diff) {
        min_diff = abs(power - 7.41f);
        closest_power = RF_TX_Power::dBm_7_41;
    }
    if (abs(power - 7.02) < min_diff) {
        min_diff = abs(power - 7.02f);
        closest_power = RF_TX_Power::dBm_7_02;
    }
    if (abs(power - 6.60) < min_diff) {
        min_diff = abs(power - 6.60f);
        closest_power = RF_TX_Power::dBm_6_60;
    }
    if (abs(power - 6.14) < min_diff) {
        min_diff = abs(power - 6.14f);
        closest_power = RF_TX_Power::dBm_6_14;
    }
    if (abs(power - 5.65) < min_diff) {
        min_diff = abs(power - 5.65f);
        closest_power = RF_TX_Power::dBm_5_65;
    }
    if (abs(power - 5.13) < min_diff) {
        min_diff = abs(power - 5.13f);
        closest_power = RF_TX_Power::dBm_5_13;
    }
    if (abs(power - 4.57) < min_diff) {
        min_diff = abs(power - 4.57f);
        closest_power = RF_TX_Power::dBm_4_57;
    }
    if (abs(power - 3.94) < min_diff) {
        min_diff = abs(power - 3.94f);
        closest_power = RF_TX_Power::dBm_3_94;
    }
    if (abs(power - 3.23) < min_diff) {
        closest_power = RF_TX_Power::dBm_3_23;
    }
    setRfTxPower(closest_power);
}

void ATC_MiThermometer::setLowPowerMeasures(bool lowPowerMeasures) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    settings.lp_measures = lowPowerMeasures;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setTransmitMeasures(bool transmitMeasures) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.tx_measures = transmitMeasures;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setShowBattery(bool showBattery) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.show_battery = showBattery;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setTempFOrC(bool tempFOrC) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_F_or_C = tempFOrC;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setBlinkingTimeSmile(bool blinkingTimeSmile) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.blinking_time_smile = blinkingTimeSmile;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setComfortSmiley(bool comfortSmiley) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.comfort_smiley = comfortSmiley;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setAdvCrypto(bool advCrypto) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.adv_crypto = advCrypto;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setAdvFlags(bool advFlags) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.adv_flags = advFlags;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setSmiley(Smiley smiley) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.smiley = smiley;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setBT5PHY(bool BT5PHY) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.bt5phy = BT5PHY;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setLongRange(bool longRange) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.long_range = longRange;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setScreenOff(bool screenOff) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.screen_off = screenOff;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setTempOffset(float tempOffset) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_offset = tempOffset;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setHumidityOffset(float humidityOffset) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.humidity_offset = humidityOffset;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setTempOffsetCal(int8_t tempOffsetCal) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.temp_offset_cal = tempOffsetCal;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setHumidityOffsetCal(int8_t humidityOffsetCal) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.humidity_offset_cal = humidityOffsetCal;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setAdvertisingIntervalSteps(uint8_t advertisingIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.advertising_interval = advertisingIntervalSteps;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setAdvertisingIntervalMs(uint16_t advertisingIntervalMs) {
    setAdvertisingIntervalSteps((uint8_t) ((float) advertisingIntervalMs / advertising_interval_step_time_ms));
}

void ATC_MiThermometer::setMeasureIntervalMs(uint32_t measureIntervalMs) {
    setMeasureIntervalSteps(measureIntervalMs / getAdvertisingIntervalMs());
}

void ATC_MiThermometer::setMeasureIntervalSteps(uint8_t measureIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.measure_interval = measureIntervalSteps;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setConnectLatencySteps(uint8_t connectLatencySteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.connect_latency = connectLatencySteps;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setConnectLatencyMs(uint16_t connectLatencyMs) {
    setConnectLatencySteps(connectLatencyMs / connect_latency_step_time_ms);
}

void ATC_MiThermometer::setLcdUpdateIntervalSteps(uint8_t lcdUpdateIntervalSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.lcd_update_interval = lcdUpdateIntervalSteps;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setLcdUpdateIntervalMs(uint16_t lcdUpdateIntervalMs) {
    setLcdUpdateIntervalSteps(lcdUpdateIntervalMs / lcd_update_interval_step_time_ms);
}

void ATC_MiThermometer::setAveragingMeasurementsSteps(uint8_t averagingMeasurementsSteps) {
    ATC_MiThermometer_Settings newSettings = getSettings();
    newSettings.averaging_measurements = averagingMeasurementsSteps;
    sendSettings(newSettings);
}

void ATC_MiThermometer::setAveragingMeasurementsMs(uint32_t averagingMeasurementsMs) {
    setAveragingMeasurementsSteps(averagingMeasurementsMs / getMeasureIntervalMs());
}

void ATC_MiThermometer::setAveragingMeasurementsSec(uint16_t averagingMeasurementsSec) {
    setAveragingMeasurementsMs(averagingMeasurementsSec * 1000);
}

void ATC_MiThermometer::resetSettings() {
    uint8_t data[1] = {0x56};
    sendCommand(data, 1);
    read_settings = false;
    received_settings = false;
    readSettings();
}

void ATC_MiThermometer::setClock(time_t time) {
    uint8_t attempts = 0;
    while (!isConnected() && attempts < 5) {
        connect();
        attempts++;
        yield();
    }
    if (!isConnected()) {
        Serial.println("Failed to connect to device");
        return;
    }
    if (commandService == nullptr) {
        connect_to_command_service();
        if (commandService == nullptr) {
            Serial.println("Command service not found");
            return;
        }
    }
    if (commandCharacteristic == nullptr) {
        connect_to_command_characteristic();
        if (commandCharacteristic == nullptr) {
            Serial.println("Command characteristic not found");
            return;
        }
    }
    uint8_t data[5];
    data[0] = 0x23;
    data[1] = (uint8_t) (time & 0xFF);
    data[2] = (uint8_t) ((time >> 8) & 0xFF);
    data[3] = (uint8_t) ((time >> 16) & 0xFF);
    data[4] = (uint8_t) ((time >> 24) & 0xFF);
    sendCommand(data, 5);
}

void ATC_MiThermometer::setClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month,
                                 uint16_t year) {
    tm time{};
    time.tm_hour = hours;
    time.tm_min = minutes;
    time.tm_sec = seconds;
    time.tm_mday = day;
    time.tm_mon = month - 1;
    time.tm_year = year - 1900;
    setClock(mktime(&time));
}
