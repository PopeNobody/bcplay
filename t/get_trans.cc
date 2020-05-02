#include <web_api.hh>
#include <fmt.hh>
#include <dbg.hh>
#include <markets.hh>

#include <balance.hh>
#include <bittrex.hh>

using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;
using namespace bittrex;

int xmain(int argc, char**argv) {
  xtrace("Showing Deposits");
	show_deposits();
  xtrace("Showing withdrawals");
  show_withdrawals();
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

