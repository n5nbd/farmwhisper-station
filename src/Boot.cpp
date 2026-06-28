/*
    ---------------------------------------------------------------------------
    FarmWhisper Boot Module
    ---------------------------------------------------------------------------

    Purpose

    The Boot module owns the top-level firmware lifecycle.

    It keeps fw-station.ino small by giving Arduino setup() and loop() one
    clean place to hand off control.

    Current Responsibilities

      • Initialize logging.
      • Initialize the station state model.
      • Initialize the OLED display.
      • Print the startup banner.
      • Dispatch recurring service updates.

    Design Notes

    Boot coordinates subsystems, but it should not contain the internal logic
    for those subsystems.

    For example:

      • Station owns station state.
      • Display owns screen rendering.
      • Heartbeat owns periodic serial heartbeat output.

    This keeps startup order explicit while preventing setup() and loop() from
    becoming dumping grounds as the firmware grows.

    ---------------------------------------------------------------------------
*/

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Boot.h"
#include "Logger.h"
#include "Heartbeat.h"
#include "BuildInfo.h"
#include "Display.h"
#include "Station.h"

namespace FW
{
    void Boot::begin()
    {
        // Logger comes first so later startup steps can report status.
        Logger::begin();

        // Station state must exist before display or future modules ask for it.
        Station::begin();

        // Display comes up early so boot/status information can be shown.
        Display::begin();

        // Print a serial startup banner after core identity is available.
        printBanner();
    }

    void Boot::loop()
    {
        // Keep the station state current.
        Station::update();

        // Refresh display content as needed.
        Display::update();

        // Emit periodic heartbeat/status output.
        Heartbeat::update();
    }

    void Boot::printBanner()
    {
        Logger::println("");
        Logger::println("========================================");
        Logger::println("      FarmWhisper");
        Logger::println("      fw-station");

        Logger::print("      Version ");
        Logger::println(FW_VERSION);

        Logger::println("");

        Logger::print("Node   : ");
        Logger::println(FW_NAME);

        Logger::print("Board  : ");
        Logger::println(FW_BOARD);

        Logger::print("Role   : ");
        Logger::println(FW_ROLE);

        Logger::println("Status : READY");
        Logger::println("");

        Logger::println("Cocka doodle doooo!");
        Logger::println("========================================");
        Logger::println("");
    }
}