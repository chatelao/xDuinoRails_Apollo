#ifndef VSD_CONFIG_PARSER_H
#define VSD_CONFIG_PARSER_H

#include "expat.h"
#include <Arduino.h>

struct SoundTrigger {
    int function_number;
    String sound_name;
};

struct SoundDefinition {
    String name;
    String type;
};

class VSDConfigParser {
public:
    VSDConfigParser();
    bool parse(char* xml_data, size_t size);
    const SoundTrigger* get_triggers() const;
    int get_trigger_count() const;
    const char* get_sound_type(const char* name) const;

private:
    static void XMLCALL start_element_handler(void* userData, const XML_Char* name, const XML_Char** atts);
    static void XMLCALL end_element_handler(void* userData, const XML_Char* name);

    enum class ParserState {
        NONE,
        IN_SOUND,
        IN_TRIGGER
    };

    ParserState _state;
    SoundTrigger _triggers[16];
    int _trigger_count;

    SoundDefinition _sounds[16];
    int _sound_count;

    String _current_sound_name;
};

#endif // VSD_CONFIG_PARSER_H
