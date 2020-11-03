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
  bittrex::dump_orders();
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    xclose(1);
    mkdir("log",0755);
    int log=open_log("log/orders.log");
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/orders.log:1:started\n";
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
#if 0
  {
    order_l ords;
    string jtxt=util::read_file("log/openorders.json");
    json jpage=json::parse(jtxt);
    cout << setw(4) << jpage << endl;
    coin::from_json(jpage,ords);
    return 0;
  }
#endif
