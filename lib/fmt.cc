#include "fmt.hh"
#include <coinfwd.hh>

namespace fmt {
	using namespace std;
	fp_val::~fp_val()
	{
	};
	size_t pct_t::get_width() const {
		return 8;
	};
  ostream &pct_t::header(ostream &lhs, int ind) const
  {
    return lhs;
  };
  ostream &pct_t::stream(ostream &lhs, int ind) const
  {
    char fill=lhs.fill();
    lhs<<setw(ind)<<"";
    lhs<<setw(get_width()-2)<<fixed<<setprecision(2)<<100*val<<"% ";
    lhs<<setfill(fill);
    return lhs;
  };
  size_t can_str::get_width() const
  {
    return string::npos;
  };
};



