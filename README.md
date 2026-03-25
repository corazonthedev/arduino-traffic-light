# Traffiqo

> A cleaner, testable, non-blocking Arduino traffic light controller with pedestrian request handling.

Traffiqo is a rebuilt version of the original traffic light sketch. It keeps the original breadboard intent and pin layout, but replaces the fragile `delay()`-driven flow with a modular state machine that is easier to test, extend, and trust.

## Highlights

- **Non-blocking controller** powered by `millis()` instead of long `delay()` chains
- **Pedestrian request queue** with debounce and safer timing behavior
- **Modular architecture** with hardware abstraction and controller separation
- **Native desktop tests** for logic validation without flashing a board every time
- **Improved UX behavior** with smoother green fade, clearer crossing flow, and predictable signal changes
- **Original-friendly migration** by preserving the existing pin layout

## Why this version is better

The original sketch worked as a simple proof of concept, but it was difficult to maintain and hard to verify.
This version focuses on the things that make a project feel solid:

- explicit states instead of hidden timing behavior
- safer handling of pedestrian button presses
- cleaner extension path for buzzer, countdown display, night mode, or emergency override
- testable logic outside the Arduino IDE

## Feature Set

### Traffic flow
- vehicle traffic starts on green
- green is guaranteed for a minimum safe window
- pedestrian requests are queued instead of being lost
- active requests shorten the remaining green **only when safe**
- transition sequence uses a smoother green step-down, yellow warning, and controlled red crossing window

### User experience improvements
- button presses feel responsive and reliable
- crossing behavior is predictable instead of abrupt
- pedestrian indicator behaves as a real stop/walk signal
- serial logs make debugging and hardware bring-up much easier

### Developer experience improvements
- controller logic is isolated from Arduino-specific I/O
- timing values are centralized in configuration
- logic can be tested locally with a standard C++ compiler
- docs describe architecture, setup, and UX decisions

## Project Structure

```text
.
├── traffiqo.ino
├── src/
│   ├── ArduinoHardware.h
│   ├── TrafficConfig.h
│   ├── TrafficController.cpp
│   ├── TrafficController.h
│   └── TrafficHardware.h
├── tests/
│   ├── MockHardware.h
│   └── test_traffic_controller.cpp
├── docs/
│   ├── ADR.md
│   ├── SETUP_GUIDE.md
│   └── UI_UX_NOTES.md
├── CHANGELOG.md
├── CONTRIBUTING.md
└── Makefile
```

## Hardware Layout

| Purpose | Pin(s) |
|---|---:|
| Vehicle red LEDs | 2, 3, 4 |
| Pedestrian stop LED | 5 |
| Vehicle yellow LEDs | 6, 7 |
| Vehicle green LEDs | 8, 9, 10, 11 |
| Pedestrian request button | 13 |

> The pedestrian button is configured as **active low**.

## Default Timing Profile

| Action | Time |
|---|---:|
| Minimum vehicle green | 3000 ms |
| Maximum vehicle green | 8000 ms |
| Request grace after minimum green | 1200 ms |
| Green fade step | 180 ms |
| Yellow window | 1100 ms total |
| Vehicle red / pedestrian crossing | 5000 ms |
| Red-to-green prepare | 640 ms total |

All timing constants live in [`src/TrafficConfig.h`](src/TrafficConfig.h).

## Quick Start

### Run on Arduino
1. Open the project in **Arduino IDE 2.x**.
2. Open `traffiqo.ino`.
3. Wire the LEDs and button to the pins listed above.
4. Select your board and port.
5. Upload the sketch.
6. Open **Serial Monitor** at `115200` baud.

### Run logic tests locally

```bash
make test
```

This builds the controller as a native C++ executable and validates traffic-cycle behavior without requiring physical hardware.

## Behavior Overview

1. Vehicles begin on green.
2. If no pedestrian request exists, green can continue until the configured maximum.
3. If the button is pressed, the request is debounced and queued.
4. Once the minimum green window is satisfied, the controller safely shortens the remaining green.
5. The system steps through green fade, yellow warning, red crossing, and prepare stages.
6. Requests received during red are preserved for the next cycle.

## Architecture Overview

Traffiqo is built around a small controller core:

- **`TrafficController`** manages timing, requests, and stage transitions
- **`TrafficHardware`** defines the I/O contract
- **`ArduinoHardware`** connects the contract to the board pins
- **`MockHardware`** enables deterministic tests on desktop

That split is what makes the project both Arduino-friendly and testable.

## Documentation

- [`docs/SETUP_GUIDE.md`](docs/SETUP_GUIDE.md) — hardware and upload setup
- [`docs/ADR.md`](docs/ADR.md) — architecture decisions
- [`docs/UI_UX_NOTES.md`](docs/UI_UX_NOTES.md) — behavior and experience notes
- [`CHANGELOG.md`](CHANGELOG.md) — change history
- [`CONTRIBUTING.md`](CONTRIBUTING.md) — contribution guidance

## Good Next Upgrades

- buzzer feedback for accessibility
- OLED or LCD countdown display
- blinking yellow night mode
- emergency override input
- richer serial diagnostics or simulation support

## Philosophy

This project is intentionally simple at the hardware level and more disciplined at the software level.
The goal is not to turn a breadboard demo into enterprise software.
The goal is to make a small Arduino project feel reliable, readable, and worth building on.
