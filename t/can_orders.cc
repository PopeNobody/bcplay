#include <bittrex.hh>
#include <fmt.hh>
#include <markets.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using coin::market_l;
using coin::market_t;

typedef vector<string> argv_t;
int xmain( argv_t args )
{
  bittrex::cancel_orders();
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << nl << nl << e.what() << nl << nl;
  }
  catch ( ... )
  {
    cout << nl;
    cout << nl;
    cout << nl;
    cout << "wtf?" << nl;
  }
  return 1;
}
