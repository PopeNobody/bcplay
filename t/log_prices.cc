#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <bittrex.hh>
#include <util.hh>
#include <balance.hh>
using namespace coin;
using fmt::pct_t;
using fmt::nl;

using namespace std;


int xmain(int argc, char**argv) {
  using namespace bittrex;
  auto markets=load_markets();
  string curr, prod;

  set<string> rows;
  map<string,map<string,bool>> cols;
  for( auto m : markets )
  {
    auto x=split_name(m.name);
    cols[x.first][x.second]=true;
    rows.insert(x.second);
  };
  cout << boolalpha << left;
  cout << "|" << setw(10) << "*" << right;
  for( auto col : cols )
    cout << "|" << setw(10) << col.first;
  cout << "|" << endl;
  for( auto row : rows )
  {
    cout << left << "|" << setw(10) << row << right << "|";
    for( auto col : cols )
    {
      cout << setw(10) << (col.second[row]?"+":" ") << "|";
    };
    cout << endl;
  }
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

