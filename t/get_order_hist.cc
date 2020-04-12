#include <web_api.hh>
#include <balance.hh>
#include <bittrex.hh>
#include <ctime>
#include <fmt.hh>
#include <iostream>
#include <json.hh>
#include <regex>

using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;
 
int xmain(int argc, char**argv) {
  auto orders = bittrex::get_order_history("USD-RVN");
  return 0;
};
int main(int argc, char**argv) {
  try {
    std::time_t result = std::time(nullptr);
    string fmt = "%Y-%m-%d-%H-%M-%S";
    char buf[4096];
    auto len=strftime(buf,sizeof(buf)-1,fmt.c_str(),std::localtime(&result));
    cout << buf << endl;
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

