#pragma once
#include <list>
#include <vector>
#include <string>

namespace squidlights {

struct changer_t {
  std::string nm, desc;
};

void init_changers();
const std::vector<changer_t>& changers();

}
