#include "Station.h"

namespace FW
{
    Status Station::m_radio   = Status::Unknown;
    Status Station::m_display = Status::Unknown;
    Status Station::m_sensors = Status::Unknown;

    void Station::begin()
    {
        m_display = Status::OK;
        m_radio   = Status::Unknown;
        m_sensors = Status::Unknown;
    }

    void Station::update()
    {
    }

    Status Station::radio()
    {
        return m_radio;
    }

    Status Station::display()
    {
        return m_display;
    }

    Status Station::sensors()
    {
        return m_sensors;
    }
}