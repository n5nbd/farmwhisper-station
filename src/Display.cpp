/*
    ---------------------------------------------------------------------------
    FarmWhisper Display Module
    ---------------------------------------------------------------------------

    Purpose

    The Display module owns the local OLED user interface for fw-station.

    The display is intentionally simple at this stage. It provides immediate
    visual feedback during boot, shows basic subsystem status, and confirms when
    the station reaches the ready state.

    Current Responsibilities

      • Initialize the I2C bus used by the OLED.
      • Initialize the SH1106-compatible display.
      • Show the FarmWhisper splash screen.
      • Show boot check status.
      • Show the ready/status screen.
      • Track whether the display has completed its startup sequence.

    Design Notes

    Display owns rendering only.

    It reads station status from the Station module, but it does not decide
    whether the station, radio, sensors, or other subsystems are healthy. That
    separation keeps display code from becoming system logic.

    The boot screens are timed using millis() instead of delay() so the main
    firmware loop can continue running while the display advances through its
    startup sequence.

    ---------------------------------------------------------------------------
*/

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

  // Standard I2C address used by the SH1106 OLED module currently fitted to
  // the RAK5005-based station hardware.
  constexpr uint8_t OLED_ADDR = 0x3C;

  // The display reset line is not controlled by a dedicated GPIO in this
  // wiring configuration, so the Adafruit driver is told there is no reset pin.
  constexpr int8_t OLED_RESET = -1;

  // Time to keep the branded splash screen visible before showing boot checks.
  constexpr unsigned long SPLASH_MS = 1500;

  // Time from display startup before switching from boot checks to ready.
  constexpr unsigned long CHECKS_MS = 3500;

  // Single display driver instance for the station OLED.
  Adafruit_SH1106G oled(128, 64, &Wire, OLED_RESET);

  // True once the display startup sequence has reached the ready screen.
  bool ready = false;

  // Tracks whether the OLED initialized successfully. Display methods return
  // quietly if hardware is missing or initialization fails.
  bool displayOk = false;

  // Timestamp captured at display startup. Used to advance boot screens without
  // blocking the firmware with delay().
  unsigned long startedAt = 0;

  // Small internal state machine for the boot-time display sequence.
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

  // Capture startup time before display work begins so the screen sequence is
  // based on elapsed runtime rather than blocking delays.
  startedAt = millis();

  // Bring up I2C at a conservative clock rate for reliable OLED operation.
  Wire.begin();

  Wire.setClock(100000);

  // Initialize the OLED. If this fails, the rest of the firmware should still
  // run; we simply suppress display updates.
  displayOk = oled.begin(OLED_ADDR, true);

  if (!displayOk)

  {

  return;

  }

  // Show the branded splash screen as soon as the OLED is available.
  showSplash();

  }

  void Display::update()

  {

  // If the OLED is not available, display work becomes a no-op.
  if (!displayOk)

  {

  return;

  }

  const unsigned long elapsed = millis() - startedAt;

  // After the splash interval, show subsystem boot status.
  if (state == ScreenState::Splash && elapsed >= SPLASH_MS)

  {

  state = ScreenState::BootChecks;

  showBootChecks();

  }

  // After boot checks have had time to be seen, switch to the steady ready
  // screen and mark the display sequence complete.
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

  // Draw the FarmWhisper rooster artwork on the left side of the splash screen.
  oled.drawBitmap(

  0, 12,

  roosterLogo,

  ROOSTER_LOGO_WIDTH,

  ROOSTER_LOGO_HEIGHT,

  SH110X_WHITE

  );

  oled.setTextColor(SH110X_WHITE);

  // Use the larger bold font for the product name so the splash screen is
  // readable at a glance on the small OLED.
  oled.setFont(&FreeSansBold9pt7b);

  oled.setTextSize(1);

  oled.setCursor(38, 18);

  oled.println("FARM");

  oled.setCursor(38, 36);

  oled.println("WHISPER");

  // Return to the default font for the smaller device role label.
  oled.setFont();

  oled.setTextSize(1);

  oled.setCursor(40, 52);

  oled.println("COOP STATION");

  // Push the completed splash screen buffer to the physical display.
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

  // Show display status using the same station status text used elsewhere.
  oled.print("OLED ");

  oled.println(Station::statusText(Station::display()));

  // I2C is considered OK here because the OLED was successfully initialized
  // over the I2C bus before this screen could be rendered.
  oled.println("I2C OK");

  oled.print("Radio ");

  oled.println(Station::statusText(Station::radio()));

  oled.print("Sensors ");

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