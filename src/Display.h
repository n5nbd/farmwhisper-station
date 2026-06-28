#pragma once

namespace FW
{
  class Display
  {
  public:
    static void begin();
    static void update();
    static bool isReady();

  private:
    static void showSplash();
    static void showBootChecks();
    static void showReady();
  };
}
