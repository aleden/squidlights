#include "light.h"
#include "common.h"
#include "device.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace fs = boost::filesystem;
using boost::property_tree::ptree;

namespace squidlights {

static std::vector<light_t> ll;

void init_lights() {
  fs::path l_dir(appdir() / "lights");
  fs::directory_iterator end_iter;

  // determine number of lights
  unsigned n = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    ++n;
  }

  ll.resize(n);

  unsigned i = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    light_t& l = ll[i];

    ptree pt;
    boost::property_tree::read_json(ss, pt);

    BOOST_FOREACH (boost::property_tree::ptree::value_type &v,
                   pt.get_child("particles.electron")) {
      cout << v.first << endl;
      std::cout << v.second.data() << std::endl;
      // etc
    }
    string nm = it->path().filename().string();

    ++i;
  }
}

const std::vector<light_t>& lights() {
  return ll;
}

}
