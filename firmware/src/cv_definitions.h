#ifndef CV_DEFINITIONS_H
#define CV_DEFINITIONS_H

/**
 * @file cv_definitions.h
 * @brief Centralized definitions for all NMRA and project-specific CVs.
 * @details This file standardizes the CV numbers and default values used throughout the firmware,
 * aligning them with the RCN-225 standard where applicable.
 */

// From NmraDcc.h Library - Standard NMRA CVs
#define CV_MULTIFUNCTION_PRIMARY_ADDRESS 1
#define CV_START_VOLTAGE 2
#define CV_ACCELERATION_RATE 3
#define CV_DECELERATION_RATE 4
#define CV_MAXIMUM_SPEED 5
#define CV_MID_SPEED 6 // Vmid in NMRA S-9.2.2
#define CV_DECODER_VERSION_ID 7
#define CV_MANUFACTURER_ID 8
#define CV_PULSE_WIDTH_PERIOD 9
#define CV_EMF_FEEDBACK_CUTOUT 10
#define CV_PACKET_TIME_OUT 11
#define CV_POWER_SOURCE_CONVERSION 12
#define CV_ALT_MODE_FUNC_STATUS_F1_F8 13
#define CV_ALT_MODE_FUNC_STATUS_FL_F9_F12 14
#define CV_DECODER_LOCK_1 15
#define CV_DECODER_LOCK_2 16
#define CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB 17
#define CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB 18
#define CV_CONSIST_ADDRESS 19
#define CV_CONSIST_ADDRESS_LONG 20
#define CV_CONSIST_FUNC_GROUP1_ENABLE 21
#define CV_CONSIST_FUNC_GROUP2_ENABLE 22
#define CV_ACCEL_ADJUSTMENT 23
#define CV_DECEL_ADJUSTMENT 24
#define CV_SPEED_TABLE_MID_POINT 25
#define CV_AUTOMATIC_STOPPING 27
#define CV_RAILCOM_CONFIG 28
#define CV_DECODER_CONFIGURATION 29
#define CV_ERROR_INFORMATION 30
#define CV_INDEXED_CV_HIGH_BYTE 31
#define CV_INDEXED_CV_LOW_BYTE 32
#define CV_OUTPUT_LOCATION_CONFIG_START 33 // CVs 33-46
#define CV_OUTPUT_LOCATION_CONFIG_END 46
#define CV_FORWARD_TRIM 66
#define CV_SPEED_TABLE_START 67 // CVs 67-94
#define CV_SPEED_TABLE_END 94
#define CV_REVERSE_TRIM 95
#define CV_FUNCTION_MAPPING_METHOD 96
#define CV_USER_ID_1 105
#define CV_USER_ID_2 106


// CV 29 Configuration Bits (from NmraDcc.h)
#define CV29_DIRECTION_BIT 0b00000001        // Bit 0: Locomotive Direction
#define CV29_FL_LOCATION_BIT 0b00000010       // Bit 1: F0 Light Location
#define CV29_POWER_SOURCE_BIT 0b00000100      // Bit 2: Power Source Conversion (Analog Mode)
#define CV29_RAILCOM_ENABLE_BIT 0b00001000   // Bit 3: RailCom Enable
#define CV29_SPEED_TABLE_ENABLE_BIT 0b00010000 // Bit 4: Speed Table Enable
#define CV29_EXT_ADDRESSING_BIT 0b00100000  // Bit 5: Extended Addressing Enable
// Bit 6 is reserved
#define CV29_ACCESSORY_DECODER_BIT 0b10000000 // Bit 7: Accessory Decoder Mode


// Default Values
#define DECODER_DEFAULT_PRIMARY_ADDRESS 3
#define DECODER_DEFAULT_START_VOLTAGE 80    // Project specific, adjust as needed
#define DECODER_DEFAULT_ACCELERATION_RATE 50  // Project specific, adjust as needed
#define DECODER_DEFAULT_DECELERATION_RATE 100 // Project specific, adjust as needed
#define DECODER_DEFAULT_MANUFACTURER_ID 165 // NMRA ID for DIY/Home-built decoders
#define DECODER_DEFAULT_CV29_CONFIG 6       // Enable 28/128 speed steps and analog mode
#define DECODER_DEFAULT_VERSION_ID 1        // Firmware version 1

#endif // CV_DEFINITIONS_H
