#include "device.h"
#include "common.h"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <ola/Logging.h>
#include <ola/base/Init.h>
#include <ola/client/ClientWrapper.h>
#include <ola/client/OlaClient.h>
#include <ola/io/SelectServer.h>
#include <olad/PortConstants.h>

using ola::NewSingleCallback;
using ola::client::OlaClient;
using ola::client::OlaClientWrapper;
using ola::client::OlaDevice;
using ola::client::OlaInputPort;
using ola::client::OlaOutputPort;
using ola::client::OlaPlugin;
using ola::client::OlaUniverse;
using ola::client::Result;
using ola::io::SelectServer;

using namespace std;
namespace fs = boost::filesystem;
namespace ba = boost::algorithm;

namespace squidlights {

static std::vector<device_t> dl;

static unordered_map<string, device_t*> ipaddr_to_dev_map;
static unordered_map<string, device_t*> nm_to_dev_map;

static void fill_ip_dev_map();
static void fill_dev_olaunivs();
static void fill_dev_nm_map();

void init_devices() {
  fill_ip_dev_map();
  fill_dev_olaunivs();
  fill_dev_nm_map();
}

static string read_text_file(const string& fp);

void fill_ip_dev_map() {
  fs::path dev_dir(appdir() / "devices");
  fs::directory_iterator end_iter;

  // determine number of color kinetic devices
  unsigned n = 0;
  for (fs::directory_iterator it(dev_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    ++n;
  }

  dl.resize(n);

  unsigned i = 0;
  for (fs::directory_iterator it(dev_dir); it != end_iter; ++it) {
    if (!fs::is_regular_file(it->status()))
      continue;

    string nm = it->path().filename().string();
    string ipaddr = read_text_file(it->path().string());
    ba::trim(ipaddr);

#if 0
    cout << "color kinetics device " << nm << " : " << ipaddr << endl;
#endif

    dl[i].nm = nm;
    dl[i].ola_univ = 0;
    dl[i].ola_port = 0;

    ipaddr_to_dev_map[ipaddr] = &dl[i];

    ++i;
  }
}

string read_text_file(const string &fp) {
  ifstream in(fp, std::ios::in);

  in.seekg(0, ios::end);
  string contents;
  contents.resize(in.tellg());

  in.seekg(0, ios::beg);
  in.read(&contents[0], contents.size());

  return contents;
}

static void fill_dev_olaports(SelectServer *ss, const Result &result,
                              const vector<OlaDevice> &devices);
static void handle_ola_ack(SelectServer *ss, const Result &result);

static unsigned int ola_kinetdev_alias;

void fill_dev_olaunivs() {
  ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
  if (!ola::NetworkInit()) {
    cerr << "ola network initialization failed" << endl;
    exit(1);
  }

  OlaClientWrapper ola_cl;

  if (!ola_cl.Setup()) {
    cerr << "ola setup failed" << endl;
    exit(1);
  }

  //
  // determine what KiNet ports correspond to our color kinetic devices
  //
  SelectServer *ss = ola_cl.GetSelectServer();
  OlaClient *cl = ola_cl.GetClient();
  cl->FetchDeviceInfo(ola::OLA_PLUGIN_KINET,
                      NewSingleCallback(&fill_dev_olaports, ss));
  ss->Run();

  // verify all devices were found in fill_dev_olaports
  for (device_t& d : dl) {
    if (d.ola_univ)
      continue;

    cerr << "device " << d.nm << " unregistered with OLA" << endl;
  }

  //
  // unpatch KiNet ports
  //
  for (device_t& d : dl) {
    cl->Patch(ola_kinetdev_alias,
              d.ola_port,
              ola::client::OUTPUT_PORT,
              ola::client::UNPATCH,
              0,
              NewSingleCallback(&handle_ola_ack, ss));
    ss->Run();
  }

  //
  // patch KiNet ports
  //
  unsigned int ola_univ = 1;
  for (device_t& d : dl) {
    d.ola_univ = ola_univ++;

    cl->Patch(ola_kinetdev_alias,
              d.ola_port,
              ola::client::OUTPUT_PORT,
              ola::client::PATCH,
              d.ola_univ,
              NewSingleCallback(&handle_ola_ack, ss));
    ss->Run();

    // Set universe name
    cl->SetUniverseName(d.ola_univ, d.nm,
                        NewSingleCallback(&handle_ola_ack, ss));
    ss->Run();
  }
}

void handle_ola_ack(SelectServer *ss, const Result &result) {
  if (!result.Success()) {
    cerr << result.Error() << endl;
    abort();
  }
  ss->Terminate();
}

void fill_dev_olaports(SelectServer *ss,
                    const Result &result,
                    const vector <OlaDevice> &devices) {
  if (!result.Success()) {
    cerr << result.Error() << endl;
    abort();
  }

  if (devices.size() != 1) {
    cerr << "error: number of OLA KiNet devices is " << devices.size() << endl;
    abort();
  }

  const OlaDevice &olad = devices.front();
  ola_kinetdev_alias = olad.Alias();

  cout << "Device " << olad.Alias() << ": " << olad.Name() << endl;

  vector<OlaOutputPort> ports = olad.OutputPorts();
  for (auto port_iter = ports.begin(); port_iter != ports.end(); ++port_iter) {
    // the following "hack" is the only way to get the IP address of this OLA
    // KiNet device port. derived from ola-0.9.8/plugins/kinet/KiNetPort.h:50
    string ipaddr =
        port_iter->Description().substr(string("Power Supply: ").size());

    auto devit = ipaddr_to_dev_map.find(ipaddr);
    if (devit == ipaddr_to_dev_map.end())
      continue;

    (*devit).second->ola_univ = 1; // mark KiNet device as having been found
    (*devit).second->ola_port = port_iter->Id();

    cout << "  port " << port_iter->Id() << ", OUT " << port_iter->Description()
         << " (" << (*devit).second->nm << ")" << endl;
  }

  ss->Terminate();
}

const std::vector<device_t>& devices() {
  return dl;
}

void fill_dev_nm_map() {
  for (device_t& d : dl)
    nm_to_dev_map[d.nm] = &d;
}

device_t* device_by_name(const std::string& nm) {
  return nm_to_dev_map[nm];
}

}
