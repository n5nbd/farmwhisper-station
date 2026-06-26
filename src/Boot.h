#pragma once

namespace FW
{
  class Boot
  {
  public:
    static void begin();
    static void loop();

  private:
    static void printBanner();
  };
}
