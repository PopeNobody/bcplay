#include <util.hh>

ostream &operator<<(ostream &lhs, const vector<string> rhs)
{
  ostringstream res;
  res << "{";
  auto b(rhs.begin());
  auto e(rhs.end());
  if(b!=e)
    res << *b++;
  while(b!=e)
    res << "," << *b++;
  res << "}";
  return lhs << res.str();
};
int check_join(const string &expect, const string &str, char del=',')
{
  auto result=util::join(del,str);
  cerr << "expected: " << expect << endl;
  cerr << "     got: " << result << endl;
  return ( expect != result );
};
int check_split(const string &str, const vector<string> expect, char del=',')
{
  auto result=util::split(del,str);
  cerr << "expected: " << expect << endl;
  cerr << "     got: " << result << endl;
  return ( expect != result );
};
int main(int, char**)
{
  int errs=0;
  errs+=check_split("",      {});
  errs+=check_split("a",     {"a"});
  errs+=check_split("a,b",   {"a","b"});
  errs+=check_split("a,,b",  {"a","","b"});
  errs+=check_split(",,b",   {"","","b"});
  errs+=check_split("a,",    {"a",""});
  errs+=check_split(",",     {"",""});
  return errs;
}
