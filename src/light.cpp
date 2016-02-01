#include "light.h"
#include "common.h"
#include "device.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace std;

namespace squidlights {

static std::vector<light_t> ll;
static void print_lights();

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

    light_t &l = ll[i];
    l.nm = it->path().stem().string();

    pt::ptree pt;
    pt::read_json(it->path().string(), pt);

    l.desc = pt.get<string>("desc");

    for (const pt::ptree::value_type &v : pt) {
      if (v.first == "desc")
        continue;

      dmx_channel_range rng;
      rng.dev = device_by_name(v.first);
      rng.beg = v.second.get<unsigned>("beg");
      rng.end = v.second.get<unsigned>("end");

      l.rngs.push_back(rng);
    }

    ++i;
  }

  print_lights();
}

void print_lights() {
  for (const light_t &l : ll) {
    cout << "light " << l.nm << endl;
    cout << "  description: " << l.desc << endl;
    for (const dmx_channel_range &rng : l.rngs)
      cout << "  " << rng.dev->nm << " [" << rng.beg << ", " << rng.end << ")"
           << endl;
  }
}

const std::vector<light_t> &lights() { return ll; }
}
