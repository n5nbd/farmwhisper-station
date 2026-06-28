#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Logger.h"

namespace FW
{
    void Logger::begin()
    {
        Serial.begin(115200);
        delay(100);
    }

    void Logger::print(const char *text)
    {
        Serial.print(text);
    }

    void Logger::println(const char *text)
    {
        Serial.println(text);
    }

    void Logger::print(unsigned long value)
    {
        Serial.print(value);
    }

    void Logger::println(unsigned long value)
    {
        Serial.println(value);
    }

    void Logger::info(const char *text)
    {
        Serial.print("[INFO] ");
        Serial.println(text);
    }

    void Logger::warning(const char *text)
    {
        Serial.print("[WARN] ");
        Serial.println(text);
    }

    void Logger::error(const char *text)
    {
        Serial.print("[ERROR] ");
        Serial.println(text);
    }

    void Logger::debug(const char *text)
    {
        Serial.print("[DEBUG] ");
        Serial.println(text);
    }
}
