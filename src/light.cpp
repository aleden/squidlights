#include "light.h"
#include "common.h"

namespace fs = boost::filesystem;

namespace squidlights {

static std::vector<light_t> ll;

void init_lights() {
}

const std::vector<light_t>& lights() {
  return ll;
}

}
