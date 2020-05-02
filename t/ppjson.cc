#include <iostream>
#include <web_api.hh>
#include <json.hh>


using namespace std;

int main(int, char**) {
  ifstream is;
  is.open("table.json");
  stringstream text;
  is >> text.rdbuf();
  string str=text.str();
  for( auto &ch : str )
  {
    cout.put(ch);
  };
  return 0;
};
