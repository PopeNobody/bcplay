#include <money.hh>

namespace coin {
  using std::ostream;
  size_t money_t::get_width() const {
    return 12;
  };
  ostream &money_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  };
  ostream &money_t::stream(ostream &lhs, int ind) const
  {
    return lhs
      <<setw(ind)<<""
      <<setw(get_width())<<setprecision(4)<<get();
  };
  money_t::~money_t()
  {
  };
}
