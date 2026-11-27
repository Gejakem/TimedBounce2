#include "TimedBounce2.h"

void TimedBounce2::setup(uint8_t pin, uint16_t shortMax, uint16_t mediumMin,
                        uint16_t mediumMax, uint16_t longMin, uint16_t debounceMs) {
  setPressedState(LOW);
  interval(debounceMs);
  attach(pin, INPUT_PULLUP);  // Calls Debouncer::begin()
  setTimings(shortMax, mediumMin, mediumMax, longMin);
  _longFired = true;          // A button held during boot gives no event
}

void TimedBounce2::setTimings(uint16_t shortMax, uint16_t mediumMin,
                             uint16_t mediumMax, uint16_t longMin) {
  _shortMax  = shortMax;
  _mediumMin = mediumMin;
  _mediumMax = mediumMax;
  _longMin   = longMin;
}

PressType TimedBounce2::update() {
  Bounce2::Button::update();

  if (pressed()) {
    _longFired = false;
  }

  // Long fires while the button is still held
  if (isPressed() && !_longFired && currentDuration() >= _longMin) {
    _longFired = true;
    return PressType::Long;
  }

  if (released() && !_longFired) {
    const unsigned long d = previousDuration();
    if (d < _shortMax) return PressType::Short;
    if (d >= _mediumMin && d <= _mediumMax) return PressType::Medium;
  }

  return PressType::None;
}

unsigned long TimedBounce2::heldMs() const {
  return isPressed() ? currentDuration() : 0;
}
