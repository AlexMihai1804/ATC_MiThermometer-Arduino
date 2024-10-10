#ifndef ATC_MI_THERMOMETER_NIMBLE_H
#define ATC_MI_THERMOMETER_NIMBLE_H

#include <NimBLEDevice.h>
#include <Arduino.h>
#include <cstdint>
#include "ATC_MiThermometer_structs.h"
#include "ATC_MiThermometer_enums.h"

class ATC_MiThermometer {
private:
    const char *address;
    NimBLEClient *pClient;
    NimBLERemoteService *environmentService;
    NimBLERemoteService *batteryService;
    NimBLERemoteService *commandService;
    NimBLERemoteCharacteristic *temperatureCharacteristic;
    NimBLERemoteCharacteristic *temperaturePreciseCharacteristic;
    NimBLERemoteCharacteristic *humidityCharacteristic;
    NimBLERemoteCharacteristic *batteryCharacteristic;
    NimBLERemoteCharacteristic *commandCharacteristic;
    bool received_settings;
    bool read_settings;
    bool started_notify_temp;
    bool started_notify_temp_precise;
    bool started_notify_humidity;
    bool started_notify_battery;
    float temperature;
    float temperature_precise;
    float humidity;
    uint16_t battery_mv;
    uint8_t battery_level;
    ATC_MiThermometer_Settings settings{};
    Connection_mode connection_mode;

    // Callback-uri
    void notifyTempPreciseCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                                   bool isNotify);

    void notifyTempCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                            bool isNotify);

    void notifyHumidityCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                                bool isNotify);

    void notifyBatteryCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                               bool isNotify);

    void notifySettingsCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length,
                                bool isNotify);

    void parseAdvertisingDataATC1441(uint8_t *data, size_t length);

    void parseAdvertisingDataPVVX(uint8_t *data, size_t length);

    void parseAdvertisingDataBTHOME(uint8_t *data, size_t length);

public:
    ATC_MiThermometer(const char *address, Connection_mode connection_mode = ADVERTISING);

    void init();

    void connect();

    void disconnect();

    void readSettings();

    bool isConnected();

    void begin_notify_temp();

    void begin_notify_temp_precise();

    void begin_notify_humidity();

    void begin_notify_battery();

    void connect_to_environment_service();

    void connect_to_battery_service();

    void connect_to_command_service();

    void connect_to_temperature_characteristic();

    void connect_to_temperature_precise_characteristic();

    void connect_to_humidity_characteristic();

    void connect_to_battery_characteristic();

    void connect_to_command_characteristic();

    void connect_to_all_services();

    void connect_to_all_characteristics();

    void begin_notify();

    float getTemperature();

    float getTemperaturePrecise();

    float getHumidity();

    void readTemperature();

    void readTemperaturePrecise();

    void readHumidity();

    uint8_t getBatteryLevel();

    void readBatteryLevel();

    void sendCommand(uint8_t *data, size_t length);

    Advertising_Type getAdvertisingType();

    void parseAdvertisingData(uint8_t *data, size_t length);

    char *getAddress();

    bool get_read_settings() const;

    uint16_t getBatteryVoltage();
};

#endif // ATC_MI_THERMOMETER_NIMBLE_H
