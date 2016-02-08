#pragma once
#include <vector>
#include <string>

namespace squidlights {

enum CHANGER_ARG_TYPE {
  CHANGER_ARG_COLOR,
  CHANGER_ARG_INT,
  CHANGER_ARG_PRECISE_INT,
  CHANGER_ARG_FILE
};

struct changer_arg_t {
  std::string nm, desc;
  CHANGER_ARG_TYPE ty;

  struct {
    int x, beg, end;
  } _int;

  struct {
    uint8_t r, g, b;
  } _color;

  std::string _file;
};

struct changer_t {
  std::string nm, desc;
  std::vector<changer_arg_t> args;
};

void init_changers();
std::vector<changer_t> &changers();
}
