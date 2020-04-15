#include <money.hh>
#include <sstream>
#include <iomanip>

namespace coin {
  using namespace std;
  size_t money_t::get_width() const {
    return 22;
  };
  ostream &money_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  };
  ostream &money_t::stream(ostream &lhs, int ind) const
  {
    static const char *ptr;
    ostringstream text;
    text << setw(ind) << "";
    text << fixed << setprecision(9) << setw(get_width()) << get();
    ptr=text.str().c_str();
    lhs<<text.str();
    return lhs;
  };
  money_t::~money_t()
  {
  };
}
