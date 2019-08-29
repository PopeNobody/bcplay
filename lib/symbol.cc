#include <symbol.hh>
#include <coinfwd.hh>


namespace coin {
	ostream &operator<<(ostream &lhs, const sym_t &rhs) {
		string tmp=rhs;
		assert(tmp.size()<6);
		tmp.resize(8,' ');
		return lhs << tmp;
	};
};

