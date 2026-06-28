#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include "Boot.h"
#include "Logger.h"
#include "Heartbeat.h"
#include "BuildInfo.h"
#include "Display.h"
#include "Station.h"

namespace FW
{
    void Boot::begin()
    {
      Logger::begin();
      Station::begin();
      Display::begin();

      printBanner();
    } 

    void Boot::loop()
    {
      Station::update();
      Display::update();
      Heartbeat::update();
    }

  void Boot::printBanner()
  {
    Logger::println("");
    Logger::println("========================================");
    Logger::println("      FarmWhisper");
    Logger::println("      fw-station");
    Logger::print("      Version ");
    Logger::println(FW_VERSION);
    Logger::println("");
    Logger::print("Node   : ");
    Logger::println(FW_NAME);
    Logger::print("Board  : ");
    Logger::println(FW_BOARD);
    Logger::print("Role   : ");
    Logger::println(FW_ROLE);
    Logger::println("Status : READY");
    Logger::println("");
    Logger::println("Cocka doodle doooo!");
    Logger::println("========================================");
    Logger::println("");
  }
}
