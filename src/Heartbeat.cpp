#include <Arduino.h>

#include <Adafruit_TinyUSB.h>
#include "Heartbeat.h"

namespace FW
{
  /*
    Track the last time a heartbeat message was printed.

    millis() is used instead of delay() so the firmware can keep doing real
    work between heartbeat messages.
  */
  unsigned long Heartbeat::lastBeat = 0;

  /*
    Count how many heartbeat messages have been emitted.

    This gives the serial log a simple monotonic marker that makes it easier
    to see resets, stalls, or unexpected gaps while testing.
  */
  unsigned long Heartbeat::beatCount = 0;

  /*
    Periodic heartbeat service.

    This should be called from the main firmware loop. It prints a lightweight
    uptime message every five seconds without blocking the rest of the station.
  */
  void Heartbeat::update()
  {
    /*
      Only emit a heartbeat when the interval has elapsed.

      Keeping this check time-based lets update() be called as often as the
      main loop wants without flooding the serial console.
    */
    if (millis() - lastBeat >= 5000)
    {
      /*
        Record the current time before printing.

        This keeps the next heartbeat scheduled relative to when this one
        actually happened.
      */
      lastBeat = millis();

      /*
        Print a compact status line for serial debugging.

        The beat number confirms the loop is still alive; uptime confirms the
        board has not silently rebooted.
      */
      Serial.print("[");
      Serial.print(++beatCount);
      Serial.print("] Uptime: ");
      Serial.print(millis() / 1000);
      Serial.println(" seconds");
    }
  }
}