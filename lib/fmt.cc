#include "fmt.hh"
#include <coinfwd.hh>

namespace fmt {
	using namespace std;
	fp_val::~fp_val()
	{
	};
	int pct_t::get_width() const {
		return 8;
	};
	string pct_t::fmt() const {
		const static string sign("%");
		char buf[12];
		xassert(get_width()==8);
		snprintf(buf,sizeof(buf),"%7.3f",100*val);
		return buf+sign;
	};
};

