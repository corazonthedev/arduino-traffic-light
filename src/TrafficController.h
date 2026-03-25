#ifndef TRAFFIQO_TRAFFIC_CONTROLLER_H
#define TRAFFIQO_TRAFFIC_CONTROLLER_H

#include "TrafficConfig.h"
#include "TrafficHardware.h"

namespace traffiqo {

enum class Stage {
  Green,
  GreenFade,
  Yellow,
  Red,
  RedPrepare,
};

class TrafficController {
 public:
  TrafficController(IHardware& hardware, const TrafficConfig& config = TrafficConfig());

  void begin();
  void update();

  Stage stage() const;
  bool pedestrianRequestPending() const;
  unsigned long stageElapsedMs() const;

 private:
  void configurePins();
  void sampleButton(unsigned long now);
  void updateStage(unsigned long now);
  void transitionTo(Stage nextStage, unsigned long now);
  void applyOutputs();
  void setPedestrianRequest(unsigned long now);
  bool isButtonPressed(int value) const;

  IHardware& hardware_;
  TrafficConfig config_;

  Stage stage_ = Stage::Green;
  unsigned long stageStartedAtMs_ = 0;
  unsigned long stageDeadlineMs_ = 0;

  bool pedestrianRequestPending_ = false;
  bool rawButtonPressed_ = false;
  bool debouncedButtonPressed_ = false;
  unsigned long lastButtonChangeAtMs_ = 0;

  std::size_t greenFadeStep_ = 0;
  bool yellowSecondLampOn_ = false;
};

}  // namespace traffiqo

#endif
