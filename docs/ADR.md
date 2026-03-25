# Architecture Decision Record

## ADR-001 — Use a non-blocking state machine
- **Decision:** Replace `delay()`-driven sequencing with `millis()`-based stage transitions.
- **Alternatives considered:** Keep the single-file `delay()` approach.
- **Why this won:** Easier testing, smoother behavior, better extension path.

## ADR-002 — Separate hardware from control logic
- **Decision:** Create a platform-agnostic controller plus an Arduino adapter.
- **Alternatives considered:** Keep all logic directly inside the `.ino` file.
- **Why this won:** Enables native desktop tests and cleaner maintenance.

## ADR-003 — Preserve the original pin layout
- **Decision:** Keep the same LED and sensor pins used in the original project.
- **Alternatives considered:** Reassign pins to simplify the code.
- **Why this won:** Lower friction for the original breadboard wiring and easier migration.
