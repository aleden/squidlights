#pragma once
#include <list>
#include <vector>
#include <string>

namespace squidlights {

struct device_t;

struct dmx_channel_range {
  device_t *dev;
  unsigned beg;
  unsigned end;
};

struct light_t {
  std::string nm, desc;
  std::list<dmx_channel_range> rngs;
};

void init_lights();
const std::vector<light_t> &lights();
}
