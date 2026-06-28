#pragma once

namespace FW {

/*
 * Shared health state used by station subsystems.
 *
 * Keeping this small and generic lets Display, Radio, Sensors, and future
 * modules report status in the same language. Detailed diagnostics stay inside
 * each module; Station only publishes the coarse health that other modules need
 * for boot screens, logs, and future remote status reports.
 */
enum class Status {
    Unknown,
    OK,
    Warning,
    Error
};

/*
 * Station tracks the overall state of this FarmWhisper device.
 *
 * The station model is intentionally not a hardware driver. It is a central
 * status board. Modules such as Radio and Display own their own implementation,
 * then Station exposes enough shared state for the rest of the firmware to make
 * consistent decisions and show consistent status text.
 */
class Station {
public:
    /* Initialize station-level state. */
    static void begin();

    /* Refresh station-level state. This must stay non-blocking. */
    static void update();

    /* Explicit setters let Boot record initialization results as they happen. */
    static void setRadio(Status status);
    static void setDisplay(Status status);
    static void setSensors(Status status);

    /* Current subsystem status values. */
    static Status radio();
    static Status display();
    static Status sensors();

    /* Convert status into compact OLED/log text. */
    static const char* statusText(Status status);

private:
    static Status m_radio;
    static Status m_display;
    static Status m_sensors;
};

} // namespace FW
