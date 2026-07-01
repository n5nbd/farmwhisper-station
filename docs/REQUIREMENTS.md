# FarmWhisper Requirements

Last updated: 2026-07-01

This document captures the current FarmWhisper requirements and design decisions from recent project discussions. It is ordered from stable product direction to current implementation, then future/deferred ideas, then superseded decisions. When an earlier brainstorm conflicts with a later decision, the later/current decision wins.

## 1. Product intent

FarmWhisper is an open, repairable, local-first farm monitoring and control ecosystem.

The goal is to make farming easier through useful information and practical automation without turning the user into an IoT expert.

Core principles:

- No required cloud dependency.
- No subscriptions.
- No vendor lock-in.
- Privacy by principle.
- Commodity/off-the-shelf repairable modules where practical.
- Local control first.
- Long-term maintainability matters.
- Documentation and comments are product features, not decoration.

FarmWhisper should feel practical, rugged, understandable, and repairable.

## 2. Brand and market requirements

Brand direction:

- Product/project name: `FarmWhisper`
- FarmWhisper is one word.
- Working tagline: `Your own automated farm whisperer.`
- `FarmWhisper`, the FarmWhisper logo, and `RoboRooster` are project-owner marks.
- Code can be MIT licensed, but trademark/brand use should be covered separately.

Primary market:

- Small farms.
- Homesteads.
- Landowners.
- Practical users with roughly 5-100 acres.
- Users who want useful monitoring and control without becoming embedded/IoT/radio specialists.

Market entry:

- Chickens are the roots/gateway market.
- The system must not be poultry-only.
- Poultry use cases are the initial beachhead, not the architectural boundary.

## 3. System architecture requirements

FarmWhisper device classes:

- Stations
- Sensors
- Annunciators
- Actuators

Stations are the local brains. They gather sensor data, make local decisions, expose local status/configuration, and control local actuators when appropriate.

Sensors should be simple, modular, and replaceable where practical.

Annunciators provide human-facing alerts/status.

Actuators perform controlled actions such as switching power, relays, doors, fans, lamps, or other farm equipment.

Architecture requirements:

- LoRa is the backbone communication layer.
- BLE is preferred for local phone configuration, status, and nearby alerts.
- USB is preferred for base-station/computer interfaces.
- Wi-Fi should not be a required dependency or a marketing association.
- Stations may be ESP32 or Raspberry Pi depending on the job.
- Use displays appropriate to the station/function/location.
- A station may have one or two built-in relays where that makes the product easier.
- Common station/device behavior should favor one-button and one-RGB-status-light patterns.
- 3.3V logic is the preferred standard.
- Keep products locally useful even if no phone, network, or internet is available.

## 4. Radio and protocol requirements

FarmWhisper uses LoRa as the field backbone.

Current US development radio profile:

```text
Default channel : CH09
Frequency       : 913.500 MHz
Bandwidth       : 500 kHz
Spreading factor: SF7
Coding rate     : CR4/5
Preamble        : 8
Sync word       : 0x12
TX power        : 14 dBm
```

Current channel plan source:

```text
shared/fwRadioInfo-US.h
```

Radio plan requirements:

- The shared radio info header is the single source of truth for tool channel/profile settings.
- Tools must not keep stale duplicated hardcoded frequency/BW/SF/CR/preamble constants.
- FarmWhisper US tools currently use 13 fixed channel slots.
- The scanner should scan the FarmWhisper channel plan, not the whole band.
- The current design direction is fixed-channel operation, not frequency hopping.
- The current regulatory design path is a 902-928 MHz digitally modulated / DTS-style path using 500 kHz bandwidth.
- Shared helpers should expose user-facing channel numbers, center frequencies, default LoRa profile, and safe channel conversions.
- Final product hardware still requires module/certification/legal review; shared constants are development defaults, not certification.

Coding rate note:

```text
SX126x-Arduino CR index 1 = CR4/5
SX126x-Arduino CR index 2 = CR4/6
SX126x-Arduino CR index 3 = CR4/7
SX126x-Arduino CR index 4 = CR4/8
```

So serial output showing `CR index : 1` is correct for `CR4/5`.

FarmWhisper packet direction:

- Current tool packets use a FarmWhisper `HELLO` style packet.
- Packet payloads should be human-readable where practical during early tools work.
- Packet monitor display should show decoded payload text, not binary frame header junk.
- Long payloads should scroll rather than being truncated in a confusing way.

## 5. Hardware platform requirements

Current RAK tool hardware:

- RAK4631 Core
- RAK19003 or RAK5005 base as appropriate
- SX1262 LoRa radio
- SH1106G OLED on `Wire` at `0x3C`
- Battery voltage read from `WB_A0`
- Arduino FQBN: `rakwireless:nrf52:WisCoreRAK4631Board`

RAK tools are the current preferred tool platform.

Heltec V4 status:

- Heltec V4 is interesting, and may be useful later.
- Drop Heltec support for now.
- Stick to RAK hardware until the tools and platform are more mature.
- Ignore/delete old `heltec-v4-packet-tool` work for now.

Station hardware direction:

- ESP32 is appropriate for many station-class products.
- Raspberry Pi is appropriate where the job needs more compute, storage, UI, networking, or Linux behavior.
- Avoid forcing every product into one MCU/platform.
- Pick the station brain based on job requirements.

Power dock architecture:

- Interchangeable station brains should be able to dock into reusable power-management boxes.
- 2-, 4-, and 6-outlet power boxes are variants of the same design.
- Scaling outlet count can be done by adding SSR/relay channels and assigning more GPIO or PCF8575 outputs.
- 3.3V logic remains the preferred interface standard.
- Power docks should be reusable across multiple station/product types.

## 6. Tooling requirements

Current FarmWhisper tools repo:

```text
~/Arduino/farmwhisper-tools
```

Current tools:

```text
fwBeaconTool
fwPacketMonitor
fwSiteScanner
shared/fwRadioInfo-US.h
```

Repo and build requirements:

- Keep normal local edit/build/test/commit/push workflow.
- Keep README files updated as part of normal changes.
- Prefer local replacement files or zip packages for large documentation updates instead of huge terminal paste jobs.
- Use one command at a time during command-line troubleshooting.
- Avoid chains of commands with conditional next steps because partial failures make later commands wasted or confusing.
- Use camelCase naming for project/file/sketch names and code identifiers where practical.
- Avoid underscores in project/file/sketch names and code identifiers when choosing new names.
- Avoid the word `utilize`; use `use`.

Arduino CLI pattern:

```bash
arduino-cli compile \
  --fqbn rakwireless:nrf52:WisCoreRAK4631Board \
  --build-property "compiler.cpp.extra_flags=-I/home/zim/Arduino/farmwhisper-tools/shared" \
  fwBeaconTool
```

Arduino IDE include handling:

- Sketches should not be changed back and forth just to satisfy IDE vs VSCode.
- The local Arduino-library shim is the accepted workaround for Arduino IDE include discovery:

```text
~/Arduino/libraries/FarmWhisperRadioInfo/
  library.properties
  src/fwRadioInfo-US.h -> /home/zim/Arduino/farmwhisper-tools/shared/fwRadioInfo-US.h
```

## 7. fwBeaconTool requirements

Purpose:

- Generate known FarmWhisper LoRa traffic.
- Help test monitors, scanners, antennas, range, and shared radio configuration.

Current behavior:

```text
Node name : COOP
Packet    : FarmWhisper HELLO
Interval  : 15000 ms
Payload   : name=COOP bat=x.xxv up=seconds
```

Requirements:

- Use `shared/fwRadioInfo-US.h`.
- Do not hardcode old 915 MHz / BW125 settings.
- Print visible version/header information.
- Print startup radio configuration to serial.
- Startup serial config must include channel, frequency, BW Hz, BW index, SF, CR index, preamble, sync word, TX power, and interval.
- Keep it simple; this is a field/debug beacon, not full station firmware.

## 8. fwPacketMonitor requirements

Purpose:

- Park on one FarmWhisper channel.
- Decode and display FarmWhisper packets reliably.
- Confirm that a beacon or station is transmitting on the expected shared radio profile.

Requirements:

- Use `shared/fwRadioInfo-US.h`.
- Show visible version/header information.
- Print startup radio configuration to serial.
- Show shared channel/profile information on the OLED.
- Keep battery voltage visible in `x.xxv` format.
- Decode FarmWhisper payload text and display the payload, not binary header bytes.
- Scroll long payloads on one line.
- Use an SNR gauge with a practical visual range.
- Keep this as a single `.ino` for now.
- Do not turn it into the full abstract station/application structure yet.

## 9. fwSiteScanner requirements

Purpose:

- Sweep FarmWhisper channel slots.
- Build a practical RF dirt map.
- Help choose a relatively quiet FarmWhisper channel.
- Prefer practical field usefulness over lab-grade spectrum measurement.

Scanner philosophy:

```text
RSSI terrain = broad noise picture
CAD hits     = LoRa-shaped activity
Good decode  = jackpot evidence; slam that bin harder
```

Requirements:

- Use `shared/fwRadioInfo-US.h`.
- Scan only the FarmWhisper channel plan.
- Sample RSSI.
- Run CAD.
- Count opportunistic decoded packet hits.
- Draw cumulative grey/stippled terrain for long-term ugliness.
- Draw latest RSSI as narrow white bars over the terrain.
- Draw the sweep tick at the bottom.
- Calculate/display best only after a terrain bin reaches full scale.
- Stop scanning when any terrain bin reaches full scale.
- A packet decode must make a visible terrain jump.
- Packet decode weighting should be easy to tune and documented near the code.
- Do not slow the scanner just to catch beacons.
- Longer dwell helps only the channel currently scanned and hurts the other channels.
- Packet decodes are opportunistic evidence while sweeping, not the main detection method.
- Use `fwPacketMonitor` when reliable packet decoding is the goal.

Current scanner timing direction:

```text
RSSI sample window : about 240 ms
CAD wait           : about 120 ms
13-channel sweep   : roughly 5-6 seconds
```

Current packet decode terrain tuning notes:

```text
terrainFullScalePoints / 5 = 20%
terrainFullScalePoints / 4 = 25%
terrainFullScalePoints / 3 = 33%
terrainFullScalePoints / 2 = 50%
```

## 10. Firmware/framework requirements

Station firmware direction:

- `farmwhisper-station` is the current station firmware repo.
- The station framework should stay readable and heavily commented.
- Bigger source files are acceptable if comments preserve why decisions were made.
- Comments should explain reasoning, not just restate code.
- Documentation in code is valuable for future maintainers, advanced users, and third-party expansion.

Future `farmwhisper-core` direction:

- `farmwhisper-core` should hold common protocol and radio/application core pieces.
- It should include FWP protocol, packet definitions, message types, versioning, serialization/deserialization, validation, radio abstraction, common data types, and utilities.
- It should not contain OLED, relay, HX711, temp, incubator, chicken-door, or product-specific features.

Repo creation constraint:

- Do not create `farmwhisper-load` or `farmwhisper-incubator` repos yet.
- Create those only when those projects formally start.

## 11. UI/UX requirements

General UI:

- Keep user-facing device behavior simple.
- Favor one-button workflows when possible.
- Favor one RGB status light pattern where possible.
- Displays should show practical status, not debug noise, unless the tool is explicitly diagnostic.
- Field tools may be more verbose because they are diagnostic.

OLED tool UI:

- SH1106G OLED on `Wire` at `0x3C` is the current RAK tool display.
- Use readable, compact 128x64 layouts.
- Battery voltage should appear as `x.xxv`.
- Packet monitor should prioritize payload readability.
- Scanner should prioritize the channel terrain picture.

## 12. Sensors and measurement requirements

General sensor strategy:

- Use COTS rugged sensing where practical.
- Prefer sensors/modules that are easy to source, replace, and understand.
- Automotive sensors are worth studying because rugged packaging and environmental hardening are already solved.

ToF / distance / level sensing:

- Time-of-flight sensors are promising for level measurement.
- Pellet/dust anecdotes suggest ToF can be practical in dusty pellet environments if protected/cleaned occasionally.
- Calibration method for tanks/bins:
  - Fill the tank/bin.
  - Take a full reading.
  - Drain/use it down while tracking readings.
  - Learn shape/capacity curve, including conical bottoms.
- `Usable empty` can be defined as the point where the sensor stops changing, not literal physical zero.

Ultrasonic / automotive sensing:

- Bosch/automotive ultrasonic parking sensors are a back-shelf rugged distance/proximity candidate.
- Keep this as a future rugged sensing strategy, not a current requirement for the first implementation.

Load cells:

- The current load-cell mule direction is a base that holds a propane tank.
- Use four load cells, one at each corner, with the supplied amplifier.
- The hanging arrangement is deferred/skipped for now.

Radiation / pulse counting:

- Geiger tubes can potentially be integrated through pulse counting.
- Tapping speaker/pulse output is a possible experimental path.
- Radiation sensing can be a FarmWhisper environmental sensor.
- Capacity-style management is not needed for radiation; simple environmental trend/count monitoring is enough.
- The same pulse-counter concept may apply to other devices, including wind turbine/anemometer comparisons.

Trend interface ideas:

```cpp
setThresholdHigh()
setThresholdLow()
getTrend()
```

These are concept-level API ideas, not locked final names.

## 13. Actuation and power-control requirements

Actuators should be local-first and safe.

Power-control boxes:

- Support 2-, 4-, and 6-outlet variants.
- Use SSRs or relays as appropriate.
- GPIO expanders such as PCF8575 are acceptable for scaling outputs.
- Station brains should be interchangeable with reusable power boxes where practical.
- Keep field wiring and repairability in mind.

## 14. Deferred / back-shelf ideas

These ideas are not rejected, but they are not current requirements.

Back-shelf items:

- Heltec V4 packet tool.
- MeshCore/Meshtastic station exploration.
- APRS over LoRa on amateur bands.
- ATS mini controller.
- Bosch ultrasonic parking sensor integration.
- Radiation sensor productization.
- Wind turbine pulse/load comparison.
- Incubator-specific repo/product.
- Load-cell-specific repo/product.
- Advanced phone app/cloud-like features.

Rules for deferred ideas:

- Do not let deferred ideas distort current tool requirements.
- Do not create dedicated repos until the project formally starts.
- Keep notes, but avoid mixing speculative features into current firmware requirements.

## 15. Superseded or rejected decisions

These were considered or existed earlier, but should not drive current requirements.

Superseded:

- Old hardcoded 915.000 MHz / BW125 tool configs.
- `fwChannelPlan.h` as the standard channel source.
- Scanner showing/calculating best live before enough terrain history fills.
- Timer-based scanner completion when bin-fill behavior became preferred.
- Heltec V4 as the current packet-tool target.
- Whole-band scanning for the site scanner.
- Treating packet decode hits as minor evidence.

Current replacements:

- Use `shared/fwRadioInfo-US.h`.
- Use CH09 / 913.500 MHz / BW500 / SF7 / CR4/5 defaults.
- Scanner completes when a terrain bin fills.
- Scanner scans FarmWhisper channel slots only.
- RAK hardware is the current tool platform.
- Packet decodes produce a visible terrain jump.

## 16. Documentation requirements

Documentation is mandatory.

Requirements:

- Keep root and per-tool README files current.
- Update docs in the same workflow as code changes.
- Prefer downloadable replacement files or zip packages for large doc updates.
- Do not rely on editing through GitHub web UI for normal changes.
- Important design reasoning should live in comments and Markdown so future maintainers understand why the code is shaped the way it is.
- Documentation should describe current accepted behavior, not every abandoned brainstorm as if it were active.

## 17. Current accepted implementation snapshot

As of this update, the current accepted state is:

- `farmwhisper-tools` has working RAK tools:
  - `fwBeaconTool`
  - `fwPacketMonitor`
  - `fwSiteScanner`
  - `shared/fwRadioInfo-US.h`
- Shared radio config is working across the tools.
- Beacon transmits `HELLO` packets with visible startup radio config.
- Packet monitor receives and displays payloads.
- Site scanner uses shared slots/profile and packet decode terrain weighting.
- README files have been updated.
- Tools are committed/pushed through normal local git workflow.
- Next likely hands-on hardware thread may be ToF sensor bring-up.
