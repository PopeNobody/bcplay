#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>
#include <list>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;
typedef vector<string> argv_t;

class brief
{
  string text;
  public:
  brief(const market_t &dat)
  {
    ostringstream temp;
    temp << setprecision(4)
      << left
      << setw(8) << dat.sym()
      << right
      << setw(20) << dat.high()
      << setw(20) << dat.low()
      << setw(20) << dat.last();
    text=temp.str();
  };
  const string &str() const {
    return text;
  };
};
ostream &operator << (ostream &lhs, const brief &rhs)
{
  return lhs << rhs.str();
};
int xmain( const argv_t &args )
{
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };

  auto &bals = balance_t::load_balances();
  auto &mkts = market_t::load_markets();

  for( auto &mkt : mkts ) {
    if(mkt.cur() == "USDT") {
      if(balance_t::get(mkt.sym(),false).ava >= 1) {
      cout << brief(mkt) << endl;
      };
    };
  };
  return 0;
};
// This is atest.
// This is only a test
int main( int argc, char** argv )
{
  try
  {
    mkdir("log",0700);
    split_stream("log/bal.log");
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << e << nl;
  }
  catch ( ... )
  {
    cout << nl << "wtf?" << nl;
  }
  return 1;
};
