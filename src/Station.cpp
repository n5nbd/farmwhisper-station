#include "Station.h"

namespace FW
{
  /*
    Cached radio subsystem status.

    Radio support is not active yet, so it starts as Unknown until the radio
    module is brought online and can report its own health.
  */
  Status Station::m_radio = Status::Unknown;

  /*
    Cached display subsystem status.

    This gives the rest of the firmware one shared place to ask how the local
    screen is doing.
  */
  Status Station::m_display = Status::Unknown;

  /*
    Cached sensor subsystem status.

    Sensors are not active yet, so this remains Unknown until real sensor
    modules are added.
  */
  Status Station::m_sensors = Status::Unknown;

  /*
    Initialize station-level subsystem state.
  */
  void Station::begin()
  {
    m_display = Status::OK;
    m_radio = Status::Unknown;
    m_sensors = Status::Unknown;
  }

  /*
    Refresh station-level state.

    Empty for now, but this gives us the right place to aggregate subsystem
    health later without changing the main firmware loop.
  */
  void Station::update()
  {
  }

  /*
    Return current radio subsystem status.
  */
  Status Station::radio()
  {
    return m_radio;
  }

  /*
    Return current display subsystem status.
  */
  Status Station::display()
  {
    return m_display;
  }

  /*
    Return current sensor subsystem status.
  */
  Status Station::sensors()
  {
    return m_sensors;
  }

  /*
    Convert internal status values into short text for display and logs.
  */
  const char* Station::statusText(Status status)
  {
    switch (status)
    {
      case Status::OK:
        return "OK";

      case Status::Warning:
        return "WARN";

      case Status::Error:
        return "ERROR";

      case Status::Unknown:
      default:
        return "--";
    }
  }
}