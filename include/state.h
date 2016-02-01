#pragma once
#include <mutex>

namespace squidlights {

extern std::mutex state_mutex;

void init_state();

// an integer for every light specifies what changer is active. -1 denotes that
// no changers are active.
int light_changer(unsigned light_idx);

// set what changer is active for a light
void set_changer_for_light(unsigned changer_idx, unsigned light_idx);

// run light changer
void run_light_changer(unsigned light_idx);

}
