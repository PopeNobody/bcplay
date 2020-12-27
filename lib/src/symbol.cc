#include <symbol.hh>
#include <coinfwd.hh>


ostream &coin::operator<<(ostream &lhs, const sym_t &rhs) {
  return lhs << (&lhs.text[0]);
};


