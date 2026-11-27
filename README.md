# TimedBounce2

Short, medium and long press detection for Arduino, built on top of [Bounce2](https://github.com/thomasfredericks/Bounce2).

`TimedBounce2` extends `Bounce2::Button` and turns button presses into simple events:

* **Short**: released before `shortMax`
* **Medium**: released between `mediumMin` and `mediumMax`
* **Long**: held for `longMin`. This fires while the button is still held, so the user doesn't have to release it.
* Presses that fall in a gap between these ranges are ignored.

Features:

* Non-blocking: no `delay()` and no interrupts
* Timings can be set per button, and changed at runtime
* Multiple buttons: use a plain array, and the array index identifies the button
* A button held during boot does not generate an event
* All `Bounce2::Button` methods stay available (`isPressed()`, `currentDuration()`, `interval()`, ...)

# Installation

TimedBounce2 needs the **Bounce2** library. Install Bounce2 first through the Library Manager (Tools → Manage Libraries, search for "Bounce2").

## From ZIP

1. On the [GitHub page](https://github.com/Gejakem/TimedBounce2), click **Code → Download ZIP**.
2. In the Arduino IDE, choose **Sketch → Include Library → Add .ZIP Library...** and select the downloaded ZIP.
3. The example is then available under **File → Examples → TimedBounce2**.

## Library structure

```
TimedBounce2/
├── library.properties
├── README.md
├── LICENSE
├── src/
│   ├── TimedBounce2.h
│   └── TimedBounce2.cpp
└── examples/
    └── TimedBounce2Demo/
        └── TimedBounce2Demo.ino
```

# Wiring

Connect each button between a digital pin and GND. The internal pull-up is enabled by `init()`, so the pin reads `LOW` when the button is pressed. No external resistor is needed.

# Basic use

```cpp
#include <TimedBounce2.h>

TimedBounce2 button;

void setup() {
  Serial.begin(115200);
  //          pin shortMax mediumMin mediumMax longMin  (ms)
  button.init(2,  1000,    2000,     4000,    10000);
}

void loop() {
  switch (button.update()) {
    case PressType::Short:  Serial.println("short");  break;
    case PressType::Medium: Serial.println("medium"); break;
    case PressType::Long:   Serial.println("long");   break;
    default: break;
  }
}
```

# Multiple buttons

```cpp
#include <TimedBounce2.h>

enum ButtonId : uint8_t { BTN_START = 0, BTN_MODE = 1, BTN_RESET = 2, BUTTON_COUNT };

TimedBounce2 buttons[BUTTON_COUNT];

void setup() {
  buttons[BTN_START].init(2, 1000, 2000, 4000, 10000);
  buttons[BTN_MODE ].init(3,  800, 1500, 3500,  6000);
  buttons[BTN_RESET].init(4, 1000, 2500, 4000, 10000);
}

void loop() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    const PressType ev = buttons[i].update();
    if (ev == PressType::None) continue;

    // i tells which button, ev tells which press type
  }
}
```

See `examples/TimedBounce2Demo` for a complete sketch.

# Documentation

## PressType

```cpp
enum class PressType : uint8_t { None, Short, Medium, Long };
```

## TimedBounce2

`TimedBounce2` adds the following methods to the ones it inherits from `Bounce2::Button`:

|Method|Description|
|-|-|
|`void init(uint8_t pin, uint16_t shortMax, uint16_t mediumMin, uint16_t mediumMax, uint16_t longMin, uint16_t debounceMs = 10)`|Attaches the pin with `INPUT_PULLUP`, sets pressed state to `LOW`, and sets the debounce interval and timings. Call it only once per button, before the first `update()`; normally in `setup()`. Calling it again resets the button. Use `setTimings()` to change timings later.|
|`void setTimings(uint16_t shortMax, uint16_t mediumMin, uint16_t mediumMax, uint16_t longMin)`|Changes the timings at runtime.|
|`PressType update()`|Updates the button and returns the event of this update, or `PressType::None`. Call it once per `loop()`.|
|`unsigned long heldMs() const`|How long the button has been held so far in ms, or 0 when released. Useful for feedback, such as blinking an LED when a long press is near.|

All timings are in milliseconds. If you don't want any gaps between the ranges, set `mediumMin` equal to `shortMax`, and `mediumMax` just below `longMin`.

# Notes

* `update()` hides `Bounce2::Button::update()`, it does not override it, because that method is not virtual in Bounce2. Always call it on a `TimedBounce2` object, not through a `Bounce2::Button` pointer or reference.
* Declare `TimedBounce2` objects globally. Bounce2 does not initialise its internal duration fields, and only global (static) objects get them zero-initialised automatically.
* In Bounce2's default debounce mode, events arrive one debounce interval (default 10 ms) after the physical change. The measured durations are still accurate. If you define `BOUNCE_WITH_PROMPT_DETECTION` in Bounce2, events arrive immediately.

# Supported hardware

TimedBounce2 only uses standard Arduino functions and C++11. All pin access goes through Bounce2 (`pinMode()`, `digitalRead()`, `millis()`), so it should work wherever Bounce2 works. It has not been tested on every board.

|Family|Examples|Notes|
|-|-|-|
|AVR|Uno, Nano, Mega, Leonardo, Pro Mini|Works.|
|ATtiny|ATTinyCore, megaTinyCore|Works. Mind the small RAM. The demo uses `Serial`.|
|megaAVR, SAMD, Renesas, mbed|Nano Every, Zero, MKR, Nano 33 IoT/BLE, Uno R4, Portenta, Giga|Should work. These cores use ArduinoCore-API (`PinMode` enum).|
|ESP8266|NodeMCU, Wemos D1|Works. GPIO16 has no internal pull-up. GPIO0/2/15 affect how the board boots.|
|ESP32|ESP32, S2, S3, C3, C6|Works. GPIO34–39 on the classic ESP32 have no internal pull-up. Avoid pins that affect how the board boots (such as GPIO0 and GPIO12).|
|STM32|STM32duino, Roger Clark STM32F1 core|Works. Bounce2 handles the STM32F1 core.|
|RP2040 / RP2350|Raspberry Pi Pico (Philhower or mbed core)|Works.|
|Others|Teensy, Adafruit nRF52, Due|Works.|

Notes:

* 3.3 V boards are fine, because the button switches to GND.
* It does not work without the Arduino API, for example plain ESP-IDF, STM32Cube/HAL or Linux on a Raspberry Pi.
* `millis()` overflow (after about 49 days) is handled correctly.

# License

MIT. See [LICENSE](LICENSE).

Bounce2 is a separate library, also licensed under MIT, by Thomas O Fredericks and contributors.
