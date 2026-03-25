#ifndef TRAFFIQO_TRAFFIC_CONFIG_H
#define TRAFFIQO_TRAFFIC_CONFIG_H

#include <array>
#include <cstddef>

namespace traffiqo {

constexpr std::size_t RED_COUNT = 3;
constexpr std::size_t YELLOW_COUNT = 2;
constexpr std::size_t GREEN_COUNT = 4;

struct TrafficConfig {
  std::array<int, RED_COUNT> redPins{2, 3, 4};
  std::array<int, YELLOW_COUNT> yellowPins{6, 7};
  std::array<int, GREEN_COUNT> greenPins{8, 9, 10, 11};
  int pedestrianStopPin = 5;
  int requestButtonPin = 13;

  bool buttonActiveLow = true;

  unsigned long debounceMs = 35;
  unsigned long minGreenMs = 3000;
  unsigned long maxGreenMs = 8000;
  unsigned long requestGraceMs = 1200;
  unsigned long greenFadeStepMs = 180;
  unsigned long yellowLeadMs = 250;
  unsigned long yellowTailMs = 850;
  unsigned long redHoldMs = 5000;
  unsigned long redPrepareLeadMs = 220;
  unsigned long redPrepareTailMs = 420;
};

}  // namespace traffiqo

#endif
