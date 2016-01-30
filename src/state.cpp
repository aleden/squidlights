#include "state.h"
#include "changer.h"
#include <vector>

using namespace std;

namespace squidlights {

mutex state_mutex;
vector<int> active_changers;

void init_state() {
  active_changers.resize(changers().size());
}

int light_active_changer(unsigned light_idx) {
  return -1;
}

void set_light_active_changer(unsigned changer_idx, unsigned light_idx) {
}

}
