/**
 * @file ATC_MiThermometer_enums.h
 * @brief This file contains the enumerations used by the ATC_MiThermometer library.
 */

#ifndef ATC_MI_THERMOMETER_ENUMS_H
#define ATC_MI_THERMOMETER_ENUMS_H

/**
 * @enum Advertising_Type
 * @brief This enum represents the different advertising types used by the thermometer.
 */
enum class Advertising_Type {
    ATC1441 = 0, /**< ATC1441 advertising type. */
    PVVX = 1, /**< PVVX advertising type. */
    XIAOMI = 2, /**< XIAOMI advertising type. */
    BTHOME = 3, /**< BTHOME advertising type. */
};

/**
 * @enum Connection_mode
 * @brief This enum represents the different connection modes used by the library.
 */
enum class Connection_mode {
    ADVERTISING = 0, /**<  Connects to the device only to read the settings and parses advertising data. */
    NOTIFICATION = 1, /**<  Connects to the device and subscribes to notifications. */
    CONNECTION = 2, /**<  Maintains a connection to the device and reads data on demand. */
};

/**
 * @enum Smiley
 * @brief This enum represents the different smiley states that can be displayed on the thermometer.
 */
enum class Smiley {
    SMILEY_OFF = 0, /**< Smiley off. */
    SMILEY_HAPPY = 1, /**< Happy smiley. */
    SMILEY_SAD = 2, /**< Sad smiley. */
    SMILEY_OOO = 3, /**< OOO smiley. */
    SMILEY_P_OFF = 4, /**< Smiley with percentage off. */
    SMILEY_P_HAPPY = 5, /**< Happy smiley with percentage. */
    SMILEY_P_SAD = 6, /**< Sad smiley with percentage. */
    SMILEY_P_OOO = 7, /**< OOO smiley with percentage. */
};

/**
 * @enum HW_VERSION_ID
 * @brief  This enum represents the different hardware versions of the thermometer.
 */
enum class HW_VERSION_ID {
    HW_VER_LYWSD03MMC_B14 = 0,  /**< LYWSD03MMC hardware version B14. */
    HW_VER_MHO_C401 = 1, /**< MHO-C401 hardware version. */
    HW_VER_CGG1 = 2,  /**< CGG1 hardware version. */
    HW_VER_LYWSD03MMC_B19 = 3, /**< LYWSD03MMC hardware version B19. */
    HW_VER_LYWSD03MMC_B16 = 4, /**< LYWSD03MMC hardware version B16. */
    HW_VER_LYWSD03MMC_B17 = 5, /**< LYWSD03MMC hardware version B17. */
    HW_VER_CGDK2 = 6, /**< CGDK2 hardware version. */
    HW_VER_CGG1_2022 = 7, /**< CGG1 2022 hardware version. */
    HW_VER_MHO_C401_2022 = 8, /**< MHO-C401 2022 hardware version. */
    HW_VER_MJWSD05MMC = 9,  /**< MJWSD05MMC hardware version. */
    HW_VER_LYWSD03MMC_B15 = 10, /**< LYWSD03MMC hardware version B15. */
    HW_VER_MHO_C122 = 11, /**< MHO-C122 hardware version. */
    HW_VER_TB03F = 16,  /**< TB03F hardware version. */
    HW_VER_TS0201 = 17, /**< TS0201 hardware version. */
    HW_VER_TNK01 = 18, /**< TNK01 hardware version. */
    HW_VER_TH03Z = 22, /**< TH03Z hardware version. */
    HW_VER_ZTH01 = 27, /**< ZTH01 hardware version. */
    HW_VER_ZTH02 = 28, /**< ZTH02 hardware version. */
    HW_VER_PLM1 = 29, /**< PLM1 hardware version. */
    HW_VER_EXTENDED = 15  /**< Extended hardware version. */
};

/**
 * @enum RF_TX_Power
 * @brief This enum represents the different RF TX power levels of the thermometer.
 */
enum class RF_TX_Power {
    dBm_3_01 = 191, /**< 3.01 dBm. */
    dBm_2_81 = 189, /**< 2.81 dBm. */
    dBm_2_61 = 187, /**< 2.61 dBm. */
    dBm_2_39 = 185, /**< 2.39 dBm. */
    dBm_1_99 = 182, /**< 1.99 dBm. */
    dBm_1_73 = 180, /**< 1.73 dBm. */
    dBm_1_45 = 178, /**< 1.45 dBm. */
    dBm_1_17 = 176, /**< 1.17 dBm. */
    dBm_0_90 = 174, /**< 0.90 dBm. */
    dBm_0_58 = 172, /**< 0.58 dBm. */
    dBm_0_04 = 169, /**< 0.04 dBm. */
    dBm_n0_14 = 168, /**< -0.14 dBm. */
    dBm_n0_97 = 164, /**< -0.97 dBm. */
    dBm_n1_42 = 162, /**< -1.42 dBm. */
    dBm_n1_89 = 160, /**< -1.89 dBm. */
    dBm_n2_48 = 158, /**< -2.48 dBm. */
    dBm_n3_03 = 156, /**< -3.03 dBm. */
    dBm_n3_61 = 154, /**< -3.61 dBm. */
    dBm_n4_26 = 152, /**< -4.26 dBm. */
    dBm_n5_03 = 150, /**< -5.03 dBm. */
    dBm_n5_81 = 148, /**< -5.81 dBm. */
    dBm_n6_67 = 146, /**< -6.67 dBm. */
    dBm_n7_65 = 144, /**< -7.65 dBm. */
    dBm_n8_65 = 142, /**< -8.65 dBm. */
    dBm_n9_89 = 140, /**< -9.89 dBm. */
    dBm_n11_4 = 138, /**< -11.4 dBm. */
    dBm_n13_29 = 136, /**< -13.29 dBm. */
    dBm_n15_88 = 134, /**< -15.88 dBm. */
    dBm_n19_27 = 132, /**< -19.27 dBm. */
    dBm_n25_18 = 130, /**< -25.18 dBm. */
    dBm_n30 = 255, /**< -30 dBm. */
    dBm_n50 = 128, /**< -50 dBm. */
    dBm_10_46 = 63, /**< 10.46 dBm. */
    dBm_10_29 = 61, /**< 10.29 dBm. */
    dBm_10_01 = 58, /**< 10.01 dBm. */
    dBm_9_81 = 56, /**< 9.81 dBm. */
    dBm_9_48 = 53, /**< 9.48 dBm. */
    dBm_9_24 = 51, /**< 9.24 dBm. */
    dBm_8_97 = 49, /**< 8.97 dBm. */
    dBm_8_73 = 47, /**< 8.73 dBm. */
    dBm_8_44 = 45, /**< 8.44 dBm. */
    dBm_8_13 = 43, /**< 8.13 dBm. */
    dBm_7_79 = 41, /**< 7.79 dBm. */
    dBm_7_41 = 39, /**< 7.41 dBm. */
    dBm_7_02 = 37, /**< 7.02 dBm. */
    dBm_6_60 = 35, /**< 6.60 dBm. */
    dBm_6_14 = 33, /**< 6.14 dBm. */
    dBm_5_65 = 31, /**< 5.65 dBm. */
    dBm_5_13 = 29, /**< 5.13 dBm. */
    dBm_4_57 = 27, /**< 4.57 dBm. */
    dBm_3_94 = 25, /**< 3.94 dBm. */
    dBm_3_23 = 23  /**< 3.23 dBm. */
};

#endif // ATC_MI_THERMOMETER_ENUMS_H