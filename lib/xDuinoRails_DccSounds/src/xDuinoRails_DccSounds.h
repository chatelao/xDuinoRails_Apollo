#ifndef XDUINORAILS_DCCSOUNDS_H
#define XDUINORAILS_DCCSOUNDS_H

#include <Arduino.h>

class xDuinoRails_DccSounds {
public:
  xDuinoRails_DccSounds();
  void begin();
  void play(int sound_id);
  void stop();
  void set_volume(int volume);
};

#endif
