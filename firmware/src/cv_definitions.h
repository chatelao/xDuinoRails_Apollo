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
#define CV_MOTOR_CONFIGURATION 50
#define CV_PID_KP 51
#define CV_PID_KI 52
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
#define DECODER_DEFAULT_MAXIMUM_SPEED 255   // Full speed
#define DECODER_DEFAULT_EXT_ADDRESS_MSB 195 // High byte for address 1000
#define DECODER_DEFAULT_EXT_ADDRESS_LSB 232 // Low byte for address 1000
#define DECODER_DEFAULT_MANUFACTURER_ID 165 // NMRA ID for DIY/Home-built decoders
#define DECODER_DEFAULT_CV29_CONFIG 6       // Enable 28/128 speed steps and analog mode
#define DECODER_DEFAULT_VERSION_ID 1        // Firmware version 1
#define DECODER_DEFAULT_MOTOR_CONFIGURATION 1 // Enable BEMF by default
#define DECODER_DEFAULT_PID_KP 10           // Kp = 0.10
#define DECODER_DEFAULT_PID_KI 10           // Ki = 0.10

// RCN-225 Default Function Mappings (CVs 33-46)
#define DECODER_DEFAULT_F0_FWD_MAPPING 1   // Map F0 Fwd to Output 1
#define DECODER_DEFAULT_F0_REV_MAPPING 2   // Map F0 Rev to Output 2
#define DECODER_DEFAULT_F1_MAPPING 4     // Map F1 to Output 3
#define DECODER_DEFAULT_F2_MAPPING 8     // Map F2 to Output 4
#define DECODER_DEFAULT_F3_MAPPING 16    // Map F3 to Output 5
#define DECODER_DEFAULT_F4_MAPPING 32    // Map F4 to Output 6
#define DECODER_DEFAULT_F5_MAPPING 64    // Map F5 to Output 7
#define DECODER_DEFAULT_F6_MAPPING 128   // Map F6 to Output 8
#define DECODER_DEFAULT_FUNCTION_MAPPING_METHOD 1 // Use RCN-225 standard mapping by default

// --- Internal CV Base Addresses for RCN-227 Indexed Blocks ---
// These are not real CVs but are used internally to store the paged data.
#define RCN227_PF_BLOCK_CV_BASE 513
#define RCN227_PO_V1_BLOCK_CV_BASE 769
#define RCN227_PO_V2_BLOCK_CV_BASE 1025
#define RCN227_PO_V3_BLOCK_CV_BASE 1281


#endif // CV_DEFINITIONS_H
