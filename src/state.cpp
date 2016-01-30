#include "state.h"
#include "changer.h"
#include <vector>
#include <algorithm>

using namespace std;

namespace squidlights {

mutex state_mutex;
vector<int> active_changers;

void init_state() {
  active_changers.resize(changers().size());
  fill(active_changers.begin(), active_changers.end(), -1);
}

int light_active_changer(unsigned light_idx) {
  return active_changers[light_idx];
}

void set_light_active_changer(unsigned changer_idx, unsigned light_idx) {
  active_changers[light_idx] = changer_idx;
}

}
