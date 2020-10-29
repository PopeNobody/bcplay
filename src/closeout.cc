#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <bittrex.hh>
#include <balance.hh>
#include <util.hh>
#include <order.hh>
#include <dbg.hh>
using namespace coin;
using namespace util;
using namespace std;
using namespace fmt;

money_t min_txn_btc;
money_t min_bal_btc;
//  string simple_xact ( const market_t &market, bool buy, money_t qty, money_t rate, bool ioc);
void adjust(const balance_t &bal)
{
  cout << "adjust: " << bal << endl;
  if( bal.btc < min_txn_btc ) {
    cout << "we need to buy more " << bal.sym << " before we can sell it." << endl;
    cout << "  btc val: " << bal.btc << endl;
    cout << "  min txn: " << min_txn_btc << endl;
    auto mlist = market_t::get(bal.sym,"BTC");
    for( auto mkt : mlist ) {
      cout << mkt << endl;
    };

  } else if ( bal.btc < min_bal_btc ) {
    cout << "we have enough " << bal.sym << " to sell, but not enough to keep" << endl;
  } else {
    cout << "how did we get here?" << endl;
  };
};
int xmain(int argc, char**argv)
{
  for( int i=0;i<argc;i++){
    if(argv[i]==string("-y"))
      bittrex::fake_buys=false;
  };
  min_txn_btc=0.0005;
  min_bal_btc=min_txn_btc*2;
  auto bals=balance_t::get_balances();
  sort(bals.begin(),bals.end(),std::less<balance_t>());
  reverse(bals.begin(),bals.end());
  for( auto &bal : bals )
  {
//    cout << "checking: " << bal.sym << " usd: " << bal.usd << "btc: " << bal.btc << endl;
    if( bal.sym == "BTC" ) {
    } else if( bal.btc > min_bal_btc ) {
//      cout << " -- got enough to hold" << endl;
    } else if ( !bal.btc ) {
//      cout << " -- got none" << endl;
    } else {
      cout << " -- adjusting" << endl;
      cout << bal << endl;
      adjust(bal);
      return 0;
    };
  };
  return 0;
};
int main(int argc, char**argv) {
  try {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    xclose(1);
    mkdir("log",0755);
    int log=open_log("log/closeout.log");
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/closeout.log:1:started\n";
    vector<string> args(argv,argv+argc);
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

