#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Logger.h"

namespace FW
{
  /*
    Initialize the serial logging interface.

    Logger is intentionally responsible for all serial initialization so the
    rest of the firmware does not need to know how logging is implemented.
    This also gives us a single place to add future features such as log
    levels, alternate transports, or remote logging.
  */
  void Logger::begin()
  {
    /*
      Start the USB serial interface.

      115200 baud is effectively ignored on native USB devices such as the
      nRF52, but keeping the value maintains compatibility with platforms that
      still use a traditional UART.
    */
    Serial.begin(115200);

    /*
      Wait briefly for a host to open the serial connection.

      During development this ensures that early boot messages are not lost.
      The timeout prevents a headless deployment from hanging forever if no
      computer is attached.
    */
    unsigned long start = millis();

    while (!Serial && millis() - start < 3000)
    {
      delay(10);
    }
  }

  /*
    Write text without appending a newline.

    Used when building a log line from multiple pieces.
  */
  void Logger::print(const char *text)
  {
    Serial.print(text);
  }

  /*
    Write text followed by a newline.

    This is the standard helper for complete log messages.
  */
  void Logger::println(const char *text)
  {
    Serial.println(text);
  }

  /*
    Write an unsigned integer without appending a newline.

    Avoids requiring callers to manually convert values to strings.
  */
  void Logger::print(unsigned long value)
  {
    Serial.print(value);
  }

  /*
    Write an unsigned integer followed by a newline.

    Primarily used for counters, timestamps, and other diagnostic values.
  */
  void Logger::println(unsigned long value)
  {
    Serial.println(value);
  }

  /*
    Write an informational message.

    Prefixing the message makes log output easier to scan and prepares the
    logging system for additional severity levels in the future.
  */
  void Logger::info(const char *text)
  {
    Serial.print("[INFO] ");
    Serial.println(text);
  }

  /*
    Write a warning message.

    Warnings indicate unexpected or degraded conditions that are not severe
    enough to stop normal operation.
  */
  void Logger::warn(const char *text)
  {
    Serial.print("[WARN] ");
    Serial.println(text);
  }
}