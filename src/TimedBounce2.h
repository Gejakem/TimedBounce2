#pragma once
/**
 * @file TimedBounce2.h
 * @brief Short, medium and long press detection on top of Bounce2::Button.
 *
 * Wiring: INPUT_PULLUP, switch to GND (pressed = LOW).
 */

#include <Bounce2.h>

/**
 * @brief Event returned by TimedBounce2::update().
 * @since 1.0.0
 */
enum class PressType : uint8_t {
  None,    ///< No event in this update
  Short,   ///< Released before shortMax
  Medium,  ///< Released between mediumMin and mediumMax
  Long     ///< Held for longMin (fires while still held)
};

/**
 * @brief What releasing the button right now would give, see TimedBounce2::holdZone().
 * @since 1.1.0
 */
enum class HoldZone : uint8_t {
  Idle,    ///< Not pressed
  Short,   ///< Held less than shortMax
  Gap,     ///< Between shortMax and mediumMin, or between mediumMax and longMin
  Medium,  ///< Held between mediumMin and mediumMax
  Long     ///< Held longMin or longer
};

/**
 * @brief Bounce2::Button with short, medium and long press events.
 *
 * Presses that fall in a gap between the ranges are ignored.
 * Declare objects globally: Bounce2 does not initialise its duration fields.
 * @since 1.0.0
 */
class TimedBounce2 : public Bounce2::Button {
public:
  /**
   * @brief Attach the button and set debounce interval and timings.
   *
   * Sets the pin to INPUT_PULLUP and the pressed state to LOW.
   * Call it only once per button, before the first update(); normally in setup().
   * Calling it again resets the button. Use setTimings() to change timings later.
   *
   * @param pin        Pin the button is connected to
   * @param shortMax   Released before this time (ms) gives PressType::Short
   * @param mediumMin  Start of the medium range (ms)
   * @param mediumMax  End of the medium range (ms, inclusive)
   * @param longMin    Held this long (ms) gives PressType::Long
   * @param debounceMs Debounce interval (ms), default 10
   * @since 1.0.0
   */
  void init(uint8_t pin, uint16_t shortMax, uint16_t mediumMin,
            uint16_t mediumMax, uint16_t longMin, uint16_t debounceMs = 10);

  /**
   * @brief Change the timings at runtime.
   *
   * Does not touch the pin or the debouncer, so it can be called at any time.
   *
   * @param shortMax  Released before this time (ms) gives PressType::Short
   * @param mediumMin Start of the medium range (ms)
   * @param mediumMax End of the medium range (ms, inclusive)
   * @param longMin   Held this long (ms) gives PressType::Long
   * @since 1.0.0
   */
  void setTimings(uint16_t shortMax, uint16_t mediumMin,
                  uint16_t mediumMax, uint16_t longMin);

  /**
   * @brief Update the button and return the event of this update.
   *
   * Call it once per loop(). Hides (does not override) Bounce2::Button::update(),
   * which is not virtual: always call it through a TimedBounce2, not through a
   * Bounce2::Button pointer or reference.
   *
   * @return The event of this update, or PressType::None
   * @since 1.0.0
   */
  PressType update();

  /**
   * @brief How long the button has been held so far.
   *
   * Useful for feedback, such as blinking an LED when a long press is near.
   *
   * @return Time held in ms, or 0 when released
   * @since 1.0.0
   */
  unsigned long heldMs() const;

  /**
   * @brief Zone the current press is in, using the same timings as update().
   *
   * Watch it for changes in loop() to give feedback while the button is held,
   * for example flashing a LED when releasing would give a medium press.
   *
   * @return HoldZone::Idle, Short, Gap, Medium or Long
   * @since 1.1.0
   */
  HoldZone holdZone() const;

private:
  uint16_t _shortMax  = 1000;
  uint16_t _mediumMin = 2000;
  uint16_t _mediumMax = 4000;
  uint16_t _longMin   = 10000;
  bool     _longFired = true;
};
