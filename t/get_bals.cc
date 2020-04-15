#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <bittrex.hh>
#include <balance.hh>
using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;


int xmain(int argc, char**argv) {
  balance_l bals = bittrex::load_balances();

  vector<string> nm = { "num", "sym", "bal", "usd", "pct" };
  vector<int> cw;
  {
    cw.push_back(5);
    cw.push_back(sym_t().get_width());
    cw.push_back(money_t().get_width());
    cw.push_back(money_t().get_width());
    cw.push_back(pct_t(0).get_width());
  };
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
    << right
    << setw(cw[0]) << nm[0] 
    << setw(cw[1]) << nm[1] 
    << setw(cw[2]) << nm[2] 
    << setw(cw[3]) << nm[3] 
    << setw(cw[4]) << nm[4] 
    << endl;
  int num=0;
  for( auto bal : bals ) {
    if(!bal.bal)
      continue;
    cout
      << right
      << setw(cw[0]) << ++num
      << setw(cw[1]) << bal.sym 
      << setw(cw[1]) << bal.bal 
      << setw(cw[2]) << bal.usd
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

