#include "VSDConfigParser.h"

VSDConfigParser::VSDConfigParser() : _state(ParserState::NONE), _trigger_count(0), _sound_count(0) {
}

bool VSDConfigParser::parse(char* xml_data, size_t size) {
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, start_element_handler, end_element_handler);

    if (XML_Parse(parser, xml_data, size, 1) == XML_STATUS_ERROR) {
        return false;
    }

    XML_ParserFree(parser);
    return true;
}

const SoundTrigger* VSDConfigParser::get_triggers() const {
    return _triggers;
}

int VSDConfigParser::get_trigger_count() const {
    return _trigger_count;
}

const char* VSDConfigParser::get_sound_type(const char* name) const {
    for (int i = 0; i < _sound_count; ++i) {
        if (_sounds[i].name.equals(name)) {
            return _sounds[i].type.c_str();
        }
    }
    return nullptr;
}

void XMLCALL VSDConfigParser::start_element_handler(void* userData, const XML_Char* name, const XML_Char** atts) {
    VSDConfigParser* self = (VSDConfigParser*)userData;

    if (strcmp(name, "sound") == 0) {
        self->_state = ParserState::IN_SOUND;
        String sound_name = "";
        String sound_type = "ONE_SHOT"; // Default type

        for (int i = 0; atts[i]; i += 2) {
            if (strcmp(atts[i], "name") == 0) {
                sound_name = atts[i + 1];
            } else if (strcmp(atts[i], "type") == 0) {
                sound_type = atts[i + 1];
            }
        }

        self->_current_sound_name = sound_name;

        // Store definition if we have space
        if (self->_sound_count < 16 && sound_name.length() > 0) {
            self->_sounds[self->_sound_count].name = sound_name;
            self->_sounds[self->_sound_count].type = sound_type;
            self->_sound_count++;
        }

    } else if (strcmp(name, "trigger") == 0 && self->_state == ParserState::IN_SOUND) {
        self->_state = ParserState::IN_TRIGGER;
        int function_number = -1;
        for (int i = 0; atts[i]; i += 2) {
            if (strcmp(atts[i], "function") == 0) {
                function_number = atoi(atts[i + 1]);
                break;
            }
        }
        if (function_number != -1 && self->_trigger_count < 16) {
            self->_triggers[self->_trigger_count].function_number = function_number;
            self->_triggers[self->_trigger_count].sound_name = self->_current_sound_name;
            self->_trigger_count++;
        }
    }
}

void XMLCALL VSDConfigParser::end_element_handler(void* userData, const XML_Char* name) {
    VSDConfigParser* self = (VSDConfigParser*)userData;

    if (strcmp(name, "sound") == 0) {
        self->_state = ParserState::NONE;
    } else if (strcmp(name, "trigger") == 0) {
        self->_state = ParserState::IN_SOUND;
    }
}
