#ifndef bittrex_json_hh
#define bittrex_json_hh bittrex_json_hh

#include <coinfwd.hh>
#include <json.hh>
#include <bittrex.hh>

namespace bittrex {
  void save_json(const string &fname, const json &json);
  const json load_json(const string &url, const string &save_to);
};

#endif
