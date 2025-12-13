#include "xDuinoRails_LocoFuncDecoder.h"

LocoFuncDecoder* globalDecoderInstance = nullptr;

LocoFuncDecoder::LocoFuncDecoder() : cvManagerAdapter(cvManager)
#if defined(PROTOCOL_MM)
, MM(7) // Default MM pin, overridden in begin if needed
#endif
{
    globalDecoderInstance = this;
}

LocoFuncDecoder::~LocoFuncDecoder() {
    if (motor) delete motor;
    if (mixer) delete mixer;
    if (soundController) delete soundController;
    if (vsdReader) delete vsdReader;
    if (vsdConfigParser) delete vsdConfigParser;
}

void LocoFuncDecoder::begin(const LocoFuncDecoderConfig& conf) {
    this->config = conf;

    // --- CV Manager ---
    cvManager.begin();

    // --- Motor Control ---
    if (config.enableMotor) {
        motor = new XDuinoRails_MotorDriver(config.motorPinA, config.motorPinB, config.bemfPinA, config.bemfPinB);
        motor->begin();

        // Load Motor CVs
        motor->setStartupKick(cvManager.readCV(CV_START_VOLTAGE), config.startupKickDuration);
        motor->setAcceleration(cvManager.readCV(CV_ACCELERATION_RATE) * 2.5);
        motor->setDeceleration(cvManager.readCV(CV_DECELERATION_RATE) * 2.5);
        motor->enablePIController(cvManager.readCV(CV_MOTOR_CONFIGURATION) & 0x01);
        float kp = (float)cvManager.readCV(CV_PID_KP) / 100.0f;
        float ki = (float)cvManager.readCV(CV_PID_KI) / 100.0f;
        motor->setPIgains(kp, ki);
    }

    // --- Sound System ---
    if (config.enableSound) {
        soundController = new SoundController();
        mixer = new SoftwareMixer(*soundController);
        vsdReader = new VSDReader();
        vsdConfigParser = new VSDConfigParser();

        soundController->begin(); // Assumes I2S pins are configured via build flags or default?
                                  // Wait, SoundController in xDuinoRails_DccSounds usually hardcodes pins or uses build flags.
                                  // If the library supports setting pins at runtime, we should do it.
                                  // For now, we assume it picks up the board defaults or build flags.

        mixer->begin();
        soundController->setVolume(25);

        // VSD Loading
        if (LittleFS.begin()) {
             if (vsdReader->begin("/test.vsd")) {
                uint8_t* xml_data = nullptr;
                size_t xml_size = 0;
                if (vsdReader->get_file_data("config.xml", &xml_data, &xml_size)) {
                    vsdConfigParser->parse((char*)xml_data, xml_size);
                    free(xml_data);
                }
            }
        }
    }

    // --- Lights & Aux ---
    if (config.enableLights && config.useDefaultPinout) {
        auxController.addPhysicalOutput(config.pinHeadlightFwd, xDuinoRails::OutputType::LIGHT_SOURCE);
        auxController.addPhysicalOutput(config.pinHeadlightRev, xDuinoRails::OutputType::LIGHT_SOURCE);
        auxController.addPhysicalOutput(config.pinCabinLight, xDuinoRails::OutputType::LIGHT_SOURCE);
        auxController.addPhysicalOutput(config.pinServo1, xDuinoRails::OutputType::SERVO);
    }

    auxController.loadFromCVs(cvManagerAdapter);

    // --- Protocol Setup ---
#if defined(PROTOCOL_DCC)
    // DCC Init is typically done in the sketch to define the pin, but we can wrap it if we passed the pin.
    // The sketch usually calls dcc.pin(...) then dcc.init(...).
    // Here we can initialize our CV syncing.
    dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, cvManager.readCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS));
    dcc.setCV(CV_MANUFACTURER_ID, cvManager.readCV(CV_MANUFACTURER_ID));
    dcc.setCV(CV_DECODER_VERSION_ID, cvManager.readCV(CV_DECODER_VERSION_ID));
    dcc.setCV(CV_DECODER_CONFIGURATION, cvManager.readCV(CV_DECODER_CONFIGURATION));
#endif

#if defined(PROTOCOL_MM)
    // MM Init is interrupt based, usually handled in sketch setup.
    // We apply motor settings here (already done above).
#endif
}

void LocoFuncDecoder::update() {
    static uint32_t last_millis = 0;
    uint32_t current_millis = millis();
    uint32_t delta_ms = current_millis - last_millis;
    last_millis = current_millis;

#if defined(PROTOCOL_DCC)
    dcc.process();
#elif defined(PROTOCOL_MM)
    MM.Parse();
    MaerklinMotorolaData* data = MM.GetData();
    if (data && !data->IsMagnet && data->Address == 5) { // Hardcoded address 5 for now, should be from config/CV
         handleMMPacket(data);
    }
#endif

    if (motor) motor->update();
    auxController.update(delta_ms);

    if (soundController) {
        soundController->loop();
        if (mixer) mixer->update();
    }
}

void LocoFuncDecoder::handleDccSpeed(uint16_t Addr, uint8_t Speed, bool isForward, uint8_t SpeedSteps) {
#if defined(PROTOCOL_DCC)
    if (Addr != dcc.getAddr()) return;
#endif

    if (motor) {
        motor->setDirection(isForward);
        uint8_t max_speed = cvManager.readCV(CV_MAXIMUM_SPEED);
        if (max_speed == 0) max_speed = 255;
        // Basic mapping for 128 steps (Speed is 0-255 already from NmraDcc usually?)
        // NmraDcc notifies raw speed steps? No, it's usually normalized?
        // Wait, main.cpp used: map(Speed, 0, 255, 0, max_speed);
        int pps = map(Speed, 0, 255, 0, max_speed);
        motor->setTargetSpeed(pps);
    }

    auxController.setDirection(isForward ? xDuinoRails::DECODER_DIRECTION_FORWARD : xDuinoRails::DECODER_DIRECTION_REVERSE);
    auxController.setSpeed(Speed); // Aux controller expects 0-255
}

void LocoFuncDecoder::handleDccFunc(uint16_t Addr, uint8_t FuncGrp, uint8_t FuncState) {
#if defined(PROTOCOL_DCC)
    if (Addr != dcc.getAddr()) return;
#endif
    // Logic extracted from main.cpp switch
    // Note: FuncGrp enum is from NmraDcc.h

    // We need to know which functions are in the group.
    // This is NMRA specific.
    // Since we are inside the library, and we might not have NmraDcc.h included if not PROTOCOL_DCC,
    // we should be careful. But this method is likely only called if DCC is active.

#if defined(PROTOCOL_DCC)
    switch (FuncGrp) {
        case FN_0_4:
            auxController.setFunctionState(0, (FuncState & FN_BIT_00) != 0);
            processFunctionGroup(1, 4, FuncState);
            break;
        case FN_5_8:   processFunctionGroup(5, 4, FuncState); break;
        case FN_9_12:  processFunctionGroup(9, 4, FuncState); break;
        case FN_13_20: processFunctionGroup(13, 8, FuncState); break;
        case FN_21_28: processFunctionGroup(21, 8, FuncState); break;
        default: break;
    }
#endif
}

void LocoFuncDecoder::processFunctionGroup(int start_fn, int count, uint8_t state_mask) {
    for (int i = 0; i < count; i++) {
        bool state = (state_mask >> i) & 0x01;
        int current_fn = start_fn + i;
        auxController.setFunctionState(current_fn, state);

        if (config.enableSound && soundController && mixer && vsdConfigParser && vsdReader) {
            // Hardcoded beep logic from main.cpp
             if (current_fn == 1 && state) {
                soundController->play(1);
            }

            if (state) {
                // Trigger sound
                 for (int j = 0; j < vsdConfigParser->get_trigger_count(); j++) {
                    const SoundTrigger* trigger = &vsdConfigParser->get_triggers()[j];
                    if (trigger->function_number == current_fn) {
                        uint8_t* wav_data = nullptr;
                        size_t wav_size = 0;
                        if (vsdReader->get_file_data(trigger->sound_name.c_str(), &wav_data, &wav_size)) {
                            WAVStream* stream = new WAVStream();
                            if (stream->begin(wav_data, wav_size)) {
                                const char* sound_type = vsdConfigParser->get_sound_type(trigger->sound_name.c_str());
                                if (sound_type && strcmp(sound_type, "CONTINUOUS_LOOP") == 0) {
                                    stream->setLooping(true);
                                }
                                mixer->play(stream);
                            } else {
                                delete stream;
                                free(wav_data);
                            }
                        }
                    }
                }
            }
        }
    }
}

void LocoFuncDecoder::handleCVChange(uint16_t CV, uint8_t Value) {
    cvManager.writeCV(CV, Value);

    if (motor) {
         switch (CV) {
            case CV_START_VOLTAGE:
                motor->setStartupKick(Value, config.startupKickDuration);
                break;
            case CV_ACCELERATION_RATE:
                motor->setAcceleration(Value * 2.5);
                break;
            case CV_DECELERATION_RATE:
                motor->setDeceleration(Value * 2.5);
                break;
            case CV_MOTOR_CONFIGURATION:
                motor->enablePIController(Value & 0x01);
                break;
            case CV_PID_KP:
            case CV_PID_KI:
                {
                    float kp = (float)cvManager.readCV(CV_PID_KP) / 100.0f;
                    float ki = (float)cvManager.readCV(CV_PID_KI) / 100.0f;
                    motor->setPIgains(kp, ki);
                }
                break;
        }
    }
}

void LocoFuncDecoder::handleMMPacket(void* voidData) {
#if defined(PROTOCOL_MM)
    MaerklinMotorolaData* data = (MaerklinMotorolaData*)voidData;
    auxController.setFunctionState(0, data->Function);

    if (motor) {
        if (data->ChangeDir) {
            motor->setDirection(!motor->getDirection());
        } else if (data->Stop) {
            motor->setTargetSpeed(0);
        } else {
            uint8_t max_speed = cvManager.readCV(CV_MAXIMUM_SPEED);
            if (max_speed == 0) max_speed = 255;
            uint8_t pps = map(data->Speed, 0, 14, 0, max_speed);
            motor->setTargetSpeed(pps);
        }
        auxController.setDirection(motor->getDirection() ? xDuinoRails::DECODER_DIRECTION_FORWARD : xDuinoRails::DECODER_DIRECTION_REVERSE);
        auxController.setSpeed(motor->getTargetSpeed());
    }
#endif
}
