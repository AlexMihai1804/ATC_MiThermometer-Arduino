#ifndef ATC_MI_THERMOMETER_NIMBLE_H
#define ATC_MI_THERMOMETER_NIMBLE_H

#include <NimBLEDevice.h>
#include <Arduino.h>
#include <cstdint>
#include "ATC_MiThermometer_structs.h"
#include "ATC_MiThermometer_enums.h"
#include <ctime>
#include <vector>
#include <map>

constexpr float advertising_interval_step_time_ms = 62.5f;
constexpr uint8_t connect_latency_step_time_ms = 20;
constexpr uint8_t lcd_update_interval_step_time_ms = 50;

class ATC_MiThermometer {
public:
    ATC_MiThermometer(const char *address, Connection_mode connection_mode = Connection_mode::ADVERTISING);

    ~ATC_MiThermometer();

    Connection_mode getConnectionMode() const;

    void setConnectionMode(Connection_mode new_connection_mode);

    void init();

    void connect();

    void disconnect();

    void readSettings();

    bool isConnected() const;

    void beginNotifyTemp();

    void stopNotifyTemp();

    void beginNotifyTempPrecise();

    void stopNotifyTempPrecise();

    void beginNotifyHumidity();

    void stopNotifyHumidity();

    void beginNotifyBattery();

    void stopNotifyBattery();

    void connectToAllServices();

    void connectToAllCharacteristics();

    void beginNotify();

    void stopNotify();

    float getTemperature();

    float getTemperaturePrecise();

    float getHumidity();

    void readTemperature();

    void readTemperaturePrecise();

    void readHumidity();

    uint8_t getBatteryLevel();

    void readBatteryLevel();

    void sendCommand(const std::vector<uint8_t> &data);

    Advertising_Type getAdvertisingType();

    void parseAdvertisingData(const uint8_t *data, size_t length);

    const char *getAddress() const;

    bool getReadSettings() const;

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

    std::vector<uint8_t> parseSettings(const ATC_MiThermometer_Settings &settingsToParse);

    void sendSettings(const ATC_MiThermometer_Settings &settings);

    ATC_MiThermometer_Settings getSettings();

    void resetSettings();

    void setClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint16_t year);

    void setClock(time_t time);

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
    ATC_MiThermometer_Settings settings;
    Connection_mode connection_mode;

    void
    notifyTempPreciseCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                              bool isNotify);

    void notifyTempCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                            bool isNotify);

    void
    notifyHumidityCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                           bool isNotify);

    void
    notifyBatteryCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                          bool isNotify);

    void
    notifySettingsCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                           bool isNotify);

    void parseAdvertisingDataATC1441(const uint8_t *data, size_t length);

    void parseAdvertisingDataPVVX(const uint8_t *data, size_t length);

    void parseAdvertisingDataBTHOME(const uint8_t *data, size_t length);

    void connectToEnvironmentService();

    void connectToBatteryService();

    void connectToCommandService();

    void connectToTemperatureCharacteristic();

    void connectToTemperaturePreciseCharacteristic();

    void connectToHumidityCharacteristic();

    void connectToBatteryCharacteristic();

    void connectToCommandCharacteristic();

    void readCharacteristicValue(NimBLERemoteCharacteristic *characteristic,
                                 std::function<void(const std::string &)> callback);
};

#endif // ATC_MI_THERMOMETER_NIMBLE_H