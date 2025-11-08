#ifndef CV_DEFINITIONS_H
#define CV_DEFINITIONS_H

// --- Standard NMRA CV Definitions ---

// These CVs are defined by the NMRA DCC standard. By centralizing them here,
// we can use them in both DCC and Märklin-Motorola protocol modes without
// creating a dependency on the NmraDcc library.

#define CV_START_VOLTAGE 2
#define CV_ACCELERATION_RATE 3
#define CV_DECELERATION_RATE 4
#define CV_MANUFACTURER_ID 8
#define CV_MULTIFUNCTION_PRIMARY_ADDRESS 1

#endif // CV_DEFINITIONS_H
