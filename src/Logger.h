#pragma once

#include <Arduino.h>

namespace FW
{
    class Logger
    {
    public:
        static void begin();

        static void print(const char *text);
        static void println(const char *text);

        static void print(unsigned long value);
        static void println(unsigned long value);

        static void info(const char *text);
        static void warning(const char *text);
        static void error(const char *text);
        static void debug(const char *text);
    };
}
