#include "common.h"
#include <unistd.h>

using namespace std;
namespace fs = boost::filesystem;

namespace squidlights {

fs::path rootdir() {
  char buf[2048] = {0};
  ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
  string str(buf, size);
  return fs::canonical(fs::path(str).parent_path().parent_path());
}

}
