#pragma once
#include <vector>
#include <string>

namespace squidlights {

enum CHANGER_ARG_TYPE {
  CHANGER_ARG_COLOR,
  CHANGER_ARG_BOUNDED_INT
};

struct changer_arg_t {
  std::string nm, desc;
  CHANGER_ARG_TYPE ty;
  union {
    struct {
      int beg, end;
    } int_rng;
    struct {
      double beg, end;
    } dbl_rng;
  };
};

struct changer_t {
  std::string nm, desc;
  std::vector<changer_arg_t> args;
};

void init_changers();
const std::vector<changer_t>& changers();

}
