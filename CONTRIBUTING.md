# Contributing

## Local workflow
1. Change logic in `src/`.
2. Run `make test`.
3. Verify the sketch still matches your wiring.
4. Open a pull request with a short description of the behavior change.

## Rules
- Keep the pin map backward-compatible unless there is a clear reason to change it.
- Prefer non-blocking logic over `delay()`.
- Add or update a test when controller behavior changes.
- Keep Serial messages short and human-readable.
