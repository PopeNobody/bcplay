#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <bittrex.hh>
#include <xcalls.hh>
#include <balance.hh>
#include <util.hh>
#include <order.hh>
#include <dbg.hh>
using namespace coin;
using namespace util;
using namespace std;
using namespace fmt;

namespace bittrex {
  order_l get_order( const string& uuid );
};
void demo(money_t rate, pct_t spread, sym_t cur, sym_t sym)
{
  string name=cur+"-"+sym;
  xexpose(spread);
  money_t bid=(1+spread/2)*rate;
  xexpose(bid);
  money_t ask=rate*2-bid;
  xexpose(ask);
  if(bid>ask)
    swap(bid,ask);
  market_t cur_sym(name,bid,ask);
  cout << cur_sym << endl;
  for( auto neutral : { true, false } ) {
    auto sym2cur=cur_sym.yield(1,cur,sym,neutral);
    auto cur2sym=cur_sym.yield(1,sym,cur,neutral);
    xexpose(sym2cur);
    xexpose(cur2sym);
    xexpose(sym2cur*cur2sym);
  };
};
template<typename key_t, typename val_t>
auto get_keys(const map<key_t, val_t>&map)
{
  vector<key_t> res;
  res.resize(map.size());
  for( auto &item : map )
    res.push_back(item.first);
  return res;
};
int xmain(int argc, char**argv)
{
  for( int i=0;i<argc;i++){
    if(argv[i]==string("-y"))
      bittrex::fake_buys=false;
  };
  //demo(10000,0.01,"BTC","$$$");
  vector<balance_t> todo;
  auto &bals=balance_t::get_balances();
  map<sym_t,bool> syms;
  for( auto &sym : { "BTC", "USD" } )
    syms[sym]=true;
  for( auto &bal : bals )
    syms[bal.sym]=true;
  auto keys=get_keys(syms);
  auto btc_spot=market_t::conv(1,"BTC","USD");
  auto min_trans=0.00065;
  auto min_bal=2*min_trans;
  xexpose(min_bal);
  xexpose(market_t::conv(min_bal,"BTC","USD",true));
  balance_t btc;
  for( auto &bal : bals )
  {
    if( bal.sym == "BTC" ) {
      btc=bal;
    } else if ( bal.sym == "USD" ) {
    } else if( bal.sym == "BTXCRD" ) {
    } else if( bal.btc < min_bal ) {
      todo.push_back(bal);
    };
  };
  cout << "we have " << btc.bal << " " << btc.sym << "  to work with.\n\n";
  xassert(btc.bal);
  xexpose(todo.size());
  for( auto &bal : todo )
  {
    if( bal.bal == 0 )
      continue;
    if( bal.ava != bal.bal )
      continue;
    if( bal.btc > min_bal )
      continue;
    cout << bal << endl; 
    auto mkts=market_t::get(bal.sym,"BTC");
    auto &mkt=mkts[0];
    cout << "Got Market: " << string(50,'-') << endl;
    cout << mkt;
    cout << "End Market: " << string(50,'-') << endl;
    if( bal.sym == mkt.sym() && mkt.cur() == "BTC" )
    {
      // Sell to reduce bal, buy to increase it.
      string res;
      try {
        if( bal.btc < min_trans*0.9 ) {
          cout << "we must get more " << bal.sym << " first." << endl;
          xexpose(mkt.name());
          xexpose(mkt.cur());
          xexpose(mkt.sym());
          xexpose(mkt.bid());
          xexpose(mkt.ask());
          res=bittrex::simple_xact (
              mkt,
              true,
              mkt.yield(min_trans*1.02,"BTC",bal.sym,true),
              mkt.ask(),
              true
              );
          while(res.size())
          {
            order_l ords=bittrex::get_order(res);
            xexpose(ords.size());
            xexpose(ords);
            if( !ords[0].is_open() )
              break;
            break;
          };
          break;
        } else {
          cout << "we can sell now." << endl;
          xexpose(mkt.name());
          xexpose(mkt.cur());
          xexpose(mkt.sym());
          xexpose(mkt.bid());
          xexpose(mkt.ask());
          res=bittrex::simple_xact (
              mkt,
              false,
              bal.btc,
              mkt.bid(),
              false
              );
          while(res.size())
          {
            order_l ords=bittrex::get_order(res);
            xexpose(ords.size());
            xexpose(ords);
            if( !ords[0].is_open() ) {
              break;
            };
          };
          break;
        };
      } catch ( exception & ex ) {
        xexpose(ex.what());
        throw;
      };
    } else if( bal.sym == mkt.cur() && mkt.sym() == "BTC" ) {
      // Sell to increase bal, buy to reduce it.
      string res;
      try {
        if( bal.btc < min_trans*0.9 ) {
          cout << "we must buy first." << endl;
          res=bittrex::simple_xact (
              mkt,
              false,
              mkt.yield(min_trans*1.02,"BTC",bal.sym,true),
              mkt.bid(),
              true
              );
          while(res.size())
          {
            order_l ords=bittrex::get_order(res);
            xexpose(ords.size());
            xexpose(ords);
            if( !ords[0].is_open() ) {
              break;
            };
            break;
          };
          break;
        } else {
          cout << "we can sell now." << endl;
          xexpose(mkt.bid());
          xexpose(mkt.ask());
          res=bittrex::simple_xact (
              mkt,
              true,
              mkt.yield(bal.bal,bal.sym,"BTC",false)*0.997,
              mkt.ask(),
              true
              );
          while(res.size())
          {
            order_l ords=bittrex::get_order(res);
            xexpose(ords.size());
            if( !ords[0].is_open() ) {
              break;
            };
          };
          break;
        };
      } catch ( exception & ex ) {
        xexpose(ex.what());
        throw;
      };
    } else {
      xthrowre("WTF?");
    };
  }
#if 0
  {
    string res;
    if( mkt.data.buy )
    {
      cout << "mkt.buy=true" << endl;
      auto min_sale=min_bal*mkt.data.bid;
      res=bittrex::simple_xact (
          mkts[0],
          true,
          0.0005,
          1/mkt.data.bid,
          true
          );
    } else {
      cout << "mkt.buy=false" << endl;
      auto min_sale=0.005/mkt.data.bid;
      cout << "min: " << min_sale<<endl;
      res=bittrex::simple_xact (
          mkts[0],
          false,
          min_sale,
          mkt.data.ask,
          true
          );
    };
    if(bittrex::fake_buys)
      continue;
    cout << "order uuid is: " << res << endl;
    auto objs=bittrex::get_order(res);
    for( auto &obj : objs )
      cout << obj << endl;
    break;
  };
//     money_t usd_to_btc = mkt.ask;
//     for( auto & bal : bals )
//     {
//     };
#endif
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

