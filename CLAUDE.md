# CLAUDE.md

Firmware for a wood-burning boiler controller (ESP8266 NodeMCU, Arduino framework, PlatformIO). See `README.md` for the overview. The owner writes in Swedish; code, comments and commit messages are in English, user-facing notification text is Swedish.

## Build / deploy

PlatformIO (`pio` is usually not on PATH on Windows; use `%USERPROFILE%\.platformio\penv\Scripts\pio.exe`).

- Build: `pio run`
- Upload firmware: `pio run -t upload` — goes **over the air** (`espota` to `192.168.1.100`), so the device must be on the network. This reflashes the live boiler controller; don't upload unless asked.
- Upload web UI/data: `pio run -t uploadfs` (LittleFS image from `data/`)
- `gzip_html.py` (pre-script on `buildfs`) gzips `src/index.html` to `data/index.html.gz` (gitignored). Edit `src/index.html`, never the `.gz`.
- No automated tests. `test/simulation.cpp` is a standalone PID simulator compiled by hand with g++ (`test/*.o`, `a.out`, `sim` are stray build artifacts).

## Architecture

All control logic is in `src/main.cpp`: `setup()` plus a `loop()` that runs every 500 ms and does slower work via `Timer` (5 s temperature read, 3 min PID step). Libraries in `lib/`:

- `AirDamper` — stepper motor on the primary air inlet (`moveToStep` absolute, `moveTo` relative, `shutdown` sleeps the driver).
- `PIDController` — homebrewed PID; output is a stepper correction, driven by exhaust temperature vs `targetExhaustTemperature` (195).
- `BurnLogger` — shared `Stats` singleton (`BurnLogger::getStats()`) read/written by main loop and web server; persists burn log/statistics as JSON in LittleFS (`data/logs.json`, `info.json`, `statistics.json`).
- `WebserverConfiguration` — WiFi, ArduinoOTA, and the web GUI/JSON endpoints.
- `Display` — SH1106 OLED.
- `Timer` — `hasPassed(s, now)` returns true once per interval per distinct interval value (keyed by interval, so two callers using the same interval share a timer).
- `Notifier` — ntfy.sh push notifications (blocking HTTPS POST, ~1–2 s, `setInsecure()`). `checkNotifications()` in `main.cpp` sends: no fire (≤50 °C after 15 min), boiler hot (≥120 °C first time), refill reminder (≤110 °C after being hot, within 2 h). Each fires once per boot; the flag is only set when the send succeeds. Thresholds are `#define`s at the top of `main.cpp`.

Key concepts:
- "Burn time" and all notification windows count from **boot** (`millis()`), not from an explicit fire-start event, so the device is restarted for each new firing.
- Two MAX6675 thermocouples: water (`waterTemperature`, heating stops at 100 °C) and exhaust (`exhaustTemperature`, used for PID and "reached temperature" at 155 °C). "Boiler temperature" in notification logic means exhaust.
- `stats.heating` drives the relay (exhaust fan + room air inlet) and parks the damper at 0 when false.

## Gotchas

- Hardware constraints: ESP8266 has ~80 KB RAM; keep TLS buffers small, avoid big `String`/JSON allocations, and call `yield()` in long operations. Pins 1/3 (TX/RX) are used for the stepper, so **no Serial output**.
- Avoid frequent LittleFS writes (a past bug wrote `statistics.json` repeatedly — flash wear).
- `main.cpp` contains WiFi credentials and the ntfy topic as literals (existing practice in this repo); the ntfy topic acts as the only secret.
- Notification HTTP headers (title/tags) must be ASCII; Swedish characters are fine in the message body only.
- Changing PID gains: comments in `main.cpp` and `test/simulation.cpp` record what worked/was too aggressive; try changes in the simulator first.
