#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <bittrex.hh>

#include <balance.hh>
using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;


int xmain(int argc, char**argv) {
  auto ml = bittrex::load_markets();
  for( m: ml ) {
    cout << m.name << endl;
  };
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

