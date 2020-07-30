#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <balance.hh>

using namespace coin;
using namespace std;
using fmt::pct_t;
using fmt::nl;



int xmain(int argc, char**argv) {
  cerr << "Loading Balances" << endl;
  balance_l bals = balance_l::load_balances();
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

