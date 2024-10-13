/**
 * @file ATC_MiThermometer_structs.h
 * @brief This file defines the structures used by the ATC_MiThermometer library.
 */
#ifndef ATC_MI_THERMOMETER_STRUCTS_H
#define ATC_MI_THERMOMETER_STRUCTS_H

#include <cstdint>
#include "ATC_MiThermometer_enums.h"

/**
 * @struct ATC_MiThermometer_Settings
 * @brief This structure holds the settings for the ATC_MiThermometer.
 */
struct ATC_MiThermometer_Settings {
    bool lp_measures; /**< True if low power measures are enabled, false otherwise. */
    bool tx_measures; /**< True if transmit measures are enabled, false otherwise. */
    bool show_battery; /**< True if battery percentage should be shown, false otherwise. */
    bool temp_F_or_C; /**< True for Fahrenheit, false for Celsius. */
    bool blinking_time_smile; /**< True if blinking time smile is enabled, false otherwise. */
    bool comfort_smiley; /**< True if comfort smiley is enabled, false otherwise. */
    Advertising_Type advertising_type; /**< The advertising type. */
    Smiley smiley; /**< The current smiley setting. */
    bool adv_crypto; /**< True if advertisement encryption is enabled, false otherwise. */
    bool adv_flags; /**< True if advertisement flags are enabled, false otherwise. */
    bool bt5phy; /**< True if Bluetooth 5 PHY is enabled, false otherwise. */
    bool long_range; /**< True if long range mode is enabled, false otherwise. */
    bool screen_off;  /**< True if screen is off, false otherwise. */
    float temp_offset; /**< The temperature offset. */
    float humidity_offset; /**< The humidity offset. */
    int8_t temp_offset_cal; /**< The calibrated temperature offset. */
    int8_t humidity_offset_cal; /**< The calibrated humidity offset. */
    uint8_t advertising_interval; /**< The advertising interval in steps. */
    uint8_t measure_interval; /**< The measure interval in steps. */
    RF_TX_Power rfTxPower; /**< The RF TX power level. */
    uint8_t connect_latency; /**< The connection latency in steps. */
    uint8_t lcd_update_interval; /**< The LCD update interval in steps. */
    HW_VERSION_ID hw_version; /**< The hardware version ID. */
    uint8_t averaging_measurements; /**< Number of measurements for averaging. */
};
#endif // ATC_MI_THERMOMETER_STRUCTS_H