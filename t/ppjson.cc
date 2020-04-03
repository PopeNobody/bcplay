#include <iostream>
#include <web_api.hh>


using namespace std;

int main(int, char**) {
  cout << "Hello, World!" << endl;
  string text;
  for(;;){
    char ch = cin.get();
    if( ch == EOF )
      break;
    text+=ch;
  };
  cout << web::pp_json(text) << endl;
  return 0;
};
