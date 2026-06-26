#pragma once

namespace FW
{
  class Heartbeat
  {
  public:
    static void update();

  private:
    static unsigned long lastBeat;
    static unsigned long beatCount;
  };
}
