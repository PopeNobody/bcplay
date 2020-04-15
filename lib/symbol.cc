#include <symbol.hh>
#include <coinfwd.hh>
#include <algorithm>


namespace coin {
  using namespace std;
  coin::sym_t::~sym_t()
  {
  };
  ostream &sym_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  }
  ostream &sym_t::stream(ostream &lhs, int ind) const
  {
    string txt(ind+get_width()-size(),' ');
    txt+=(*this);
    lhs << txt;
    return lhs;
  };
  size_t sym_t::get_width() const
  {
    return 7;
  };
};

