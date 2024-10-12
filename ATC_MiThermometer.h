#ifndef ATC_MI_THERMOMETER_NIMBLE_H
#define ATC_MI_THERMOMETER_NIMBLE_H

#include <NimBLEDevice.h>
#include <Arduino.h>
#include <cstdint>
#include "ATC_MiThermometer_structs.h"
#include "ATC_MiThermometer_enums.h"
#include <ctime>

const float advertising_interval_step_time_ms = 62.5;
const uint8_t connect_latency_step_time_ms = 20;
const uint8_t lcd_update_interval_step_time_ms = 50;

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

    Connection_mode getConnectionMode();

    void setConnectionMode(Connection_mode new_connection_mode);

    void init();

    void connect();

    void disconnect();

    void readSettings();

    bool isConnected();

    void begin_notify_temp();

    void stop_notify_temp();

    void begin_notify_temp_precise();

    void stop_notify_temp_precise();

    void begin_notify_humidity();

    void stop_notify_humidity();

    void begin_notify_battery();

    void stop_notify_battery();

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

    void stop_notify();

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

    RF_TX_Power getRfTxPower();

    void setRfTxPower(RF_TX_Power power);

    float getRfTxPowerdBm();

    void setRfTxPowerdBm(float power);

    bool getLowPowerMeasures();

    void setLowPowerMeasures(bool lowPowerMeasures);

    bool getTransmitMeasures();

    void setTransmitMeasures(bool transmitMeasures);

    bool getShowBattery();

    void setShowBattery(bool showBattery);

    bool getTempFOrC();

    void setTempFOrC(bool tempFOrC);

    bool getBlinkingTimeSmile();

    void setBlinkingTimeSmile(bool blinkingTimeSmile);

    bool getComfortSmiley();

    void setComfortSmiley(bool comfortSmiley);

    bool getAdvCrypto();

    void setAdvCrypto(bool advCrypto);

    bool getAdvFlags();

    void setAdvFlags(bool advFlags);

    Smiley getSmiley();

    void setSmiley(Smiley smiley);

    bool getBT5PHY();

    void setBT5PHY(bool BT5PHY);

    bool getLongRange();

    void setLongRange(bool longRange);

    bool getScreenOff();

    void setScreenOff(bool screenOff);

    float getTempOffset();

    void setTempOffset(float tempOffset);

    float getHumidityOffset();

    void setHumidityOffset(float humidityOffset);

    int8_t getTempOffsetCal();

    void setTempOffsetCal(int8_t tempOffsetCal);

    int8_t getHumidityOffsetCal();

    void setHumidityOffsetCal(int8_t humidityOffsetCal);

    uint8_t getAdvertisingIntervalSteps();

    void setAdvertisingIntervalSteps(uint8_t advertisingIntervalSteps);

    uint16_t getAdvertisingIntervalMs();

    void setAdvertisingIntervalMs(uint16_t advertisingIntervalMs);

    uint8_t getMeasureIntervalSteps();

    void setMeasureIntervalSteps(uint8_t measureIntervalSteps);

    uint32_t getMeasureIntervalMs();

    void setMeasureIntervalMs(uint32_t measureIntervalMs);

    uint8_t getConnectLatencySteps();

    void setConnectLatencySteps(uint8_t connectLatencySteps);

    uint16_t getConnectLatencyMs();

    void setConnectLatencyMs(uint16_t connectLatencyMs);

    uint8_t getLcdUpdateIntervalSteps();

    void setLcdUpdateIntervalSteps(uint8_t lcdUpdateIntervalSteps);

    uint16_t getLcdUpdateIntervalMs();

    void setLcdUpdateIntervalMs(uint16_t lcdUpdateIntervalMs);

    HW_VERSION_ID getHwVersion();

    uint8_t getAveragingMeasurementsSteps();

    void setAveragingMeasurementsSteps(uint8_t averagingMeasurementsSteps);

    uint32_t getAveragingMeasurementsMs();

    void setAveragingMeasurementsMs(uint32_t averagingMeasurementsMs);

    uint16_t getAveragingMeasurementsSec();

    void setAveragingMeasurementsSec(uint16_t averagingMeasurementsSec);

    uint8_t *parseSettings(ATC_MiThermometer_Settings settingsToParse);

    void sendSettings(ATC_MiThermometer_Settings settings);

    ATC_MiThermometer_Settings getSettings();

    void resetSettings();

    void setClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint16_t year);

    void setClock(time_t time);
};

#endif // ATC_MI_THERMOMETER_NIMBLE_H
