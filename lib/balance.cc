#include <balance.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>

#undef checkin
#define checkin()

using fmt::nl;

namespace coin {
	bool balance_t::operator<(const balance_t &rhs) const {
		if( usd < rhs.usd )
			return true;
		if( usd > rhs.usd )
			return false;
		return sym < rhs.sym;
	};
//   	sym_l balance_l::syms() const
//   	{
//   		sym_l res;
//   		for( auto b : *this ) {
//   			res.push_back(b.sym);
//   		};
//   		return res;
//   	};

	balance_t::balance_t(const balance_t &rhs)
  {
    checkin();
    sym=rhs.sym;
    bal=rhs.bal;
    pend=rhs.pend;
    usd=rhs.usd;
    btc=rhs.btc;
	};
	balance_t::balance_t() {
    checkin(); 
	};
	balance_t::~balance_t() {
    checkin(); 
	};
	balance_l::balance_l() {
    checkin(); 
	};
	balance_l::~balance_l() {
    checkin(); 
	};
	ostream &balance_t::header(ostream &lhs, int ind) const {
		return lhs;
	};
	ostream &balance_t::stream(ostream &lhs, int ind) const {
		lhs << sym << " U$" << usd;
		return lhs;
	};
};

