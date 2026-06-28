#pragma once

namespace FW
{
    enum class Status
    {
        Unknown,
        OK,
        Warning,
        Error
    };

    class Station
    {
    public:
        static void begin();
        static void update();

        static Status radio();
        static Status display();
        static Status sensors();

    private:
        static Status m_radio;
        static Status m_display;
        static Status m_sensors;
    };
}