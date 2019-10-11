#include <markets.hh>
#include <web_api.hh>
#include <util.hh>
#include <bittrex.hh>

using namespace util;
using namespace coin;
using namespace fmt;
market_l coin::market_l::markets;
coin::market_t::market_t()
{
};
const market_l& market_l::get_markets()
{
	if(!markets.size())
		load_markets();
	return markets;
};
const market_l &market_l::load_markets() {
	return markets=bittrex::load_markets();
};
money_t market_l::conv1(const string &from, const string &to) {
	if(from==to)
		return 1;
	market_l list=get_conv(from,to);
	if(list.size()==0)
		return 0;
	return list.back().last;
};
money_t market_l::conv(const string &from, const string &to) {
	money_t c1 = conv1(from,to), c2=0;
	if(c1)
		return c1;
	c1=conv1(from,"BTC");
	c2=conv1("BTC",to);
	return c1*c2;
};
market_l market_l::get_conv(const string &f, const string &t)
{
	market_l res;
	const auto flist = split<vector<string>>(',',f.begin(),f.end());
	const auto tlist = split<vector<string>>(',',t.begin(),t.end());
	for( auto const &m : get_markets() )
 	{
		if( tlist.size()!=0 && !contains(tlist,m.t_coin) )
			continue;
		if( flist.size()!=0 && !contains(flist,m.f_coin) )
			continue;
		res.push_back(m);
	};
	return res;
};
ostream &coin::market_l::stream(ostream &lhs, int ind) const
{
	lhs << setw(ind) << "" << "market_l {";
	if(size())
		lhs << nl;
	for( auto m : *this ) {
		m.stream(lhs,ind+2) << nl;
	};
	lhs << setw(ind) << "" << "}";
	return lhs;
};
static const sym_t no_sym;
//   sym_t market_path::to_coin() const 
//   {
//   	return t_coin;
//   };
//   sym_t market_path_l::to_coin() const 
//   {
//   	if( size() ) {
//   		const auto &mark=back();
//   		return mark.to_coin();
//   	} else {
//   		return no_sym;
//   	};
//   };
//   sym_t market_path::from_coin() const 
//   {
//   	return f_coin;
//   };
//   sym_t market_path_l::from_coin() const 
//   {
//   	if( size() ) {
//   		const auto &mark=back();
//   		return mark.from_coin();
//   	} else {
//   		return no_sym;
//   	};
//   };
//   ostream &coin::market_path_l::stream(ostream &lhs, int ind) const
//   {
//   	lhs 
//   		<< setw(ind)<<""
//   		<< "market_path_l(" << from_coin() << "=>" << to_coin() << ") {" << nl
//   		;
//   	for( auto const &mp : *this )
//   		mp.stream(lhs,ind+2) << nl;
//   	lhs
//   		<< setw(ind)<<"" << "}";
//   	return lhs;
//   };
ostream &coin::market_t::stream( ostream &lhs, int ind ) const
{
	lhs 
		<< setw(ind) << ""
		<< "| "
		<< setw(20) << price() << " | "
		<< left << setw(12) << name << " |" 
		;
	return lhs;
};
//   ostream &coin::market_path::stream(ostream &lhs, int ind) const
//   {
//   	ostringstream str;
//   	str << setw(ind) << "";
//   	str << "market_path( " << f_coin << "," << t_coin << ") {" << nl;
//   	sym_t cur(f_coin);
//   	money_t rate=1;
//   	for( auto const &m : path ) {
//   		str << setw(ind+2) << "";
//   		str << (m.buy?"b ":"s ") << cur << ":" << m.last << " " << m.name << nl;
//   		rate*=m.last;
//   		cur=m.t_coin;
//   	}	
//   	str << setw(ind+2) << "";
//   	str << "  " << cur << rate << nl;
//   	str << setw(ind) << "";
//   	str << "}";
//   	return lhs << str.str();
//   };
string market_t::price() const {
	ostringstream str;
	str << fixed << setprecision(10);
	str << money_t(1) << " " << setw(6) << f_coin;
	str << " = ";
	str << last << " " << setw(6) << t_coin;
	return move(str.str());
};
//   money_t market_path::rate() const {
//   	money_t res(1);
//   	for( auto const &m : path ) {
//   		res*=m.last;
//   		res*=(1-1.0/400);
//   	}
//   	return res;
//   };
//   bool market_path::operator<(const market_path &rhs) const {
//   	if(f_coin!=rhs.f_coin)
//   		return f_coin<rhs.f_coin;
//   	if(t_coin!=rhs.t_coin)
//   		return t_coin<rhs.t_coin;
//   	if(rate()!=rhs.rate())
//   		return rate()<rhs.rate();
//   	if(path.size()!=rhs.path.size())
//   		return 	path.size()<rhs.path.size();
//   	if(path.size())
//   		return path[0].name < rhs.path[0].name;
//   	return false;
//   };
//   market_path::market_path(sym_t f_coin, sym_t t_coin, market_t m )
//   	: f_coin(f_coin), t_coin(t_coin)
//   {
//   	path.push_back(m);
//   };
//   market_path::market_path(sym_t f_coin, sym_t t_coin, market_t m1, market_t m2 )
//   	: f_coin(f_coin), t_coin(t_coin)
//   {
//   	path.push_back(m1);
//   	path.push_back(m2);
//   };
//   market_path_l::market_path_l(
//   		const sym_t &fsym, 
//   		const sym_t &tsym,
//   		const sym_l &syms
//   		)
//   {
//   	auto const &ml1=market_l::get_conv(fsym,"");
//   	for( auto const &m1 : ml1 ) {
//   		if(syms.size() && !contains(syms,m1.t_coin))
//   			continue;
//   		if( m1.t_coin == tsym ) {
//   			push_back(market_path(fsym,tsym,m1));
//   		} else {
//   			for( auto const &m2 : market_l::get_conv(m1.t_coin, tsym) ) {
//   				push_back(market_path(fsym,tsym,m1,m2));
//   			};
//   		};
//   		if(size()) {
//   			assert(back().f_coin==fsym);
//   			assert(back().t_coin==tsym);
//   		};
//   	};
//   	sort(begin(),end());
//   };
//   market_path_l market_l::get_paths(
//   		const sym_t &fsym, 
//   		const sym_t &tsym,
//   		const sym_l &syms
//   		)
//   {
//   	return market_path_l(fsym,tsym,syms);
//   };
bool market_t::split_name(const string &name, string &f_coin, string &t_coin)
{
	do {
		const auto b(name.begin()), e(name.end());
		auto s(find(b,e,'-'));
		f_coin.assign(b,s);
		t_coin.assign(++s,e);
		assert(f_coin.length()>1);
		assert(t_coin.length()>1);
		return true;
	} while(false);
	f_coin.clear();
	t_coin.clear();
	return false;
};

#define empty_des(x) x::~ x(){}
empty_des(can_str);
empty_des(market_l);
empty_des(market_t);
//   empty_des(market_path);
//   empty_des(market_path_l);

