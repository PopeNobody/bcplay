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
//   const market_l market_l::get_markets(sym_t sym)
//   {
//     market_l res;
//     for( auto m : get_markets() ) {
//       if(m.f_coin == sym || m.t_coin == sym)
//         res.push_back(m);
//     };
//     return res;
//   };
const market_l &market_l::load_markets() {
  return markets=bittrex::load_markets();
};
money_t market_l::conv1(const string &from, const string &to) {
  if(from==to)
    return 1;
  market_l list=get_conv(from,to);
  if(list.size()==0)
    return 0;
  auto &mkt=list.back();
  return (mkt.bid+mkt.ask)/2;
};
money_t market_t::conv1(const string &from, const string &to) {
  return market_l::conv1(from,to);
}
money_t market_t::conv(const string &from, const string &to) {
  return market_l::conv(from,to);
}
money_t market_l::conv(const string &from, const string &to) {
  money_t c1 = conv1(from,to), c2=0;
  if(c1)
    return c1;
  c1=conv1(from,"BTC");
  c2=conv1("BTC",to);
  return c1*c2;
};
market_l market_l::all_conv(const string &f, const string &t)
{
  market_l res;
  const auto flist = split(',',f);
  const auto tlist = split(',',t);
  for( auto const &m : get_markets() )
  {
    if( !contains(tlist,m.t_coin) )
      continue;
    if( !contains(flist,m.f_coin) )
      continue;
    res.push_back(m);
  };
  return res;
};
market_l market_l::get_conv(const string &f, const string &t)
{
  market_l res;
  const auto flist = split(',',f);
  const auto tlist = split(',',t);
  for( auto const &m : get_markets() )
  {
    if( !contains(tlist,m.t_coin) )
      continue;
    if( !contains(flist,m.f_coin) )
      continue;
    res.push_back(m);
  };
  return res;
};
//   ostream &coin::market_l::stream(ostream &lhs, int ind) const
//   {
//     lhs << setw(ind) << "" << "market_l {";
//     if(size())
//       lhs << nl;
//     for( auto m : *this ) {
//       m.stream(lhs,ind+2) << nl;
//     };
//     lhs << setw(ind) << "" << "}";
//     return lhs;
//   };
static const sym_t no_sym;
ostream &coin::market_t::stream( ostream &lhs, int ind ) const
{
  lhs 
    << setw(ind) << ""
    << "| "
    << setw(20) << price() << "| "
    << left << setw(12) << name << "|" 
    ;
  return lhs;
};
string market_t::price() const {
  ostringstream str;
  str << fixed << setprecision(10);
  str << setw(6) << ("1 "+f_coin);
  str << " = ";
  str << last << " " << setw(6) << t_coin;
  return move(str.str());
};
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

