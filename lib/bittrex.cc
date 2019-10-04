#include <bittrex.hh>
#include <util.hh>
#include <json.hh>
#include <web_api.hh>

using namespace coin;
using namespace util;
using namespace fmt;

bool bittrex::fake_buys=true;
bool bittrex::show_urls=false;

namespace coin {
	void from_json(json &j, market_t &m)
	{
		m.name=j.at("MarketName");
		if( !market_t::split_name(m.name,m.t_coin,m.f_coin) )
			throw runtime_error("split_name failed");
#define decode(x,y) if(j.at(#x).is_null()){m.y=0;}else{m.y=j.at(#x);}
#define do_decode(x,y) do{ decode(x,y) }while(0)
		do_decode(Last,last);
		do_decode(Bid,bid);
		do_decode(Ask,ask);
		do_decode(High,high);
		do_decode(Low,low);
		do_decode(Volume,vol);
		m.buy=false;
	};
	void from_json(json &j, market_l &ml)
	{
		for( auto it = j.begin(); it != j.end();it++ ) 
		{
			market_t cur;
			from_json(*it,cur);
			ml.push_back(cur);
			ml.push_back(cur.reverse());
		};
	};
	void from_json(const json &j, money_t &val) {
		val=0;
		if( !j.is_null() )
			val=(double)j;
	};
	void from_json(const json &j, balance_t &bal) {
		using coin::market_l;
		auto const &markets=market_l::get_markets();

		balance_t res;
		res.sym=j.at("Currency");
		res.bal=j.at("Balance");
		res.ava=j.at("Available");
		res.pend=j.at("Pending");
		res.usd=res.ava*markets.conv(res.sym,"USDT");
		res.eth=res.ava*markets.conv(res.sym,"ETH");
		res.btc=res.ava*markets.conv(res.sym,"BTC");
		bal=res;
	};
};
namespace bittrex {
	bool buy_limit(
			const coin::market_t &market,
			sym_t sym,
			coin::money_t tqty
			);
	bool sell_limit(
			const coin::market_t &market,
			sym_t sym,
			coin::money_t fqty
			);
};
bool bittrex::xact_limit(
		sym_t fsym,
		sym_t tsym,
		money_t qty,
		sym_t qunit
		)
{
	if( qty < 0 )
		return xact_limit(tsym,fsym,-qty,qunit);
	cout << "convert from " << fsym << " to " << tsym << endl;
	cout << "  quant: " << qty << " " << qunit << endl;
//   	cout << "xact:" << endl;
//   	cout << "  USD: " << qty*market_l::conv(qunit,"USDT") << endl;
//   	cout << "  BTC: " << qty*market_l::conv(qunit,"BTC") << endl;
//   	cout << "   FR: " << qty*market_l::conv(qunit,fsym) << endl;
//   	cout << "   TO: " << qty*market_l::conv(qunit,tsym) << endl;
	market_l marks=market_l::get_conv(fsym,tsym);
	assert(marks.size()==1);
	market_t market=marks[0];
	cout << market << endl;
	if(market.buy) {
		money_t tqty=qty*market_l::conv(qunit,tsym);
		return buy_limit(market,tsym,tqty);
	} else {
		money_t fqty=qty*market_l::conv(qunit,fsym);
		return sell_limit(market,fsym,fqty);
	};

//   	sym_t tsym(market.t_coin);
//   	money_t tqty(fqty*market.last);
//   	cout << "  fr: " << fsym << " qty: " << fqty << endl;
//   	cout << "  to: " << tsym << " qty: " << tqty << endl;
//   	if( market.buy ) {
//   		buy_limit(market,tsym,tqty);
//   	} else {
//   		sell_limit(market,fsym,fqty);
//   	};
};
bool bittrex::buy_limit(
		const market_t &market,
		sym_t sym,
		coin::money_t qty
		)
{
	money_t rate(1/market.ask);
	money_t total(qty*rate);
	cout 
		<< "buy  " << qty << sym
		<< " at " << rate
		<< " on " << market.name
		<< " for " << total
		<< endl
		;
	assert(market.t_coin==sym);

	const static string sl_url=
		"https://bittrex.com/api/v1.1/market/buylimit?";
	string url=sl_url;
	url+="market="+market.name;
	url+="&quantity="+strip(lexical_cast<string>(qty));
	url+="&rate="+strip(lexical_cast<string>(rate));
	url+="&";
	if(show_urls)
		cout << "url: " << url << endl;
	if(fake_buys){
		cout << " not gonna do it!" << endl;
		return true;
	};

	string page = web::load_hmac_page(url);
	cout << "res: " << endl;
	cout << page << endl;
	cout << endl;
	auto jpage=json::parse(page);
	cout << setw(4) << jpage << endl;
	if(!jpage.at("success")) {
		throw runtime_error(
				"no success in buylimit result\n\n"+page
				);
	};
	return true;
};
bool bittrex::sell_limit(
		const market_t &market,
		sym_t sym,
		coin::money_t qty
		)
{
	assert(market.f_coin==sym);
	cout 
		<< "sell " << money_t(qty) << sym
		<< " at " << money_t(market.bid)
		<< " on " << market.name
		<< endl
		;
	cout
		<< " get " << qty*market.bid
		<< endl
		;
	const static string sl_url=
		"https://bittrex.com/api/v1.1/market/selllimit?";
	string url=sl_url;
	url+="market="+market.name;
	url+="&quantity="+strip(lexical_cast<string>(qty));
	string str_pri=strip(lexical_cast<string>(money_t(market.bid)));
	url+="&rate="+str_pri;
	url+="&";
	if(show_urls)
		cout << "url: " << url << endl;
	if(fake_buys){
		cout << " not gonna do it!" << endl;
		return true;
	};

	string page = web::load_hmac_page(url);
	cout << "res: " << endl;
	cout << page << endl;
	cout << endl;
	auto jpage=json::parse(page);
	cout << setw(4) << jpage << endl;
	if(!jpage.at("success")) {
		throw runtime_error(
				"no success in selllimit result\n\n"+page
				);
	};
	return true;
};

void bittrex::show_withdrawals() {
	cout << "with" << endl;
	const static string gw_url=
		"https://bittrex.com/api/v1.1/account/getwithdrawalhistory?";
	string page = web::load_hmac_page(gw_url);
//	cout << string('-',20) << endl;
	cout << "--------------" << endl << page << endl << endl;
	json jpage=json::parse(page);
	if(!jpage.at("success")) {
		throw runtime_error(
				"no success in getbalances result\n\n"+page
				);
	};
	cout << endl << endl;
	cout << setw(4) << jpage;
	cout << endl << endl;
};
void bittrex::show_deposits() {
	cout << "dep" << endl;
	const static string gd_url=
		"https://bittrex.com/api/v1.1/account/getdeposithistory?";
	string page = web::load_hmac_page(gd_url);
//	cout << string('*',20) << endl;
	json jpage=json::parse(page);
	if(!jpage.at("success")) {
		throw runtime_error(
				"no success in getbalances result\n\n"+page
				);
	};
	cout << endl << endl;
	cout << setw(4) << jpage;
	cout << endl << endl;
};
const coin::balance_l bittrex::load_balances()
{
	balance_l temp;
	const static string gb_url=
		"https://bittrex.com/api/v1.1/account/getbalances?";
	string page = web::load_hmac_page(gb_url);
	json jpage=json::parse(page);
	if(!jpage.at("success")) {
		throw runtime_error(
				"no success in getbalances result\n\n"+page
				);
	};
	json bals=*jpage.find("result");
	for( json bal : bals ) {
		temp.push_back(balance_t(bal));
	};
	sort(temp.begin(),temp.end());
	//cout << "read " << temp.size() << " balances" << nl;
	return balance_l(temp.rbegin(),temp.rend());
};
const market_l bittrex::load_markets() {
	const static string gms_url=
		"https://bittrex.com/api/v1.1/public/getmarketsummaries";
	json jpage;
#if 0
	if(exists("markets.json.use") && exists("markets.json")) {
		cout << "reading from file" << endl;
		string page=read_file("markets.json");
		jpage=json::parse(page);
	} else {
#else
	{
#endif
		string page=web::load_page(gms_url);
 		jpage = json::parse(page);
		ofstream ofile("markets.json");
		cout << "writing to file" << endl;
		ofile << setw(4) << jpage << endl;
	};


	assert(jpage.type() == value_t::object);
	auto it=jpage.find("success");
	if( it != jpage.end() ) {
		if( !it.value() ) {
			throw runtime_error("no success in page");
		}
	};
	auto res_it=jpage.find("result");
	if( res_it == jpage.end() ) {
		throw runtime_error("no result in page");
	};
	jpage=*res_it;
	market_l markets;
	from_json(jpage,markets);
	//cout << "read " << markets.size() << " markets" << endl;
	return markets;
};
void bittrex::dump_orders() {
	const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
	string page = web::load_hmac_page(url);
	cout << setw(4) << json::parse(page) << endl;
};

