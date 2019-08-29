#include "fmt.hh"
#include <coinfwd.hh>

namespace fmt {
	using namespace std;
	fp_val::~fp_val()
	{
	};
	string pct_t::fmt() const {
		const static string sign("%");
		char buf[12];
		assert(get_width()==7);
		snprintf(buf,sizeof(buf),"%7.3f",100*val);
		return buf+sign;
	};
};

