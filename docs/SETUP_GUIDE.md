# Setup Guide

## Hardware
- Arduino Uno or compatible board
- 3 red LEDs
- 2 yellow LEDs
- 4 green LEDs
- 1 LED for pedestrian stop
- 1 push button for crossing request
- Suitable resistors and jumper wires

## Wiring
- Red LEDs -> pins 2, 3, 4
- Pedestrian stop LED -> pin 5
- Yellow LEDs -> pins 6, 7
- Green LEDs -> pins 8, 9, 10, 11
- Button -> pin 13 (active low)

## Upload
1. Open `traffiqo.ino` in Arduino IDE 2.x.
2. Select the correct board and port.
3. Upload.
4. Open Serial Monitor at 115200 baud.

## Native logic tests
Run this on a desktop machine with `g++` installed:

```bash
make test
```

## What success looks like
- Green starts active.
- Button press is captured once.
- Green shortens only after the minimum safe time.
- Pedestrian stop LED turns off during the red crossing window.
