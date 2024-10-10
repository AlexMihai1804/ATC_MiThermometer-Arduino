#ifndef ATC_MI_TERMOMETER_ESP32_ATC_MITHERMOMETER_STRUCTS_H
#define ATC_MI_TERMOMETER_ESP32_ATC_MITHERMOMETER_STRUCTS_H

#include <cstdint>
#include "ATC_MiThermometer_enums.h"

struct ATC_MiThermometer_Settings {
    bool lp_measures;
    bool tx_measures;
    bool show_battery;
    bool temp_F_or_C;
    bool blinking_time_smile;
    bool comfort_smiley;
    Advertising_Type advertising_type;
    Smiley smiley;
    bool adv_crypto;
    bool adv_flags;
    bool bt5phy;
    bool long_range;
    bool screen_off;
    float temp_offset;
    float humidity_offset;
    int8_t temp_offset_cal;
    int8_t humidity_offset_cal;
    uint8_t advertising_interval;
    uint8_t measure_interval;
    RF_TX_Power rfTxPower;
    uint8_t connect_latency;
    uint8_t lcd_update_interval;
    HW_VERSION_ID hw_version;
    uint8_t averaging_measurements;
};
#endif
