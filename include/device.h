#pragma once
#include <vector>
#include <string>

namespace squidlights {

enum DEVICE_TYPE {
  DEVICE_TYPE_KINET,
  DEVICE_TYPE_SERIAL_USB
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
    int ttyUSB_idx;
  } serial_usb;
};

void init_devices();
const std::vector<device_t> &devices();
device_t *device_by_name(const std::string &);
}
