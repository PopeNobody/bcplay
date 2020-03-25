#include <symbol.hh>
#include <coinfwd.hh>


namespace coin {
  ostream &operator<<(ostream &lhs, const sym_t &rhs) {
    string tmp=rhs;
    tmp.resize(8,' ');
    lhs << tmp;
    //		assert(tmp.size()<6);
    return lhs;
  };
};

