#pragma once

namespace FW
{
  /*
    Heartbeat provides a simple periodic "I'm still alive" indication.

    During development it outputs a short status message to the serial console
    at a fixed interval. This makes it easy to verify that the firmware is
    still running, the main loop has not stalled, and unexpected resets have
    not occurred.

    The heartbeat intentionally performs no blocking operations so it can be
    called from the main loop every iteration.
  */
  class Heartbeat
  {
  public:
    /*
      Service the heartbeat timer.

      Call this once per pass through the main firmware loop. When the
      configured interval has elapsed, a single heartbeat message is emitted.
      Otherwise, this function returns immediately.
    */
    static void update();

  private:
    /*
      Timestamp (from millis()) when the last heartbeat was emitted.

      Used to determine when the next heartbeat should be printed without
      delaying or blocking the rest of the firmware.
    */
    static unsigned long lastBeat;

    /*
      Sequential heartbeat counter.

      Incremented each time a heartbeat is emitted to make it easy to spot
      missed intervals, resets, or other unexpected behavior while reviewing
      serial logs.
    */
    static unsigned long beatCount;
  };
}