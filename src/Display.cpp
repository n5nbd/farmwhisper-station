#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeSansBold9pt7b.h>

#include "Display.h"
#include "BuildInfo.h"
#include "Station.h"
#include "assets/RoosterLogo.h"


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

    // Rooster logo
    oled.drawBitmap(
        0, 12,
        roosterLogo,
        ROOSTER_LOGO_WIDTH,
        ROOSTER_LOGO_HEIGHT,
        SH110X_WHITE
    );

    oled.setTextColor(SH110X_WHITE);

    // Product name
    oled.setFont(&FreeSansBold9pt7b);
    oled.setTextSize(1);

    oled.setCursor(38, 18);
    oled.println("FARM");

    oled.setCursor(38, 36);
    oled.println("WHISPER");

    // Product type
    oled.setFont();
    oled.setTextSize(1);

    oled.setCursor(40, 52);
    oled.println("COOP STATION");

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

    oled.print("OLED     ");
    oled.println(Station::statusText(Station::display()));

    oled.println("I2C      OK");

    oled.print("Radio    ");
    oled.println(Station::statusText(Station::radio()));

    oled.print("Sensors  ");
    oled.println(Station::statusText(Station::sensors()));

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

    oled.print("Radio : ");
    oled.println(Station::statusText(Station::radio()));

    oled.print("Sensors: ");
    oled.println(Station::statusText(Station::sensors()));

    oled.display();
  }
}