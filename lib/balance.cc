#include <balance.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>

using fmt::nl;

namespace coin {
//   		balance_t::balance_t(const json &data)
//   			: sym("wtf?")
//   		{
//   			sym=data.at("Currency");
//   			bal=data.at("Balance");
//   			ava=data.at("Available");
//   			pend=data.at("Pending");
//   			using coin::market_l;
//   			auto const &markets=market_l::get_markets();
//   			usd=ava*markets.conv(sym,"USDT");
//   			eth=ava*markets.conv(sym,"ETH");
//   			btc=ava*markets.conv(sym,"BTC");
//   		};
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
};

