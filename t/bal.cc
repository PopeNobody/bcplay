#include <web_api.hh>
#include <bitcoin/bitcoin.hpp>
#include <typeinfo>
#include <web_api.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>
#include <util.hh>
using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using bc::data_chunk;
using bc::hmac_sha512_hash;
using bc::decode_base16;

using coin::market_l;
using coin::market_t;

class goals_t : public map<sym_t,pct_t> {
	public:
	mapped_type &operator[](const key_type& k)
	{
		return map<sym_t,pct_t>::operator[](k);
	};
	mapped_type operator[](const key_type& k) const 
	{
		auto pos=find(k);
		if( pos != end() )
			return pos->second;
		else
			return 0;
	};
};

goals_t const &mk_goals() {
	static goals_t res;
	
	res["BTC"]=24;
	res["BCH"]=24;
	res["XLM"]=24;
	res["RVN"]=24;
	res["USDT"]=24;

	double tot=0;
	for ( auto goal : res ) {
		tot=tot+(double)goal.second;
	};
	for ( auto &goal : res ) {
		goal.second=(double)goal.second/tot;
	};
	for ( auto goal : res ) {
		cout << goal.first << " " << pct_t(goal.second) << endl;
	};
	return res;
};
auto const &goals = mk_goals();

using bittrex::xact_limit;

money_t abs(money_t rhs)
{
	if(rhs<0)
		rhs=-rhs;
	return rhs;
};
int xmain(int argc, char**argv) {
	bittrex::fake_buys=true;
	if(argc>1 && string(argv[1])=="-y") {
		bittrex::fake_buys=false;
	}
	const market_l &markets=market_l::load_markets();
	balance_l bals;
	money_t tot_usd, tot_btc;
	for( auto b : balance_l::load_balances() ) {
		if( goals.find(b.sym)!=goals.end() ) {
			;
		} else if ( b.usd > 0.01 ) {
			;
		} else if ( b.sym == "BTC" ) {
			;
		} else {
			continue;
		};
		bals.push_back(b);
		tot_usd+=b.usd;
		tot_btc+=b.btc;
	};
	bool sell=true;	
	while(true) {
		for( auto b : bals ) {
			double goal=double(goals[b.sym]);
			money_t goal_usd=goal*tot_usd;
			money_t goal_btc=goal*tot_btc;
			cout << b.sym << " goal: " << pct_t(goal) << endl;
			if( b.sym == "BTC" )
				continue;
			money_t delta=(goal_btc-b.btc);
			cout << "delta: " << delta << endl;
			if( sell ) {
				if(delta>0)
					continue;
			} else {
				if(delta<0)
					continue;
			};
			if(abs(delta)<=0.001)
				continue;
			try {
				xact_limit(
						"BTC",
						b.sym,
						delta,
						"BTC"
						);
			} catch ( exception &e ) {
				cout << e.what() << endl;
			};

		};
		cout << endl;
		if(sell)
			sell=false;
		else
			break;
	};
	return 0;
};
int main(int argc, char**argv) {
	try {
		if(xmain(argc,argv))
			return 1;
		return 0;
	} catch ( exception &e ) {
		cout << nl << nl << e.what() << nl << nl;
	} catch ( ... ) {
		cout << nl;
		cout << nl;
		cout << nl;
		cout << "wtf?" << nl;
	}
	return 1;
};

