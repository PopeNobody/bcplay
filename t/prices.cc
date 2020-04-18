#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>

#include <balance.hh>
#include <sqlite/connection.hpp>

using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;


int xmain(int argc, char**argv) {
  using coin::market_l;
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

