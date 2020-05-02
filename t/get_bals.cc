#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>

#include <balance.hh>
using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;


int xmain(int argc, char**argv) {
  using coin::market_l;
  cerr << "Loading Markets" << endl;
  const market_l &markets=market_t::get_markets();
  cerr << "Loading Balances" << endl;
  balance_l bals = balance_l::load_balances();
  int i = 0;
  vector<int> cw =          { 25,     25,    25,    25,    25,    25,     25,        25 };
  vector<const char *> nm = { "num", "sym", "bal", "usd", "btc", "pct", "perBTC", "%ava" };
  money_t btc_sum=0;
  money_t usd_sum=0;
  {
    balance_l tmp(bals);
    bals.clear();
    for( auto bal : tmp ) {
      if( bal.usd < 0.01 )
        continue;
      btc_sum += bal.btc;
      usd_sum += bal.usd;
      bals.push_back(bal);
    };
  };
  money_t btc_avg=btc_sum/bals.size();
  money_t usd_avg=usd_sum/bals.size();
#define ROW(lab,val1,val2) lab << setw(25) << val1 << setw(25) << val2 << nl

    cout
      << setfill('-');
  cout  <<  ROW("....","BTC","USD");
  cout  <<  ROW("tot:",btc_sum,usd_sum);
  cout  <<  ROW("avg:",btc_avg,usd_avg);
  cout  <<  "-----------------------------" << endl;
  cout
    << left
    << setw(cw[0]) << nm[0] << " "
    << setw(cw[1]) << nm[1] << " "
    << right
    << setw(cw[2]) << nm[2] << " "
    << setw(cw[3]) << nm[3] << " "
    << setw(cw[4]) << nm[4] << " "
    << setw(cw[5]) << nm[5] << " "
    << setw(cw[6]) << nm[6] << " "
    << endl;
  int num=0;
  for( auto bal : bals ) {
    if(!bal.bal)
      continue;

    cout
      << left
      << setw(cw[0]) << ++num << "|"
      << setw(cw[1]) << bal.sym  << "|"
      << right << fixed << setprecision(8)
      << setw(cw[1]) << bal.ava  << "|"
      << setw(cw[2]) << bal.usd << "|"
      << setw(cw[4]) << bal.btc << "|"
      << setw(cw[5]) << pct_t(bal.btc,btc_sum) << "|"
      << setw(cw[6]) << pct_t(bal.ava,bal.bal) << "|"
      << endl;
#undef setw
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

