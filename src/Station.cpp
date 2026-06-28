#include "Station.h"
#include "Radio.h"

namespace FW {

/*
 * Cached subsystem health.
 *
 * These defaults describe a board before startup has proved anything. Boot and
 * individual modules promote them as hardware comes online.
 */
Status Station::m_radio = Status::Unknown;
Status Station::m_display = Status::Unknown;
Status Station::m_sensors = Status::Unknown;

void Station::begin() {
    /*
     * Display is currently treated as OK because the existing Display module
     * fails quietly if no OLED is present and does not yet report hardware
     * status back to Station. A future Display::isAvailable() can make this more
     * exact without changing callers.
     */
    m_display = Status::OK;
    m_radio = Status::Unknown;
    m_sensors = Status::Unknown;
}

void Station::update() {
    /*
     * The radio can fail during runtime even after a clean boot. For this first
     * prototype we only track whether it initialized. Later, repeated TX/RX
     * errors can promote radio status to Warning or Error.
     */
    if (Radio::stats().initialized) {
        m_radio = Status::OK;
    }
}

void Station::setRadio(Status status) {
    m_radio = status;
}

void Station::setDisplay(Status status) {
    m_display = status;
}

void Station::setSensors(Status status) {
    m_sensors = status;
}

Status Station::radio() {
    return m_radio;
}

Status Station::display() {
    return m_display;
}

Status Station::sensors() {
    return m_sensors;
}

const char* Station::statusText(Status status) {
    switch (status) {
        case Status::OK:
            return "OK";
        case Status::Warning:
            return "WARN";
        case Status::Error:
            return "ERROR";
        case Status::Unknown:
        default:
            return "--";
    }
}

} // namespace FW
