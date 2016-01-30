#pragma once
#include <vector>
#include <string>

namespace squidlights {

struct device_t {
  std::string nm;
  unsigned int ola_univ;
  unsigned int ola_port;
};

void init_devices();
const std::vector<device_t>& devices();

}
