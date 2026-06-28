#pragma once

#include <Arduino.h>

namespace FW
{
  /*
    Logger provides a single, centralized interface for all serial output.

    Keeping serial access in one place makes it easy to:
      • Change baud rates or startup behavior.
      • Enable or disable logging globally.
      • Redirect output to another destination in the future.
      • Add features such as log levels, timestamps, or remote logging
        without changing the rest of the firmware.

    Application code should write messages through Logger rather than
    accessing Serial directly whenever practical.
  */
  class Logger
  {
  public:
    /*
      Initialize the logging system.

      This is typically called once during firmware startup before any
      diagnostic messages are written. It prepares the serial interface
      and performs any platform-specific initialization required for
      logging.
    */
    static void begin();

    /*
      Write text without appending a newline.

      Useful when constructing a line from multiple pieces or when
      displaying values inline.
    */
    static void print(const char *text);

    /*
      Write text followed by a newline.

      This is the most common logging function for complete status or
      diagnostic messages.
    */
    static void println(const char *text);

    /*
      Write an unsigned integer without appending a newline.

      Provided as a convenience so callers do not need to convert
      numeric values into strings before logging.
    */
    static void print(unsigned long value);

    /*
      Write an unsigned integer followed by a newline.

      Useful for standalone numeric status values or counters.
    */
    static void println(unsigned long value);

    /*
      Write an informational message.

      Intended for normal operational messages that help describe
      what the firmware is doing during development or troubleshooting.
    */
    static void info(const char *text);

    /*
      Write a warning message.

      Intended for recoverable problems or unexpected conditions that
      deserve attention but do not prevent the firmware from continuing
      to operate.
    */
    static void warn(const char *text);
  };
}