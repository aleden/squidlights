#pragma once
#include <vector>
#include <string>

namespace squidlights {

enum DEVICE_TYPE {
  DEVICE_TYPE_KINET,
  DEVICE_TYPE_KINET_SPDS480CA
};

struct device_t {
  std::string nm;
  unsigned int ola_univ;
  unsigned int ola_port;
  DEVICE_TYPE ty;
  struct {
    std::string ip;
  } kinet;
  struct {
    std::string ip;
    uint8_t port;
  } kinet_spds480ca;
};

void init_devices();
const std::vector<device_t> &devices();
device_t *device_by_name(const std::string &);
}
