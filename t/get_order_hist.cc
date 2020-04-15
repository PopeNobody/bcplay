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

using boost::date_time::Feb;
using boost::date_time::not_a_date_time;
using boost::gregorian::date;
using boost::gregorian::from_simple_string;
using boost::local_time::local_date_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using fmt::nl;
using fmt::pct_t;

int xmain(int argc, char**argv) {
  checkin();
  auto orders = bittrex::get_order_history("USD-RVN");
  return 0;
};
int main(int argc, char**argv)
{
  checkin();
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

