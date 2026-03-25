#ifndef TRAFFIQO_TESTS_MOCK_HARDWARE_H
#define TRAFFIQO_TESTS_MOCK_HARDWARE_H

#include <map>
#include <string>
#include <vector>

#include "../src/TrafficHardware.h"

namespace traffiqo {

class MockHardware : public IHardware {
 public:
  unsigned long nowMs = 0;
  std::map<int, bool> digitalOutputs;
  std::map<int, int> digitalInputs;
  std::map<int, PinMode> pinModes;
  std::vector<std::string> logs;

  unsigned long millis() const override { return nowMs; }

  void pinMode(int pin, PinMode mode) override { pinModes[pin] = mode; }

  void digitalWrite(int pin, bool high) override { digitalOutputs[pin] = high; }

  int digitalRead(int pin) override {
    const auto it = digitalInputs.find(pin);
    return it == digitalInputs.end() ? 1 : it->second;
  }

  void log(const char* message) override { logs.emplace_back(message); }

  void setButtonLow(int pin) { digitalInputs[pin] = 0; }
  void setButtonHigh(int pin) { digitalInputs[pin] = 1; }
};

}  // namespace traffiqo

#endif
