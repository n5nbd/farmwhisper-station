/*
 * FarmWhisper Boot Module
 *
 * Purpose
 *   Boot owns the top-level firmware lifecycle. It keeps fw-station.ino small
 *   by giving Arduino setup() and loop() one clean place to hand off control.
 *
 * Current responsibilities
 *   - Initialize logging.
 *   - Initialize the station state model.
 *   - Initialize the OLED display.
 *   - Initialize the radio prototype layer.
 *   - Print the startup banner.
 *   - Dispatch recurring service updates.
 *
 * Design notes
 *   Boot coordinates subsystems, but it should not contain the internal logic
 *   for those subsystems. Radio owns radio behavior. Display owns rendering.
 *   Station owns shared health state. Heartbeat owns the simple alive marker.
 */

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Boot.h"
#include "BuildInfo.h"
#include "Display.h"
#include "Heartbeat.h"
#include "Logger.h"
#include "Radio.h"
#include "Station.h"

namespace FW {

void Boot::begin() {
    /* Logger comes first so later startup steps can report status. */
    Logger::begin();

    /* Station state must exist before display or future modules ask for it. */
    Station::begin();

    /* Display starts before radio so the OLED can show boot progress. */
    Display::begin();

    /* Bring up the first raw LoRa P2P radio prototype layer. */
    const bool radioOk = Radio::begin();
    Station::setRadio(radioOk ? Status::OK : Status::Error);

    /* Print a serial startup banner after core identity/status is available. */
    printBanner();
}

void Boot::loop() {
    /* Service radio first so TX/RX completion flags are handled quickly. */
    Radio::update();

    /* Keep the shared station status model current. */
    Station::update();

    /* Refresh display content as needed. */
    Display::update();

    /* Emit periodic heartbeat/status output. */
    Heartbeat::update();
}

void Boot::printBanner() {
    Logger::println("");
    Logger::println("========================================");
    Logger::println(" FarmWhisper");
    Logger::println(" fw-station");
    Logger::print(" Version ");
    Logger::println(FW_VERSION);
    Logger::println("");
    Logger::print("Node : ");
    Logger::println(FW_NAME);
    Logger::print("Board : ");
    Logger::println(FW_BOARD);
    Logger::print("Role : ");
    Logger::println(FW_ROLE);
    Logger::print("Radio : ");
    Logger::println(Station::statusText(Station::radio()));
    Logger::println("");
    Logger::println("Cocka doodle doooo!");
    Logger::println("========================================");
    Logger::println("");
}

} // namespace FW
