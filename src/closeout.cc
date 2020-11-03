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

const money_t min_txn_btc = 0.00055;
const money_t min_bal_btc = 4*min_txn_btc;

//  string simple_xact ( const market_t &market, bool buy, money_t qty, money_t rate, bool ioc);
//#define xcheckin(x)
// using bittrex::simple_xact;
//   string bittrex::simple_xact (
//    const market_t &market,
//    bool buy,    // buy or sell
//    money_t qty, // in the symbol to buy or sell
//    money_t rate, // in currency per unit
//    bool ioc
//    )
bool adjust(const balance_t &bal)
{
  xtrace("adjust: " << bal);
  bool shown=false;
  if( bal.btc < min_txn_btc ) {
    shown=true;
    xexpose(bal.btc);
  }
  xexpose(min_txn_btc);
  if( !shown && bal.btc < min_bal_btc ) {
    shown=true;
    xexpose(bal.btc);
  };
  xexpose(min_bal_btc);

  auto mlist = market_t::get(bal.sym,"BTC");
  xassert(mlist.size()==1);
  auto &mkt=mlist[0];
  string res;
  if( bal.btc < min_txn_btc ) {
    xtrace("Buy Then Sell");
    if( mkt.sym() == bal.sym ) {
      money_t rate = mkt.ask();
      money_t qty = min_txn_btc/rate;
      xexpose(rate);
      xexpose(qty);
      string res=bittrex::simple_xact( mkt, true, qty, rate, false ); 
      cout << res << endl;
      return true;
    } else {
      money_t rate = mkt.bid();
      money_t qty = min_txn_btc*rate;
      xexpose(rate);
      xexpose(qty);
      string res=bittrex::simple_xact( mkt, false, qty, rate, false ); 
      cout << res << endl;
      return true;
    };
  } else if ( bal.btc < min_bal_btc ) {
    xtrace("we have enough " << bal.sym << " to sell, but not enough to keep");
    if( mkt.sym() == bal.sym ) {
      cout << "yes" << endl;
    } else {
      money_t rate=mkt.ask();
      money_t q = bal.bal/rate;
      money_t qty;
      ((qty=q));
      ((qty=q/1.0015));
      res=bittrex::simple_xact( mkt, true, qty, rate, false ); 
      cout << res << endl;
      return true;
    };
  } else {
    xtrace("how did we get here?");
  };
  return false;
};
int xmain(int argc, char**argv)
{
  for( int i=0;i<argc;i++){
    if(argv[i]==string("-y"))
      bittrex::fake_buys=false;
  };
  cout << min_bal_btc << " BTC min bal" << endl;
  cout << min_txn_btc << " BTC min trade" << endl; 
  auto bals=balance_t::get_balances();
  sort(bals.begin(),bals.end(),std::less<balance_t>());
  cout << market_t::get("USD","BTC") << endl;
  cout << endl;

  for( auto &bal : bals )
  {
    if( bal.sym == "BTC" ) {
    } else if( bal.btc > min_bal_btc ) {
      cout << " -- got enough to hold" << endl;
    } else if ( !bal.btc ) {
    } else {
      xtrace(bal.sym << ":");
      xtrace("  " << bal.bal);
      cout << bal << endl;
      if(adjust(bal)) {
        return 0;
      };
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
    int log=open_log("log/closeout.log",true);
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

