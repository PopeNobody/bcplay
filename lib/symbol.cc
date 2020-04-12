#include <symbol.hh>
#include <coinfwd.hh>
#include <algorithm>


namespace coin {
  using namespace std;
//     ostream &operator<<(ostream &lhs, const sym_t &rhs) {
//       string tmp(rhs.get_width(),' ');
//       lhs << tmp;
//       //		assert(tmp.size()<6);
//       return lhs;
//     };
  coin::sym_t::~sym_t()
  {
  };
  ostream &sym_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  }
  ostream &sym_t::stream(ostream &lhs, int ind) const
  {
    auto flags = lhs.flags();
    auto fill = lhs.fill();
    lhs
      << setw(ind) << ""
      << left
      << setw(get_width())
      << c_str();
    lhs.flags(flags);
    lhs.fill(fill);
    return lhs;
  };
  size_t sym_t::get_width() const
  {
    return 6;
  };
};

