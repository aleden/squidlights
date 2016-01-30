#include "changer.h"
#include "common.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace std;

namespace squidlights {

static std::vector<changer_t> cl;

void init_changers() {
  fs::path l_dir(appdir() / "changers");
  fs::directory_iterator end_iter;

  // determine number of changers
  unsigned n = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    ++n;
  }

  cl.resize(n);

  unsigned i = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    changer_t& chg = cl[i];
    chg.nm = it->path().stem().string();

    pt::ptree pt;
    pt::read_json(it->path().string(), pt);

    chg.desc = pt.get<string>("desc");

#if 0
    for (const pt::ptree::value_type &v : pt) {
      if (v.first == "desc")
        continue;
    }
#endif

    ++i;
  }
}

const std::vector<changer_t>& changers() {
  return cl;
}

}
