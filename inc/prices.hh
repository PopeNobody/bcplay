#ifndef prices_hh
#define prices_hh prices_hh

#include <coinfwd.hh>

namespace coin {
	using std::string;
	typedef std::vector<string> sym_v;

	struct res_t {
		string key;
		float  val;
		res_t(const string &key="", float val=0)
			: key(key), val(val)
		{
		};
	};
	typedef std::vector<res_t> res_v;

	inline int less_pair(const res_t &lhs, const res_t &rhs) {
		if( rhs.val < lhs.val )
			return true;

		if( lhs.val > rhs.val )
			return false;

		return lhs.key < rhs.key;
	};
	res_v get_hist_prices( size_t days_ago, const sym_v &);
	res_v get_prices(const sym_v &);
};


#endif

