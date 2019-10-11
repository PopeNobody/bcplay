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
	
	res["BTC"]=1;
	res["BCH"]=1;
	res["XLM"]=1;
	res["RVN"]=1;
	res["BSV"]=1;
	double tot=0;
	for ( auto goal : res ) {
		tot=tot+(double)goal.second;
	};
	for ( auto &goal : res ) {
		goal.second=(double)goal.second/tot;
	};
	return res;
};
static auto const &goals = mk_goals();

using bittrex::xact_limit;

inline money_t abs(money_t rhs)
{
	return (rhs<0)?-rhs:rhs;
};

int xmain(int argc, char**argv) {
	struct coin_data {
		balance_t bal;
		pct_t goal;
	};
	std::map<sym_t,coin_data> data;
	for( auto g : goals ) {
		assert(g.first!="USDT");
		assert(g.first!="USD");
		data[g.first].bal.sym=g.first;
		data[g.first].goal=g.second;
	};
	money_t tot=0.0000001;
	coin_data usdt;
	usdt.goal=0.8;
	usdt.bal.sym="USDT";
	for( auto b : balance_l::load_balances() ) {
		if( b.sym == usdt.bal.sym ){
			usdt.bal=b;
			tot+=usdt.bal.usd;
			continue;
		};
		if( goals.find(b.sym)!=goals.end() ) {
			;
		} else if ( b.usd > 0.01 ) {
			;
		} else {
			continue;
		};
		data[b.sym].bal=b;
		tot += b.usd;
	}
	cout << "tot: " << tot << "USD" << endl;
	cout << "|"
		<< setw(usdt.bal.sym.get_width()) << "SYM " << "|"
		<< setw(usdt.bal.usd.get_width()) << "cur$ " << "|"
		<< setw(usdt.goal.get_width()) << "cur% " << "|"
		<< setw(usdt.goal.get_width()) << "goal% " << "|"
		<< setw(usdt.bal.usd.get_width()) << "goal$ " << "|"
		<< endl;
	cout << "|"
	 	<< usdt.bal.sym << "|"
		<< usdt.bal.bal << "|"
		<< pct_t(usdt.bal.bal/tot) << "|"
		<< usdt.goal << "|"
		<< tot*usdt.goal.get() << "|"
		<< endl;
	for( auto d : data ) {
		pct_t per_goal(d.second.goal.get()*(1-usdt.goal.get()));
		money_t usd_goal=tot*per_goal.get();
		d.second.bal.sym=d.first;
		cout << "|"
			<< d.first << "|"
			<< d.second.bal.usd << "|"
			<< pct_t(d.second.bal.usd/tot) << "|"
			<< per_goal << "|"
			<< usd_goal << "|"
			<< endl;
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

