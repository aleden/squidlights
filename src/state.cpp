#include "state.h"

namespace squidlights {

std::mutex state_mutex;

void init_state() {
}

int light_active_changer(unsigned light_idx) {
  return -1;
}

void set_light_active_changer(unsigned changer_idx, unsigned light_idx) {
}

}
