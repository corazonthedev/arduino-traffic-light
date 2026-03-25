#include "TrafficController.h"

#include <algorithm>
#include <cstddef>

namespace traffiqo {
namespace {

constexpr char kStageGreen[] = "[traffiqo] Stage -> GREEN";
constexpr char kStageGreenFade[] = "[traffiqo] Stage -> GREEN_FADE";
constexpr char kStageYellow[] = "[traffiqo] Stage -> YELLOW";
constexpr char kStageRed[] = "[traffiqo] Stage -> RED / PEDESTRIAN CROSS";
constexpr char kStageRedPrepare[] = "[traffiqo] Stage -> RED_PREPARE";
constexpr char kPedestrianRequest[] = "[traffiqo] Pedestrian request captured";

}  // namespace

TrafficController::TrafficController(IHardware& hardware, const TrafficConfig& config)
    : hardware_(hardware), config_(config) {}

void TrafficController::begin() {
  configurePins();
  transitionTo(Stage::Green, hardware_.millis());
}

void TrafficController::update() {
  const unsigned long now = hardware_.millis();
  sampleButton(now);
  updateStage(now);
  applyOutputs();
}

Stage TrafficController::stage() const { return stage_; }

bool TrafficController::pedestrianRequestPending() const {
  return pedestrianRequestPending_;
}

unsigned long TrafficController::stageElapsedMs() const {
  return hardware_.millis() - stageStartedAtMs_;
}

void TrafficController::configurePins() {
  hardware_.pinMode(config_.pedestrianStopPin, PinMode::Output);
  hardware_.pinMode(
      config_.requestButtonPin,
      config_.buttonActiveLow ? PinMode::InputPullup : PinMode::Input);

  for (int pin : config_.redPins) {
    hardware_.pinMode(pin, PinMode::Output);
  }
  for (int pin : config_.yellowPins) {
    hardware_.pinMode(pin, PinMode::Output);
  }
  for (int pin : config_.greenPins) {
    hardware_.pinMode(pin, PinMode::Output);
  }
}

void TrafficController::sampleButton(unsigned long now) {
  const bool pressedNow = isButtonPressed(hardware_.digitalRead(config_.requestButtonPin));

  if (pressedNow != rawButtonPressed_) {
    rawButtonPressed_ = pressedNow;
    lastButtonChangeAtMs_ = now;
  }

  if ((now - lastButtonChangeAtMs_) < config_.debounceMs) {
    return;
  }

  if (debouncedButtonPressed_ == rawButtonPressed_) {
    return;
  }

  debouncedButtonPressed_ = rawButtonPressed_;
  if (debouncedButtonPressed_) {
    setPedestrianRequest(now);
  }
}

void TrafficController::updateStage(unsigned long now) {
  const unsigned long elapsed = now - stageStartedAtMs_;

  switch (stage_) {
    case Stage::Green:
      if (elapsed >= stageDeadlineMs_) {
        transitionTo(Stage::GreenFade, now);
      }
      break;

    case Stage::GreenFade:
      greenFadeStep_ = std::min<std::size_t>(GREEN_COUNT, elapsed / config_.greenFadeStepMs);
      if (greenFadeStep_ >= GREEN_COUNT) {
        transitionTo(Stage::Yellow, now);
      }
      break;

    case Stage::Yellow:
      if (!yellowSecondLampOn_ && elapsed >= config_.yellowLeadMs) {
        yellowSecondLampOn_ = true;
      }
      if (elapsed >= (config_.yellowLeadMs + config_.yellowTailMs)) {
        transitionTo(Stage::Red, now);
      }
      break;

    case Stage::Red:
      if (elapsed >= config_.redHoldMs) {
        transitionTo(Stage::RedPrepare, now);
      }
      break;

    case Stage::RedPrepare:
      if (!yellowSecondLampOn_ && elapsed >= config_.redPrepareLeadMs) {
        yellowSecondLampOn_ = true;
      }
      if (elapsed >= (config_.redPrepareLeadMs + config_.redPrepareTailMs)) {
        transitionTo(Stage::Green, now);
      }
      break;
  }
}

void TrafficController::transitionTo(Stage nextStage, unsigned long now) {
  stage_ = nextStage;
  stageStartedAtMs_ = now;
  greenFadeStep_ = 0;
  yellowSecondLampOn_ = false;

  switch (stage_) {
    case Stage::Green:
      stageDeadlineMs_ = pedestrianRequestPending_ ? config_.minGreenMs : config_.maxGreenMs;
      hardware_.log(kStageGreen);
      break;

    case Stage::GreenFade:
      hardware_.log(kStageGreenFade);
      break;

    case Stage::Yellow:
      hardware_.log(kStageYellow);
      break;

    case Stage::Red:
      pedestrianRequestPending_ = false;
      hardware_.log(kStageRed);
      break;

    case Stage::RedPrepare:
      hardware_.log(kStageRedPrepare);
      break;
  }

  applyOutputs();
}

void TrafficController::applyOutputs() {
  const bool vehicleRedOn = (stage_ == Stage::Red || stage_ == Stage::RedPrepare);
  const bool pedestrianStopOn = (stage_ != Stage::Red);

  for (int pin : config_.redPins) {
    hardware_.digitalWrite(pin, vehicleRedOn);
  }

  bool yellowOne = false;
  bool yellowTwo = false;
  if (stage_ == Stage::GreenFade) {
    yellowOne = true;
  } else if (stage_ == Stage::Yellow || stage_ == Stage::RedPrepare) {
    yellowOne = true;
    yellowTwo = yellowSecondLampOn_;
  }

  hardware_.digitalWrite(config_.yellowPins[0], yellowOne);
  hardware_.digitalWrite(config_.yellowPins[1], yellowTwo);

  std::size_t activeGreenCount = 0;
  if (stage_ == Stage::Green) {
    activeGreenCount = GREEN_COUNT;
  } else if (stage_ == Stage::GreenFade) {
    activeGreenCount = GREEN_COUNT - greenFadeStep_;
  }

  for (std::size_t i = 0; i < GREEN_COUNT; ++i) {
    hardware_.digitalWrite(config_.greenPins[i], i < activeGreenCount);
  }

  hardware_.digitalWrite(config_.pedestrianStopPin, pedestrianStopOn);
}

void TrafficController::setPedestrianRequest(unsigned long now) {
  pedestrianRequestPending_ = true;
  hardware_.log(kPedestrianRequest);

  if (stage_ != Stage::Green) {
    return;
  }

  const unsigned long elapsed = now - stageStartedAtMs_;
  if (elapsed < config_.minGreenMs) {
    stageDeadlineMs_ = config_.minGreenMs;
    return;
  }

  const unsigned long shortenedDeadline = std::min(
      config_.maxGreenMs,
      std::max(config_.minGreenMs, elapsed + config_.requestGraceMs));
  stageDeadlineMs_ = std::min(stageDeadlineMs_, shortenedDeadline);
}

bool TrafficController::isButtonPressed(int value) const {
  const bool high = value != 0;
  return config_.buttonActiveLow ? !high : high;
}

}  // namespace traffiqo
