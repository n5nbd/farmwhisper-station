#include "src/Boot.h"
#include "src/Heartbeat.h"

void setup()
{
  FW::Boot::begin();
}

void loop()
{
  FW::Heartbeat::update();
}
