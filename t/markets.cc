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
//     if( argc != 3 ) {
//       cerr << "usage: " << argv[0] << " <f-coin> <t-coin>" << endl;
//       exit(1);
//     };
//     sym_t f_coin(argv[1]);
//     sym_t t_coin(argv[2]);
  cerr << "Loading Markets" << endl;
  const market_l &markets=market_l::load_markets();
  map<string,vector<string>> names;
  for( auto &m: markets )
  {
    if(!m.buy)
      names[m.f_coin].push_back(m.t_coin);
  };
  for( auto &pair: names )
  {
    cout << left << setw(8) << pair.first;
    sort(begin(pair.second),end(pair.second));
    for( auto b(begin(pair.second)), e(end(pair.second)); b!=e; b++ )
      cout << " " << setw(8) << *b;
    cout << endl;
  };
//     marks=market_l::get_conv(f_coin, t_coin);
//     if( !marks.size() ) {
//       marks=market_l::get_conv(f_coin,"USD,USDT,BTC,ETC");
//     }; 
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

