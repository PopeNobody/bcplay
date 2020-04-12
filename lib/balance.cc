#include <balance.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>

using fmt::nl;

namespace coin {
	balance_l balance_l::list;
	bool balance_t::operator<(const balance_t &rhs) const {
		if( usd < rhs.usd )
			return true;
		if( usd > rhs.usd )
			return false;
		return sym < rhs.sym;
	};
	const balance_l &balance_l::load_balances()
	{
		list=bittrex::load_balances();
    return list;
	};
  const balance_l &balance_l::get_balances() {
    if(list.size()==0)
      list=load_balances();
    return list;
  };
	sym_l balance_l::syms() const
	{
		sym_l res;
		for( auto b : *this ) {
			res.push_back(b.sym);
		};
		return res;
	};
#define SHOW_POS() do{ \
  cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl; \
}while(0)
#undef SHOW_POS
#define SHOW_POS()

	balance_t::balance_t() {
    SHOW_POS(); 
	};
	balance_t::~balance_t() {
    SHOW_POS(); 
	};
	balance_l::balance_l() {
    SHOW_POS(); 
	};
	balance_l::~balance_l() {
    SHOW_POS(); 
	};
	ostream &balance_t::header(ostream &lhs, int ind) const {
		return lhs;
	};
	ostream &balance_t::stream(ostream &lhs, int ind) const {
		lhs << sym << " U$" << usd;
		return lhs;
	};
};

