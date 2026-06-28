#pragma once

/*
    ---------------------------------------------------------------------------
    FarmWhisper Boot Module
    ---------------------------------------------------------------------------

    Coordinates the overall firmware lifecycle.

    The Arduino sketch intentionally contains almost no application logic.
    Instead, setup() and loop() immediately transfer control to this module.

    Boot is responsible for coordinating startup and recurring system services,
    while individual modules remain responsible for their own implementation.

    Current Startup Sequence

      1. Initialize logging.
      2. Initialize station state.
      3. Initialize the display.
      4. Display firmware identification.

    Current Runtime Responsibilities

      • Update station state.
      • Refresh the display.
      • Run the heartbeat service.

    Design Philosophy

    Boot should answer one question:

        "What happens after power is applied?"

    It should never become the implementation of every subsystem. Instead,
    Boot coordinates modules while those modules own their own behavior.

    Keeping Boot small and readable makes it easy for new contributors to
    understand the firmware lifecycle and safely extend it.

    ---------------------------------------------------------------------------
*/

namespace FW
{
    class Boot
    {
    public:

        // Perform one-time system initialization.
        static void begin();

        // Execute recurring system services.
        static void loop();

    private:

        // Display firmware identification information on the serial console.
        static void printBanner();
    };
}