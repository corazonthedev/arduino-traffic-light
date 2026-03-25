# UI / UX Notes

This repo has no graphical interface, so UX means **how the light system behaves for a real person**.

## Improvements made
- **Predictable crossing:** the button request is captured once and not lost.
- **Safer timing:** the vehicle green phase never cuts instantly below the minimum.
- **Smoother feedback:** green LEDs step down instead of switching off harshly.
- **Clearer pedestrian signal:** the extra red output is now treated as a real pedestrian stop indicator.
- **Debug visibility:** stage changes are written to Serial Monitor in plain language.

## Why this feels better
The original sketch worked, but from a user perspective it felt abrupt and opaque.
The new flow is easier to read on the breadboard and easier to trust while waiting to cross.
