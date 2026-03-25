#include "src/ArduinoHardware.h"
#include "src/TrafficController.h"

traffiqo::ArduinoHardware hardware;
traffiqo::TrafficController controller(hardware);

void setup() {
  Serial.begin(115200);
  controller.begin();
}

void loop() {
  controller.update();
}
