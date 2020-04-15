#include "fmt.hh"
#include <coinfwd.hh>

namespace fmt {
	using namespace std;
	fp_val::~fp_val()
	{
	};
	size_t pct_t::get_width() const {
		return 18;
	};
  ostream &pct_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  };
  ostream &pct_t::stream(ostream &lhs, int ind) const
  {
    ostringstream str;
    str << setw(ind) << "";
    str << right << fixed << setprecision(3) << (100*val) << "%";
    lhs << setw(get_width()) << str.str();
    return lhs;
  };
};



