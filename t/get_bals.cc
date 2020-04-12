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
  const market_l &markets=market_l::get_markets();
  balance_l bals = balance_l::load_balances();
  int i = 0;
  vector<int> cw = { 5, 6, 15, 15, 15, 15, 15 };
  vector<const char *> nm = { "num", "sym", "bal", "usd", "pend", "pct" };
  money_t usd_sum=0;
  money_t btc_sum=0;
  {
    balance_l tmp(bals);
    bals.clear();
    for( auto bal : tmp ) {
      if( bal.usd < 0.01 )
        continue;
      usd_sum += bal.usd;
      btc_sum += bal.btc;
      bals.push_back(bal);
    };
  };
  money_t usd_avg=usd_sum/bals.size();
  money_t btc_avg=btc_sum/bals.size();
#define ROW(lab,val1,val2) lab << setw(16) << val1 << setw(16) << val2 << nl;

  cout  <<  ROW("....","USD","BTC");
  cout  <<  ROW("tot:",usd_sum,btc_sum);
  cout  <<  ROW("avg:",usd_avg,btc_avg);
#undef ROW
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
    << endl;
  int num=0;
  for( auto bal : bals ) {
    if(!bal.bal)
      continue;
    cout
      << left
      << setw(cw[0]) << ++num
      << setw(cw[1]) << bal.sym 
      << right << fixed << setprecision(8)
      << setw(cw[1]) << bal.ava 
      << setw(cw[2]) << bal.usd
      << setw(cw[4]) << bal.pend
      << setw(cw[5]) << pct_t(bal.usd,usd_sum)
      << endl;
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

