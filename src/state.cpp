#include "common.h"
#include "state.h"
#include "light.h"
#include "changer.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <boost/format.hpp>
#include <spawn.h>
#include <signal.h>

using namespace std;
namespace fs = boost::filesystem;

namespace squidlights {

mutex state_mutex;

class child_process {
  pid_t pid;
  bool dead;

public:
  child_process(const string &exe_fp, const list<string> &argv) : dead(false) {
    vector<unique_ptr<char[]>> argv_cmem;
    argv_cmem.resize(argv.size());

    unsigned i = 0;
    for (const string &arg : argv) {
      argv_cmem[i].reset(new char[arg.size() + 1]);
      strcpy(argv_cmem[i].get(), arg.c_str());
    }

    vector<char *> argv_flattened;
    argv_flattened.resize(argv_cmem.size() + 1);
    for (i = 0; i < argv_cmem.size(); ++i)
      argv_flattened[i] = argv_cmem[i].get();
    argv_flattened[argv_flattened.size()] = NULL;

    char *envp[] = {NULL};

    int status =
        posix_spawn(&pid, exe_fp.c_str(), NULL, NULL, &argv_flattened[0], envp);
    if (status != 0) {
      cerr << "warning: failed to launch " << exe_fp << endl;
      dead = true;
    }
  }

  ~child_process() {
    if (!dead)
      kill(pid, SIGKILL);
  }
};

static vector<unique_ptr<child_process>> light_chgr_process_map;
static vector<int> light_chgr_map;

void init_state() {
  light_chgr_map.resize(lights().size());
  light_chgr_process_map.resize(lights().size());

  fill(light_chgr_map.begin(), light_chgr_map.end(), -1);
}

int light_changer(unsigned light_idx) {
  return light_chgr_map[light_idx];
}

void set_changer_for_light(unsigned changer_idx, unsigned light_idx) {
  light_chgr_map[light_idx] = changer_idx;

  const changer_t& chgr = changers()[changer_idx];
  string chgr_py = chgr.nm + ".py";

  fs::path python_path("/usr/bin/python");
  fs::path chgr_path(appdir() / "changers" / chgr_py);

  if (!fs::exists(python_path)) {
    cerr << "warning: could not find " << python_path << endl;
    return;
  }

  if (!fs::exists(chgr_path)) {
    cerr << "warning: could not find " << chgr_path << endl;
    return;
  }

  string argstr;

  string argv0 = "-c";
  string argv1 =
      (boost::format(
           "import sys; sys.path.insert(0, '%s'); import %s; %s.squid(%s)") %
       (appdir() / "changers").string() % chgr.nm % chgr.nm % argstr)
          .str();

  list<string> argv = {argv0, argv1};

  //
  // launch the changer
  //
  light_chgr_process_map[light_idx].reset(
      new child_process(python_path.string(), argv));
}

// restart light changer (e.g. due to change in arguments)
void restart_light_changer(unsigned light_idx) {
}

}
