#include <cassert>
#include <iostream>
#include <string>

#include "../src/TrafficController.h"
#include "MockHardware.h"

using traffiqo::PinMode;
using traffiqo::Stage;
using traffiqo::TrafficConfig;
using traffiqo::TrafficController;
using traffiqo::MockHardware;

namespace {

void tick(TrafficController& controller, MockHardware& hardware, unsigned long advanceMs) {
  hardware.nowMs += advanceMs;
  controller.update();
}

void assertPin(MockHardware& hardware, int pin, bool expected, const std::string& label) {
  const bool actual = hardware.digitalOutputs[pin];
  if (actual != expected) {
    std::cerr << "Expected pin " << pin << " for " << label << " to be "
              << (expected ? "HIGH" : "LOW") << " but got "
              << (actual ? "HIGH" : "LOW") << std::endl;
    std::abort();
  }
}

void test_initial_green_state() {
  MockHardware hardware;
  TrafficController controller(hardware);

  controller.begin();
  controller.update();

  assert(controller.stage() == Stage::Green);
  assertPin(hardware, 8, true, "green led 1");
  assertPin(hardware, 11, true, "green led 4");
  assertPin(hardware, 5, true, "pedestrian stop");
  assertPin(hardware, 2, false, "vehicle red");
}

void test_active_low_button_uses_pullup_mode() {
  MockHardware hardware;
  TrafficController controller(hardware);

  controller.begin();

  assert(hardware.pinModes[13] == PinMode::InputPullup);
}

void test_button_shortens_green_after_minimum() {
  MockHardware hardware;
  TrafficConfig config;
  TrafficController controller(hardware, config);

  controller.begin();
  controller.update();

  tick(controller, hardware, 3100);
  hardware.setButtonLow(config.requestButtonPin);
  tick(controller, hardware, 40);
  tick(controller, hardware, config.requestGraceMs + 10);
  tick(controller, hardware, 2500);

  assert(controller.stage() == Stage::GreenFade);
  assert(controller.pedestrianRequestPending());
}

void test_button_before_minimum_waits_until_minimum_green() {
  MockHardware hardware;
  TrafficConfig config;
  TrafficController controller(hardware, config);

  controller.begin();
  controller.update();

  tick(controller, hardware, 1000);
  hardware.setButtonLow(config.requestButtonPin);
  tick(controller, hardware, 40);
  tick(controller, hardware, 1500);

  assert(controller.stage() == Stage::Green);
  tick(controller, hardware, 500);
  assert(controller.stage() == Stage::GreenFade);
}

void test_button_during_red_queues_next_green_transition() {
  MockHardware hardware;
  TrafficConfig config;
  TrafficController controller(hardware, config);

  controller.begin();
  controller.update();

  tick(controller, hardware, config.maxGreenMs + 1);
  tick(controller, hardware, config.greenFadeStepMs * 4 + 1);
  tick(controller, hardware, config.yellowLeadMs + config.yellowTailMs + 1);
  assert(controller.stage() == Stage::Red);

  hardware.setButtonLow(config.requestButtonPin);
  tick(controller, hardware, 1);
  tick(controller, hardware, 40);
  assert(controller.pedestrianRequestPending());

  tick(controller, hardware, config.redHoldMs + 1);
  tick(controller, hardware, config.redPrepareLeadMs + config.redPrepareTailMs + 1);
  assert(controller.stage() == Stage::Green);
  assert(controller.pedestrianRequestPending());

  tick(controller, hardware, config.minGreenMs - 1);
  assert(controller.stage() == Stage::Green);
  tick(controller, hardware, 1);
  assert(controller.stage() == Stage::GreenFade);
}

void test_red_phase_clears_request_and_opens_crossing() {
  MockHardware hardware;
  TrafficConfig config;
  TrafficController controller(hardware, config);

  controller.begin();
  controller.update();

  tick(controller, hardware, config.maxGreenMs + 1);
  tick(controller, hardware, config.greenFadeStepMs * 4 + 1);
  tick(controller, hardware, config.yellowLeadMs + config.yellowTailMs + 1);

  assert(controller.stage() == Stage::Red);
  assert(!controller.pedestrianRequestPending());
  assertPin(hardware, 5, false, "pedestrian walk enabled");
  assertPin(hardware, 2, true, "vehicle red 1");
  assertPin(hardware, 4, true, "vehicle red 3");
}

void test_cycle_returns_to_green() {
  MockHardware hardware;
  TrafficConfig config;
  TrafficController controller(hardware, config);

  controller.begin();
  controller.update();

  tick(controller, hardware, config.maxGreenMs + 1);
  tick(controller, hardware, config.greenFadeStepMs * 4 + 1);
  tick(controller, hardware, config.yellowLeadMs + config.yellowTailMs + 1);
  tick(controller, hardware, config.redHoldMs + 1);
  tick(controller, hardware, config.redPrepareLeadMs + config.redPrepareTailMs + 1);

  assert(controller.stage() == Stage::Green);
  assertPin(hardware, 8, true, "green led returns");
  assertPin(hardware, 5, true, "pedestrian stop returns");
}

}  // namespace

int main() {
  test_initial_green_state();
  test_active_low_button_uses_pullup_mode();
  test_button_shortens_green_after_minimum();
  test_button_before_minimum_waits_until_minimum_green();
  test_button_during_red_queues_next_green_transition();
  test_red_phase_clears_request_and_opens_crossing();
  test_cycle_returns_to_green();

  std::cout << "All native traffic controller tests passed." << std::endl;
  return 0;
}
