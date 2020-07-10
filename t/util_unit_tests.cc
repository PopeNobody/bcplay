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
int check_split(const string &str, const vector<string> expect, char del)
{
  auto result=util::split(del,str);
  if( expect == result )
    return 0;
  cerr << "expected: " << expect << endl;
  cerr << "     got: " << result << endl;
  return 1;
};
int main(int, char**)
{
  int errs=0;
  errs+=check_split(",",{"",""},',');
  return 0;
}
