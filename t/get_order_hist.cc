#include <web_api.hh>
#include <balance.hh>
#include <bittrex.hh>
#include <fmt.hh>
#include <json.hh>
#include <dbg.hh>

#include <ctime>
#include <iostream>
#include <regex>
#include <boost/date_time/posix_time/ptime.hpp>

using namespace coin;
using namespace std;

using fmt::nl;
using fmt::pct_t;

int xmain(int argc, char**argv) {
  xcheckin();
  auto orders = bittrex::get_order_history("USD-RVN");
  return 0;
};
int main(int argc, char**argv)
{
  xcheckin();
  try {
    return xmain(argc,argv);
  } catch ( std::exception &e ) {
    cout << endl << endl << e.what() << endl << endl;
  } catch ( ... ) {
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "wtf?" << endl;
  };
  return -1;
};

