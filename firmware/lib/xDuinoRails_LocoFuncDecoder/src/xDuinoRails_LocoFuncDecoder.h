#pragma once

#include <Arduino.h>
#include "LocoFuncDecoderConfig.h"
#include "cv_definitions.h"
#include "CVManager.h"
#include "CVManagerAdapter.h"
#include <xDuinoRails_DccLightsAndFunctions.h>
#include <xDuinoRails_DccSounds.h>
#include "sound/VSDReader.h"
#include "sound/WAVStream.h"
#include "sound/VSDConfigParser.h"
#include "sound/SoftwareMixer.h"
#include <XDuinoRails_MotorControl.h>

#if defined(PROTOCOL_DCC)
#include <NmraDcc.h>
#endif

#if defined(PROTOCOL_MM)
#include <MaerklinMotorola.h>
#endif

class LocoFuncDecoder {
public:
    LocoFuncDecoder();
    ~LocoFuncDecoder();

    /**
     * @brief Initializes the decoder with the given configuration.
     */
    void begin(const LocoFuncDecoderConfig& config);

    /**
     * @brief Main loop update method. Must be called frequently.
     */
    void update();

    /**
     * @brief Accessors for subsystems.
     */
    xDuinoRails::AuxController& getAuxController() { return auxController; }
    CVManager& getCVManager() { return cvManager; }
    XDuinoRails_MotorDriver* getMotorDriver() { return motor; }

#if defined(PROTOCOL_DCC)
    NmraDcc& getDcc() { return dcc; }
#endif
#if defined(PROTOCOL_MM)
    MaerklinMotorola& getMM() { return MM; }
#endif

    // --- Callback Handlers (Called by Global Wrappers) ---
    void handleDccSpeed(uint16_t Addr, uint8_t Speed, bool isForward, uint8_t SpeedSteps);
    void handleDccFunc(uint16_t Addr, uint8_t FuncGrp, uint8_t FuncState);
    void handleCVChange(uint16_t CV, uint8_t Value);

    // Helper for MM
    void handleMMPacket(void* data); // void* to avoid exposing MM types if not included

private:
    LocoFuncDecoderConfig config;

    // Subsystems
    CVManager cvManager;
    CVManagerAdapter cvManagerAdapter;
    xDuinoRails::AuxController auxController;

    // Dynamically allocated to save resources if not enabled
    SoundController* soundController = nullptr;
    VSDReader* vsdReader = nullptr;
    VSDConfigParser* vsdConfigParser = nullptr;
    SoftwareMixer* mixer = nullptr;
    XDuinoRails_MotorDriver* motor = nullptr;

#if defined(PROTOCOL_DCC)
    NmraDcc dcc;
#endif

#if defined(PROTOCOL_MM)
    MaerklinMotorola MM;
#endif

    void processFunctionGroup(int start_fn, int count, uint8_t state_mask);
};

// Global instance pointer for callbacks
extern LocoFuncDecoder* globalDecoderInstance;
