#pragma once
#include <vector>

namespace squidlights {

struct light_t {
};

void init_lights();
const std::vector<light_t>& lights();

}
