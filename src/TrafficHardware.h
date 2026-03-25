#ifndef TRAFFIQO_TRAFFIC_HARDWARE_H
#define TRAFFIQO_TRAFFIC_HARDWARE_H

#include <cstdint>

namespace traffiqo {

enum class PinMode : std::uint8_t {
  Input = 0,
  Output = 1,
  InputPullup = 2,
};

class IHardware {
 public:
  virtual ~IHardware() = default;

  virtual unsigned long millis() const = 0;
  virtual void pinMode(int pin, PinMode mode) = 0;
  virtual void digitalWrite(int pin, bool high) = 0;
  virtual int digitalRead(int pin) = 0;
  virtual void log(const char* message) = 0;
};

}  // namespace traffiqo

#endif
