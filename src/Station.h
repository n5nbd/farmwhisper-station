#pragma once

namespace FW
{
  /*
    Shared health state used by station subsystems.

    Keeping this small and generic lets Display, Radio, Sensors, and future
    modules report status in the same language.
  */
  enum class Status
  {
    Unknown,
    OK,
    Warning,
    Error
  };

  /*
    Station tracks the overall state of this FarmWhisper device.

    It provides one place for subsystem status so display, logging, and future
    radio reports can all agree on what the station thinks is happening.
  */
  class Station
  {
  public:
    /*
      Initialize station-level state.
    */
    static void begin();

    /*
      Refresh station-level state.

      This should stay non-blocking so it can be called every loop.
    */
    static void update();

    /*
      Current radio subsystem status.
    */
    static Status radio();

    /*
      Current display subsystem status.
    */
    static Status display();

    /*
      Current sensor subsystem status.
    */
    static Status sensors();

    /*
      Convert a status enum into display/log-friendly text.
    */
    static const char* statusText(Status status);

  private:
    /*
      Cached subsystem health values.

      Stored centrally so other modules can read status without owning or
      directly probing each subsystem.
    */
    static Status m_radio;
    static Status m_display;
    static Status m_sensors;
  };
}