#pragma once

namespace FW
{
  /*
    Display owns the small local screen used by the station.

    It is intentionally exposed as a static service so the rest of the
    firmware can ask for display startup and refresh work without passing
    display objects around.
  */
  class Display
  {
  public:
    /*
      Initialize the display hardware and draw the startup screens.

      Safe startup matters because the station should still boot even if
      the screen is missing, disconnected, or not responding.
    */
    static void begin();

    /*
      Give the display a chance to refresh or advance any screen state.

      This should stay non-blocking so the main firmware loop can continue
      servicing radio, sensors, heartbeat, and future station work.
    */
    static void update();

    /*
      Report whether the display was successfully initialized.

      Other modules can use this before depending on visible status output.
    */
    static bool isReady();

  private:
    /*
      Draw the FarmWhisper startup identity screen.

      Kept private because outside code should not decide when boot artwork
      appears; Display controls its own screen sequence.
    */
    static void showSplash();

    /*
      Draw the boot/status checklist shown during startup.

      This gives the user a quick local view of what came up cleanly.
    */
    static void showBootChecks();

    /*
      Draw the normal ready/idle screen after startup is complete.

      This is the user's confirmation that the station is alive and ready
      for the rest of the firmware loop.
    */
    static void showReady();
  };
}