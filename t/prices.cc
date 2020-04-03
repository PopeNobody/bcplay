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
  vector<sym_t> show_marks = { 
    "BTC", "BCH", "BSV"
  };
  vector<sym_t> div_btc = {
    "BCH", "BSV"
  };
  bool first=true;
  auto stime = 1;
  while((stime%15)!=1) {
    stime=time(0);
  };
  while(true) {
    market_l markets=market_l::load_markets();
    if(first) {
      cout << setw(10) << "T";
      for( auto m : show_marks ) {
        cout << setw(10) << ""+m;
      }
      for ( auto m : div_btc ) {
        cout << setw(14) << ""+m+"/BTC";
      };
      cout << endl;
      first=false;
    };
    cout 
      << right 
      << setw(10) << (time(0)-stime);
    for( auto m : show_marks ) {
      cout 
        << fixed
        << setprecision(3)
        << setw(10) 
        << 0.001*int(markets.conv(m,"USD").get()*1000);
    };
    for ( auto m : div_btc ) {
      cout << 
        fixed << setprecision(8) <<
        setw(14) << markets.conv("BTC",m).get();
    };
    cout << endl;
    bool done=false;
    while(!done) {
      done=!(time(0)%15);
      sleep(1);
    };
  }
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

