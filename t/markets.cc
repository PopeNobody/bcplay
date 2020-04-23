#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <util.hh>
#include <balance.hh>
using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;
using namespace util;


int xmain(int argc, char**argv) {
  using coin::market_l;
  cerr << "Loading Markets" << endl;
  const auto &bals=balance_l::get_balances();
  vector<string> syms;
  for( auto &b: bals )
  {
    syms.push_back(b.sym);
  };
  const auto &markets=market_l::get_markets();
  for( auto &m: markets )
  {
    if(m.f_coin == "BTC" && contains(syms,m.t_coin))
      cout << m << endl;
  };
  return 0;
};
int main(int argc, char**argv) {
  try {
    return xmain(argc,argv);
  } catch ( exception &e ) {
    cout << endl << endl << e.what() << endl << endl;
  } catch ( ... ) {
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "wtf?" << endl;
  };
  return -1;
};

