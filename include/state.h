#pragma once
#include <mutex>

namespace squidlights {

extern std::mutex state_mutex;

void init_state();

// an integer for every light specifies what changer is active. -1 denotes that
// no changers are active.
int light_active_changer(unsigned light_idx);

// set what changer is active for a light
void set_light_active_changer(unsigned changer_idx, unsigned light_idx);

}
