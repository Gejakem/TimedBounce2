#include <TimedBounce2.h>

// Button indices, so loop() can use names instead of numbers
enum ButtonId : uint8_t { BTN_START = 0, BTN_MODE = 1, BTN_RESET = 2, BUTTON_COUNT };

TimedBounce2 buttons[BUTTON_COUNT];

const char* typeName(PressType t) {
  switch (t) {
    case PressType::Short:  return "short";
    case PressType::Medium: return "medium";
    case PressType::Long:   return "long";
    default:                return "none";
  }
}

void setup() {
  Serial.begin(115200);
  //                    pin shortMax mediumMin mediumMax longMin
  buttons[BTN_START].init(2, 1000,    2000,     4000,    10000);
  buttons[BTN_MODE ].init(3,  800,    1500,     3500,     6000);
  buttons[BTN_RESET].init(4, 1000,    2500,     4000,    10000);
}

void loop() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    const PressType ev = buttons[i].update();
    if (ev == PressType::None) continue;

    Serial.print("Button ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(typeName(ev));

    switch (i) {
      case BTN_START:
        if (ev == PressType::Short) { /* Start */ }
        break;
      case BTN_MODE:
        if (ev == PressType::Medium) { /* Next mode */ }
        break;
      case BTN_RESET:
        if (ev == PressType::Long) { /* Factory reset */ }
        break;
    }
  }

  // Other non-blocking work goes here
}
