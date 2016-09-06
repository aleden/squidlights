#include "changer.h"
#include "common.h"
#include <unordered_map>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
using namespace std;

namespace squidlights {

static std::vector<changer_t> cl;
static unordered_map<string, CHANGER_ARG_TYPE> chang_arg_ty_map;

static void init_changer_arg_type_map();
static void print_changers();

void init_changers() {
  init_changer_arg_type_map();

  fs::path l_dir(appdir() / "changers");
  fs::directory_iterator end_iter;

  // determine number of changers
  unsigned n = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()) || it->path().extension() != ".json")
      continue;

    ++n;
  }

  cl.resize(n);

  unsigned i = 0;
  for (fs::directory_iterator it(l_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()) || it->path().extension() != ".json")
      continue;

    changer_t &chg = cl[i];
    chg.nm = it->path().stem().string();

    pt::ptree pt;
    pt::read_json(it->path().string(), pt);

    chg.desc = pt.get<string>("desc");

    for (const pt::ptree::value_type &v : pt) {
      if (v.first == "desc")
        continue;

      changer_arg_t arg;
      arg.nm = v.first;
      arg.desc = v.second.get<string>("desc");

      auto ty_it = chang_arg_ty_map.find(v.second.get<string>("type"));
      if (ty_it == chang_arg_ty_map.end()) {
        cerr << "error: changer " << it->path() << " argument " << v.first
             << " has invalid type";
        abort();
      }

      arg.ty = (*ty_it).second;
      switch (arg.ty) {
      case CHANGER_ARG_PRECISE_INT:
      case CHANGER_ARG_INT:
        arg._int.beg = v.second.get<int>("beg");
        arg._int.end = v.second.get<int>("end");
        arg._int.x = (arg._int.beg + arg._int.end) / 2;
        break;
      case CHANGER_ARG_COLOR:
        arg._color.r = 0;
        arg._color.g = 0;
        arg._color.b = 0;
        break;
      case CHANGER_ARG_TEXT:
        break;
      case CHANGER_ARG_FILE:
        break;
      }

      chg.args.push_back(arg);
    }

    ++i;
  }

  print_changers();
}

void init_changer_arg_type_map() {
  chang_arg_ty_map["color"] = CHANGER_ARG_COLOR;
  chang_arg_ty_map["int"] = CHANGER_ARG_INT;
  chang_arg_ty_map["precise_int"] = CHANGER_ARG_PRECISE_INT;
  chang_arg_ty_map["text"] = CHANGER_ARG_TEXT;
  chang_arg_ty_map["file"] = CHANGER_ARG_FILE;
}

const char *chang_arg_ty_string_map[] = {"color", "int", "precise_int", "text", "file"};

void print_changers() {
  for (const changer_t &chg : cl) {
    cout << "changer " << chg.nm << endl;
    cout << "  description: " << chg.desc << endl;
    if (!chg.args.empty()) {
      cout << "  args: " << endl;
      for (const changer_arg_t &a : chg.args)
        cout << "    " << a.nm << " (" << chang_arg_ty_string_map[a.ty]
             << ") : " << a.desc << endl;
    }
  }
}

std::vector<changer_t> &changers() { return cl; }
}
