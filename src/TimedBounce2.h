#pragma once
// Short / medium / long press detection on top of Bounce2::Button.
// Wiring: INPUT_PULLUP, switch to GND (pressed = LOW).

#include <Bounce2.h>

enum class PressType : uint8_t { None, Short, Medium, Long };

class TimedBounce2 : public Bounce2::Button {
public:
  // Timings in ms:
  //   released before shortMax              -> Short
  //   released between mediumMin..mediumMax -> Medium
  //   held for longMin (fires while held)   -> Long
  //   anything else (gaps)                  -> ignored
  void setup(uint8_t pin, uint16_t shortMax, uint16_t mediumMin,
             uint16_t mediumMax, uint16_t longMin, uint16_t debounceMs = 10);

  void setTimings(uint16_t shortMax, uint16_t mediumMin,
                  uint16_t mediumMax, uint16_t longMin);

  // Hides (does not override) Bounce2::Button::update(), which is not virtual.
  // Always call it through a TimedBounce2, not through a Bounce2::Button pointer.
  // Call once per loop(); returns the event of this update or PressType::None.
  PressType update();

  // Time held so far (0 when released); useful for LED feedback
  unsigned long heldMs() const;

private:
  uint16_t _shortMax  = 1000;
  uint16_t _mediumMin = 2000;
  uint16_t _mediumMax = 4000;
  uint16_t _longMin   = 10000;
  bool     _longFired = true;
};
