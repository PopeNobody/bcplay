#include <balance.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>

using fmt::nl;

namespace coin {
	balance_l balance_l::list;
	bool balance_t::operator<(const balance_t &rhs) const {
		if( btc < rhs.btc )
			return true;
		if( btc > rhs.btc )
			return false;
		return sym < rhs.sym;
	};
	const balance_l &balance_l::load_balances()
	{
		return list=bittrex::load_balances();
	};
	sym_l balance_l::syms() const
	{
		sym_l res;
		for( auto b : *this ) {
			res.push_back(b.sym);
		};
		return res;
	};
	balance_t::~balance_t() {
	};
	ostream &balance_t::stream(ostream &lhs, int ind) const {
		lhs << sym << " U$" << usd << " B$" << btc;
		return lhs;
	};
};

