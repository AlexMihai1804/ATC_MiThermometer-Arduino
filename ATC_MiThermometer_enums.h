#ifndef ATC_MI_TERMOMETER_ESP32_ATC_MITHERMOMETER_ENUMS_H
#define ATC_MI_TERMOMETER_ESP32_ATC_MITHERMOMETER_ENUMS_H
enum Advertising_Type {
    ATC1441 = 0,
    PVVX = 1,
    XIAOMI = 2,
    BTHOME = 3,
};
enum Connection_mode {
    ADVERTISING = 0,
    NOTIFICATION = 1,
    CONNECTION = 2,
};
enum Smiley {
    SMILEY_OFF = 0,
    SMILEY_HAPPY = 1,
    SMILEY_SAD = 2,
    SMILEY_OOO = 3,
    SMILEY_P_OFF = 4,
    SMILEY_P_HAPPY = 5,
    SMILEY_P_SAD = 6,
    SMILEY_P_OOO = 7,
};

enum HW_VERSION_ID {
    HW_VER_LYWSD03MMC_B14 = 0,    // LYWSD03MMC B1.4
    HW_VER_MHO_C401 = 1,          // MHO-C401 2020
    HW_VER_CGG1 = 2,              // CGG1 2020/2021
    HW_VER_LYWSD03MMC_B19 = 3,    // LYWSD03MMC B1.9
    HW_VER_LYWSD03MMC_B16 = 4,    // LYWSD03MMC B1.6
    HW_VER_LYWSD03MMC_B17 = 5,    // LYWSD03MMC B1.7
    HW_VER_CGDK2 = 6,             // CGDK2
    HW_VER_CGG1_2022 = 7,         // CGG1 2022
    HW_VER_MHO_C401_2022 = 8,     // MHO-C401N 2022
    HW_VER_MJWSD05MMC = 9,        // MJWSD05MMC
    HW_VER_LYWSD03MMC_B15 = 10,   // LYWSD03MMC B1.5
    HW_VER_MHO_C122 = 11,         // MHO-C122
    HW_VER_TB03F = 16,            // DIY TB-03F-Kit
    HW_VER_TS0201 = 17,           // ZigBee TS0201
    HW_VER_TNK01 = 18,            // DIY - modul PB-03F, Water tank controller
    HW_VER_TH03Z = 22,            // ZigBee TH03Z
    HW_VER_ZTH01 = 27,            // ZigBee ZTH01
    HW_VER_ZTH02 = 28,            // ZigBee ZTH02
    HW_VER_PLM1 = 29,             // Tuya BLE Plant Monitor ECF-SGS01-A rev1.3
    HW_VER_EXTENDED = 15          // DIY - hardware extensibil
};

enum class RF_TX_Power {
    dBm_3_01 = 191,
    dBm_2_81 = 189,
    dBm_2_61 = 187,
    dBm_2_39 = 185,
    dBm_1_99 = 182,
    dBm_1_73 = 180,
    dBm_1_45 = 178,
    dBm_1_17 = 176,
    dBm_0_90 = 174,
    dBm_0_58 = 172,
    dBm_0_04 = 169,
    dBm_n0_14 = 168,
    dBm_n0_97 = 164,
    dBm_n1_42 = 162,
    dBm_n1_89 = 160,
    dBm_n2_48 = 158,
    dBm_n3_03 = 156,
    dBm_n3_61 = 154,
    dBm_n4_26 = 152,
    dBm_n5_03 = 150,
    dBm_n5_81 = 148,
    dBm_n6_67 = 146,
    dBm_n7_65 = 144,
    dBm_n8_65 = 142,
    dBm_n9_89 = 140,
    dBm_n11_4 = 138,
    dBm_n13_29 = 136,
    dBm_n15_88 = 134,
    dBm_n19_27 = 132,
    dBm_n25_18 = 130,
    dBm_n30 = 255,
    dBm_n50 = 128,
    dBm_10_46 = 63,
    dBm_10_29 = 61,
    dBm_10_01 = 58,
    dBm_9_81 = 56,
    dBm_9_48 = 53,
    dBm_9_24 = 51,
    dBm_8_97 = 49,
    dBm_8_73 = 47,
    dBm_8_44 = 45,
    dBm_8_13 = 43,
    dBm_7_79 = 41,
    dBm_7_41 = 39,
    dBm_7_02 = 37,
    dBm_6_60 = 35,
    dBm_6_14 = 33,
    dBm_5_65 = 31,
    dBm_5_13 = 29,
    dBm_4_57 = 27,
    dBm_3_94 = 25,
    dBm_3_23 = 23
};
#endif
