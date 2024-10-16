/**
 * @file ATC_MiThermometer.h
 * @brief This file contains the declaration of the ATC_MiThermometer class, which is used to interact with Xiaomi Mijia Bluetooth Thermometers and Hygrometers.
 */
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

/** @brief  Advertising interval step time in milliseconds. */
constexpr float advertising_interval_step_time_ms = 62.5f;
/** @brief Connect latency step time in milliseconds. */
constexpr uint8_t connect_latency_step_time_ms = 20;
/** @brief LCD update interval step time in milliseconds. */
constexpr uint8_t lcd_update_interval_step_time_ms = 50;

/**
 * @class ATC_MiThermometer
 * @brief This class provides an interface for interacting with Xiaomi Mijia Bluetooth Thermometers and Hygrometers.
 */
class ATC_MiThermometer {
public:
    /**
     * @brief Constructor for the ATC_MiThermometer class.
     * @param address The MAC address of the thermometer.
     * @param connection_mode The connection mode to use (ADVERTISING, NOTIFICATION, or CONNECTION). Defaults to ADVERTISING.
     */
    ATC_MiThermometer(const char *address, Connection_mode connection_mode = Connection_mode::ADVERTISING);

    ATC_MiThermometer(std::string address, Connection_mode connection_mode = Connection_mode::ADVERTISING);

    /**
     * @brief Destructor for the ATC_MiThermometer class.
     */
    ~ATC_MiThermometer();

    /**
     * @brief Gets the current connection mode.
     * @return The current connection mode.
     */
    Connection_mode getConnectionMode() const;

    /**
     * @brief Sets the connection mode.
     * @param new_connection_mode The new connection mode to use.
     */
    void setConnectionMode(Connection_mode new_connection_mode);

    /**
     * @brief Initializes the thermometer.  Connects to the device and reads the settings.
     *        If the connection mode is ADVERTISING, it will disconnect after reading settings.
     *        If the connection mode is NOTIFICATION, it will subscribe to notifications.
     *        If the connection mode is CONNECTION, it will read the current values of temperature, humidity, and battery level.
     */
    void init();

    /**
     * @brief Connects to the thermometer.
     */
    void connect();

    /**
     * @brief Disconnects from the thermometer.
     */
    void disconnect();

    /**
     * @brief Reads the settings from the thermometer.
     */
    void readSettings();

    /**
     * @brief Checks if the thermometer is currently connected.
     * @return True if connected, false otherwise.
     */
    bool isConnected() const;

    /**
     * @brief Starts notifications for temperature.
     */
    void beginNotifyTemp();

    /**
     * @brief Stops notifications for temperature.
     */
    void stopNotifyTemp();

    /**
     * @brief Starts notifications for precise temperature.
     */
    void beginNotifyTempPrecise();

    /**
     * @brief Stops notifications for precise temperature.
     */
    void stopNotifyTempPrecise();

    /**
     * @brief Starts notifications for humidity.
     */
    void beginNotifyHumidity();

    /**
     * @brief Stops notifications for humidity.
     */
    void stopNotifyHumidity();

    /**
     * @brief Starts notifications for battery level.
     */
    void beginNotifyBattery();

    /**
     * @brief Stops notifications for battery level.
     */
    void stopNotifyBattery();

    /**
     * @brief Connects to all available services.
     */
    void connectToAllServices();

    /**
     * @brief Connects to all available characteristics.
     */
    void connectToAllCharacteristics();

    /**
     * @brief Starts notifications for all available characteristics.
     */
    void beginNotify();

    /**
     * @brief Stops notifications for all available characteristics.
     */
    void stopNotify();

    /**
     * @brief Gets the temperature from the thermometer.  Handles different advertising types and connection modes.
     * @return The temperature in degrees Celsius.
     */
    float getTemperature();

    /**
     * @brief Gets the precise temperature from the thermometer. Handles different advertising types and connection modes.
     * @return The precise temperature in degrees Celsius.
     */
    float getTemperaturePrecise();

    /**
     * @brief Gets the humidity from the thermometer.
     * @return The humidity in percentage.
     */
    float getHumidity();

    /**
     * @brief Reads the temperature from the thermometer, using a connection if necessary.
     */
    void readTemperature();

    /**
     * @brief Reads the precise temperature from the thermometer, using a connection if necessary.
     */
    void readTemperaturePrecise();

    /**
     * @brief Reads the humidity from the thermometer, using a connection if necessary.
     */
    void readHumidity();

    /**
     * @brief Gets the battery level from the thermometer.
     * @return The battery level in percentage.
     */
    uint8_t getBatteryLevel();

    /**
     * @brief Reads the battery level from the thermometer, using a connection if necessary.
     */
    void readBatteryLevel();

    /**
     * @brief Sends a command to the thermometer.
     * @param data The command data to send.
     */
    void sendCommand(const std::vector<uint8_t> &data);

    /**
     * @brief Gets the advertising type of the thermometer.
     * @return The advertising type.
     */
    Advertising_Type getAdvertisingType();

    /**
     * @brief Parses the advertising data from the thermometer.
     * @param data The advertising data.
     * @param length The length of the advertising data.
     */
    void parseAdvertisingData(const uint8_t *data, size_t length);

    /**
     * @brief Gets the MAC address of the thermometer.
     * @return The MAC address.
     */
    const char *getAddress() const;

    std::string getAddressString() const;

    /**
    * @brief returns if the settings have been read from the device
    * @return true if the settings have been read
    */
    bool getReadSettings() const;

    /**
     * @brief Gets the battery voltage from the thermometer.
     * @return The battery voltage in millivolts.
     */
    uint16_t getBatteryVoltage();

    /**
     * @brief  Gets the RF TX Power.
     * @return RF TX Power as RF_TX_Power enum.
     */
    RF_TX_Power getRfTxPower();

    /**
     * @brief Sets the RF TX Power.
     * @param power The RF TX power to set.
     */
    void setRfTxPower(RF_TX_Power power);

    /**
     * @brief Gets the RF TX Power in dBm.
     * @return RF TX Power in dBm.
     */
    float getRfTxPowerdBm();

    /**
     * @brief Sets the RF TX Power in dBm.  Finds the closest matching enum value.
     * @param power The RF TX power to set in dBm.
     */
    void setRfTxPowerdBm(float power);

    /**
     * @brief Gets the low power measures setting.
     * @return True if low power measures are enabled, false otherwise.
     */
    bool getLowPowerMeasures();

    /**
     * @brief Sets the low power measures setting.
     * @param lowPowerMeasures  True to enable low power measures, false to disable.
     */
    void setLowPowerMeasures(bool lowPowerMeasures);

    /**
     * @brief Get the transmit measures setting.
     * @return  True if transmit measures are enabled, false otherwise.
     */
    bool getTransmitMeasures();

    /**
     * @brief Sets the transmit measures setting.
     * @param transmitMeasures True to enable transmit measures, false to disable.
     */
    void setTransmitMeasures(bool transmitMeasures);

    /**
     * @brief Get the show battery setting.
     * @return  True if show battery is enabled, false otherwise.
     */
    bool getShowBattery();

    /**
     * @brief Sets the show battery setting.
     * @param showBattery True to enable show battery, false to disable.
     */
    void setShowBattery(bool showBattery);

    /**
     * @brief Get the temperature unit setting (Fahrenheit or Celsius).
     * @return True if Fahrenheit, false if Celsius.
     */
    bool getTempFOrC();

    /**
     * @brief Sets the temperature unit setting.
     * @param tempFOrC True for Fahrenheit, false for Celsius.
     */
    void setTempFOrC(bool tempFOrC);

    /**
     * @brief Get the blinking time smile setting.
     * @return  True if blinking time smile is enabled, false otherwise.
     */
    bool getBlinkingTimeSmile();

    /**
     * @brief Sets the blinking time smile setting.
     * @param blinkingTimeSmile True to enable blinking time smile, false to disable.
     */
    void setBlinkingTimeSmile(bool blinkingTimeSmile);

    /**
     * @brief Get the comfort smiley setting.
     * @return  True if comfort smiley is enabled, false otherwise.
     */
    bool getComfortSmiley();

    /**
     * @brief Sets the comfort smiley setting.
     * @param comfortSmiley True to enable comfort smiley, false to disable.
     */
    void setComfortSmiley(bool comfortSmiley);

    /**
     * @brief Get the advertising crypto setting.
     * @return  True if advertising crypto is enabled, false otherwise.
     */
    bool getAdvCrypto();

    /**
     * @brief Sets the advertising crypto setting.
     * @param advCrypto True to enable advertising crypto, false to disable.
     */
    void setAdvCrypto(bool advCrypto);

    /**
     * @brief Get the advertising flags setting.
     * @return  True if advertising flags is enabled, false otherwise.
     */
    bool getAdvFlags();

    /**
     * @brief Sets the advertising flags setting.
     * @param advFlags True to enable advertising flags, false to disable.
     */
    void setAdvFlags(bool advFlags);

    /**
     * @brief Get the current smiley setting.
     * @return The current smiley.
     */
    Smiley getSmiley();

    /**
     * @brief Set the smiley.
     * @param smiley The smiley to set.
     */
    void setSmiley(Smiley smiley);

    /**
     * @brief  Get the BT5 PHY setting.
     * @return  True if BT5 PHY is enabled, false otherwise.
     */
    bool getBT5PHY();

    /**
     * @brief Sets the BT5 PHY setting.
     * @param BT5PHY  True to enable BT5 PHY, false to disable.
     */
    void setBT5PHY(bool BT5PHY);

    /**
     * @brief  Get the long range setting.
     * @return  True if long range is enabled, false otherwise.
     */
    bool getLongRange();

    /**
     * @brief Sets the long range setting.
     * @param longRange  True to enable long range, false to disable.
     */
    void setLongRange(bool longRange);

    /**
     * @brief Get the screen off setting.
     * @return  True if screen off is enabled, false otherwise.
     */
    bool getScreenOff();

    /**
     * @brief Sets the screen off setting.
     * @param screenOff True to enable screen off, false to disable.
     */
    void setScreenOff(bool screenOff);

    /**
     * @brief Gets the temperature offset.
     * @return  The temperature offset.
     */
    float getTempOffset();

    /**
     * @brief Sets the temperature offset.
     * @param tempOffset The temperature offset to set.
     */
    void setTempOffset(float tempOffset);

    /**
     * @brief Gets the humidity offset.
     * @return The humidity offset.
     */
    float getHumidityOffset();

    /**
     * @brief Sets the humidity offset.
     * @param humidityOffset  The humidity offset to set.
     */
    void setHumidityOffset(float humidityOffset);

    /**
     * @brief Gets the calibrated temperature offset.
     * @return  The calibrated temperature offset.
     */
    int8_t getTempOffsetCal();

    /**
     * @brief Sets the calibrated temperature offset.
     * @param tempOffsetCal The calibrated temperature offset to set.
     */
    void setTempOffsetCal(int8_t tempOffsetCal);

    /**
     * @brief Gets the calibrated humidity offset.
     * @return  The calibrated humidity offset.
     */
    int8_t getHumidityOffsetCal();

    /**
     * @brief Sets the calibrated humidity offset.
     * @param humidityOffsetCal  The calibrated humidity offset to set.
     */
    void setHumidityOffsetCal(int8_t humidityOffsetCal);

    /**
     * @brief Gets the advertising interval in steps.
     * @return The advertising interval in steps.
     */
    uint8_t getAdvertisingIntervalSteps();

    /**
     * @brief Sets the advertising interval in steps.
     * @param advertisingIntervalSteps The advertising interval in steps to set.
     */
    void setAdvertisingIntervalSteps(uint8_t advertisingIntervalSteps);

    /**
     * @brief  Gets the advertising interval in milliseconds.
     * @return  The advertising interval in milliseconds.
     */
    uint16_t getAdvertisingIntervalMs();

    /**
     * @brief  Sets the advertising interval in milliseconds.
     * @param advertisingIntervalMs The advertising interval in milliseconds to set.
     */
    void setAdvertisingIntervalMs(uint16_t advertisingIntervalMs);

    /**
     * @brief Gets the measure interval in steps.
     * @return The measure interval in steps.
     */
    uint8_t getMeasureIntervalSteps();

    /**
     * @brief Sets the measure interval in steps.
     * @param measureIntervalSteps  The measure interval in steps to set.
     */
    void setMeasureIntervalSteps(uint8_t measureIntervalSteps);

    /**
     * @brief Gets the measure interval in milliseconds.
     * @return The measure interval in milliseconds.
     */
    uint32_t getMeasureIntervalMs();

    /**
     * @brief Sets the measure interval in milliseconds.
     * @param measureIntervalMs  The measure interval in milliseconds to set.
     */
    void setMeasureIntervalMs(uint32_t measureIntervalMs);

    /**
     * @brief Gets the connect latency in steps.
     * @return  The connect latency in steps.
     */
    uint8_t getConnectLatencySteps();

    /**
     * @brief  Sets the connect latency in steps.
     * @param connectLatencySteps The connect latency in steps to set.
     */
    void setConnectLatencySteps(uint8_t connectLatencySteps);

    /**
     * @brief  Gets the connect latency in milliseconds.
     * @return  The connect latency in milliseconds.
     */
    uint16_t getConnectLatencyMs();

    /**
     * @brief Sets the connect latency in milliseconds.
     * @param connectLatencyMs The connect latency in milliseconds to set.
     */
    void setConnectLatencyMs(uint16_t connectLatencyMs);

    /**
     * @brief Gets the LCD update interval in steps.
     * @return The LCD update interval in steps.
     */
    uint8_t getLcdUpdateIntervalSteps();

    /**
     * @brief Sets the LCD update interval in steps.
     * @param lcdUpdateIntervalSteps The LCD update interval in steps to set.
     */
    void setLcdUpdateIntervalSteps(uint8_t lcdUpdateIntervalSteps);

    /**
     * @brief Gets the LCD update interval in milliseconds.
     * @return The LCD update interval in milliseconds.
     */
    uint16_t getLcdUpdateIntervalMs();

    /**
     * @brief  Sets the LCD Update interval in milliseconds.
     * @param lcdUpdateIntervalMs The LCD update interval in milliseconds to set.
     */
    void setLcdUpdateIntervalMs(uint16_t lcdUpdateIntervalMs);

    /**
     * @brief  Gets the hardware version of the thermometer.
     * @return The hardware version ID.
     */
    HW_VERSION_ID getHwVersion();

    /**
     * @brief Gets the number of measurements to average in steps.
     * @return  The number of measurements to average in steps.
     */
    uint8_t getAveragingMeasurementsSteps();

    /**
     * @brief Sets the number of measurements to average in steps.
     * @param averagingMeasurementsSteps  The number of measurements to average in steps to set.
     */
    void setAveragingMeasurementsSteps(uint8_t averagingMeasurementsSteps);

    /**
     * @brief  Gets the averaging measurements time in milliseconds.
     * @return The averaging measurements time in milliseconds.
     */
    uint32_t getAveragingMeasurementsMs();

    /**
     * @brief  Sets the averaging measurements time in milliseconds.
     * @param averagingMeasurementsMs The averaging measurements time in milliseconds to set.
     */
    void setAveragingMeasurementsMs(uint32_t averagingMeasurementsMs);

    /**
     * @brief  Gets the averaging measurements time in seconds.
     * @return The averaging measurements time in seconds.
     */
    uint16_t getAveragingMeasurementsSec();

    /**
     * @brief Sets the averaging measurements time in seconds.
     * @param averagingMeasurementsSec The averaging measurements time in seconds to set.
     */
    void setAveragingMeasurementsSec(uint16_t averagingMeasurementsSec);

    /**
     * @brief Parses the given settings into a byte array suitable for sending to the device.
     * @param settingsToParse The settings to parse.
     * @return A vector of bytes representing the parsed settings.
     */
    std::vector<uint8_t> parseSettings(const ATC_MiThermometer_Settings &settingsToParse);

    /**
     * @brief Sends the given settings to the thermometer.
     * @param settings The settings to send.
     */
    void sendSettings(const ATC_MiThermometer_Settings &settings);

    /**
     * @brief Gets the current settings of the thermometer.
     * @return The current settings.
     */
    ATC_MiThermometer_Settings getSettings();

    /**
     * @brief Resets the settings of the thermometer to default values.
     */
    void resetSettings();

    /**
     * @brief Sets the clock on the thermometer.
     * @param hours The hour to set (0-23).
     * @param minutes  The minute to set (0-59).
     * @param seconds The second to set (0-59).
     * @param day  The day of the month to set (1-31).
     * @param month The month to set (1-12).
     * @param year The year to set (e.g., 2024).
     */
    void setClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint16_t year);

    /**
     * @brief Sets the clock on the thermometer using a time_t value.
     * @param time The time_t value representing the time to set.
     */
    void setClock(time_t time);

    bool getTimeTracking() const;

    void setTimeTracking(bool timeTracking);

    time_t getLastReadTime() const;

private:
    std::string address; /**< The MAC address of the thermometer. */
    NimBLEClient *pClient; /**< Pointer to the BLE client. */
    NimBLERemoteService *environmentService; /**< Pointer to the environment service. */
    NimBLERemoteService *batteryService; /**< Pointer to the battery service. */
    NimBLERemoteService *commandService; /**< Pointer to the command service. */
    NimBLERemoteCharacteristic *temperatureCharacteristic; /**< Pointer to the temperature characteristic. */
    NimBLERemoteCharacteristic *temperaturePreciseCharacteristic; /**< Pointer to the precise temperature characteristic. */
    NimBLERemoteCharacteristic *humidityCharacteristic; /**< Pointer to the humidity characteristic. */
    NimBLERemoteCharacteristic *batteryCharacteristic; /**< Pointer to the battery characteristic. */
    NimBLERemoteCharacteristic *commandCharacteristic; /**< Pointer to the command characteristic. */
    bool received_settings; /**< Flag indicating whether settings have been received. */
    bool read_settings; /**< Flag indicating whether settings have been read. */
    bool started_notify_temp; /**< Flag indicating whether temperature notifications have been started. */
    bool started_notify_temp_precise; /**< Flag indicating whether precise temperature notifications have been started. */
    bool started_notify_humidity; /**< Flag indicating whether humidity notifications have been started. */
    bool started_notify_battery; /**< Flag indicating whether battery notifications have been started. */
    float temperature; /**< The current temperature. */
    float temperature_precise; /**< The current precise temperature. */
    float humidity; /**< The current humidity. */
    uint16_t battery_mv; /**< The current battery voltage in mV */
    uint8_t battery_level; /**< The current battery level. */
    ATC_MiThermometer_Settings settings; /**< The current settings of the thermometer. */
    Connection_mode connection_mode; /**< The connection mode being used. */
    time_t last_read_time; /**< The last time the thermometer was read. */
    bool time_tracking; /**< Flag indicating whether time tracking is enabled. */
    /**
     * @brief Callback function for precise temperature notifications.
     * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
     * @param pData Pointer to the notification data.
     * @param length Length of the notification data.
     * @param isNotify True if this is a notification, false otherwise.
     */
    void
    notifyTempPreciseCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                              bool isNotify);

    /**
     * @brief Callback function for temperature notifications.
     * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
     * @param pData  Pointer to the notification data.
     * @param length  Length of the notification data.
     * @param isNotify True if this is a notification, false otherwise.
     */
    void notifyTempCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                            bool isNotify);

    /**
     * @brief Callback function for humidity notifications.
     * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
     * @param pData  Pointer to the notification data.
     * @param length Length of the notification data.
     * @param isNotify True if this is a notification, false otherwise.
     */
    void
    notifyHumidityCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                           bool isNotify);

    /**
     * @brief Callback function for battery level notifications.
     * @param pBLERemoteCharacteristic Pointer to the characteristic that triggered the notification.
     * @param pData Pointer to the notification data.
     * @param length Length of the notification data.
     * @param isNotify True if this is a notification, false otherwise.
     */
    void
    notifyBatteryCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                          bool isNotify);

    /**
     * @brief Callback function for settings notifications.
     * @param pBLERemoteCharacteristic  Pointer to the characteristic that triggered the notification.
     * @param pData  Pointer to the notification data.
     * @param length  Length of the notification data.
     * @param isNotify True if this is a notification, false otherwise.
     */
    void
    notifySettingsCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, const uint8_t *pData, size_t length,
                           bool isNotify);

    /**
     * @brief Parses advertising data specifically for ATC1441 format.
     * @param data  Pointer to the advertising data.
     * @param length Length of the advertising data.
     */
    void parseAdvertisingDataATC1441(const uint8_t *data, size_t length);

    /**
     * @brief Parses advertising data specifically for PVVX format.
     * @param data Pointer to the advertising data.
     * @param length  Length of the advertising data.
     */
    void parseAdvertisingDataPVVX(const uint8_t *data, size_t length);

    /**
     * @brief  Parses advertising data specifically for BTHome format.
     * @param data Pointer to the advertising data.
     * @param length Length of the advertising data.
     */
    void parseAdvertisingDataBTHOME(const uint8_t *data, size_t length);

    /**
     * @brief  Connects to the environment service.
     */
    void connectToEnvironmentService();

    /**
     * @brief Connects to the battery service.
     */
    void connectToBatteryService();

    /**
     * @brief Connects to the command service.
     */
    void connectToCommandService();

    /**
     * @brief Connects to the temperature characteristic.
     */
    void connectToTemperatureCharacteristic();

    /**
     * @brief Connects to the precise temperature characteristic.
     */
    void connectToTemperaturePreciseCharacteristic();

    /**
     * @brief Connects to the humidity characteristic.
     */
    void connectToHumidityCharacteristic();

    /**
     * @brief Connects to the battery characteristic.
     */
    void connectToBatteryCharacteristic();

    /**
     * @brief Connects to the command characteristic.
     */
    void connectToCommandCharacteristic();

    /**
     * @brief Reads the value of a characteristic and calls the provided callback with the result.
     *
     * @param characteristic A pointer to the NimBLERemoteCharacteristic to read.
     * @param callback A function to call with the read value. The value is passed as a std::string.
     */
    void readCharacteristicValue(NimBLERemoteCharacteristic *characteristic,
                                 std::function<void(const std::string &)> callback);
};

#endif