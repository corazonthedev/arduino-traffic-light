#ifndef TRAFFIQO_ARDUINO_HARDWARE_H
#define TRAFFIQO_ARDUINO_HARDWARE_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "TrafficHardware.h"

namespace traffiqo {

class ArduinoHardware : public IHardware {
 public:
  unsigned long millis() const override {
#ifdef ARDUINO
    return ::millis();
#else
    return 0;
#endif
  }

  void pinMode(int pin, PinMode mode) override {
#ifdef ARDUINO
    switch (mode) {
      case PinMode::Output:
        ::pinMode(pin, OUTPUT);
        break;
      case PinMode::InputPullup:
        ::pinMode(pin, INPUT_PULLUP);
        break;
      case PinMode::Input:
      default:
        ::pinMode(pin, INPUT);
        break;
    }
#else
    (void)pin;
    (void)mode;
#endif
  }

  void digitalWrite(int pin, bool high) override {
#ifdef ARDUINO
    ::digitalWrite(pin, high ? HIGH : LOW);
#else
    (void)pin;
    (void)high;
#endif
  }

  int digitalRead(int pin) override {
#ifdef ARDUINO
    return ::digitalRead(pin);
#else
    (void)pin;
    return 1;
#endif
  }

  void log(const char* message) override {
#ifdef ARDUINO
    Serial.println(message);
#else
    (void)message;
#endif
  }
};

}  // namespace traffiqo

#endif
