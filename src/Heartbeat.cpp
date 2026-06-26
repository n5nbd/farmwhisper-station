#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Heartbeat.h"

namespace FW
{
  unsigned long Heartbeat::lastBeat = 0;
  unsigned long Heartbeat::beatCount = 0;

  void Heartbeat::update()
  {
    if (millis() - lastBeat >= 5000)
    {
      lastBeat = millis();

      Serial.print("[");
      Serial.print(++beatCount);
      Serial.print("] Uptime: ");
      Serial.print(millis() / 1000);
      Serial.println(" seconds");
    }
  }
}
