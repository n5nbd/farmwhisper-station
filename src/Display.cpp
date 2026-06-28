#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Display.h"
#include "BootScreen.h"
#include "BuildInfo.h"

namespace FW
{
  namespace
  {
    constexpr uint8_t OLED_ADDR = 0x3C;
    constexpr int8_t OLED_RESET = -1;
    constexpr unsigned long SPLASH_MS = 1500;
    constexpr unsigned long CHECKS_MS = 3500;

    Adafruit_SH1106G oled(128, 64, &Wire, OLED_RESET);

    bool ready = false;
    bool displayOk = false;
    unsigned long startedAt = 0;

    enum class ScreenState
    {
      Splash,
      BootChecks,
      Ready
    };

    ScreenState state = ScreenState::Splash;
  }

  void Display::begin()
  {
    startedAt = millis();

    Wire.begin();
    Wire.setClock(100000);

    displayOk = oled.begin(OLED_ADDR, true);
    if (!displayOk)
    {
      return;
    }

    showSplash();
  }

  void Display::update()
  {
    if (!displayOk)
    {
      return;
    }

    const unsigned long elapsed = millis() - startedAt;

    if (state == ScreenState::Splash && elapsed >= SPLASH_MS)
    {
      state = ScreenState::BootChecks;
      showBootChecks();
    }
    else if (state == ScreenState::BootChecks && elapsed >= CHECKS_MS)
    {
      state = ScreenState::Ready;
      ready = true;
      showReady();
    }
  }

  bool Display::isReady()
  {
    return ready;
  }

  void Display::showSplash()
  {
    oled.clearDisplay();
    oled.drawBitmap(0, 0, BOOT_SCREEN_128X64, 128, 64, SH110X_WHITE);
    oled.display();
  }

  void Display::showBootChecks()
  {
    oled.clearDisplay();
    oled.setTextColor(SH110X_WHITE);
    oled.setTextSize(1);

    oled.setCursor(0, 0);
    oled.println("FarmWhisper");
    oled.println("Coop Station");
    oled.println();
    oled.println("Boot checks");
    oled.println("OLED     OK");
    oled.println("I2C      OK");
    oled.println("Radio    WAIT");
    oled.println("Sensors  WAIT");

    oled.display();
  }

  void Display::showReady()
  {
    oled.clearDisplay();
    oled.setTextColor(SH110X_WHITE);
    oled.setTextSize(1);

    oled.setCursor(0, 0);
    oled.println("FarmWhisper");
    oled.println("Coop Station");
    oled.println();
    oled.print("Version ");
    oled.println(FW_VERSION);
    oled.println();
    oled.println("Status: READY");
    oled.println("Radio : idle");
    oled.println("Sensors: 0");

    oled.display();
  }
}
