#include <markets.hh>
#include <web_api.hh>
#include <util.hh>
#include <bittrex.hh>

using namespace util;
using namespace coin;
using namespace fmt;
market_t::market_t()
{
};
market_t market_t::reverse() const {
  market_t res(*this);
  swap(res.f_coin,res.t_coin);
  res.ask=1/res.ask;
  res.bid=1/res.bid;
  res.high=1/res.last;
  res.last=1/res.last;		
  res.low=1/res.low;
  res.vol=1/res.vol;
  return res;
}
const market_l& market_l::get_markets()
{
  static market_l markets;
  if(!markets.size())
    markets=bittrex::load_markets();
  return markets;
};
//   const market_l &market_l::load_markets() {
//     return markets=bittrex::load_markets();
//   };
//   money_t market_l::conv1(const string &from, const string &to) {
//     if(from==to)
//       return 1;
//     market_l list=get_conv(from,to);
//     if(list.size()==0)
//       return 0;
//     return list.back().last;
//   };
//   money_t market_l::conv(const string &from, const string &to) {
//     money_t c1 = conv1(from,to), c2=0;
//     if(c1)
//       return c1;
//     c1=conv1(from,"BTC");
//     c2=conv1("BTC",to);
//     return c1*c2;
//   };
#if 0
vector<market_l> market_l::all_conv(const string &f, const string &t)
{
  vector<market_l> res;
  const auto flist = split(',',f);
  const auto tlist = split(',',t);
  for( auto const &m1 : get_markets() )
  {
    if( flist.size() && !contains(flist,m1.f_coin) )
      continue;
    if(contains(tlist, m1.t_coin))
    {
      market_l marks;
      marks.push_back(m1);
      res.push_back(marks);
    };
  };
  for( auto const &m1 : get_markets() )
  {
    if( flist.size() && !contains(flist,m1.f_coin) )
      continue;
    if(contains(tlist, m1.t_coin))
      continue;
    auto m2list = get_conv(m1.t_coin,t);
    for( auto m2 : m2list )
    {
      market_l marks;
      marks.push_back(m1);
      marks.push_back(m2);
      res.push_back(marks);
    };
  };
  
  return res;
};
#endif
market_l market_l::get_conv(const sym_t &fsym, const sym_t &tsym )
{
  market_l res;
  for( auto const &m : get_markets() )
  {
    if( (m.f_coin == fsym && m.t_coin == tsym) || (m.f_coin == tsym && m.t_coin == fsym)  )
      res.push_back(m);
  };
  return res;
};
ostream &coin::market_l::header(ostream &lhs, int ind) const
{
  return lhs;
};
ostream &coin::market_l::stream(ostream &lhs, int ind) const
{
  lhs << setw(ind) << "" << "market_l [";
  if(size()) {
    lhs << nl;
    front().header(lhs,ind+2) << nl;
    for( auto m : *this ) {
      m.stream(lhs,ind+2) << nl;
    };
    lhs << setw(ind) << "";
  } else {
    lhs << " * ";
  };
  lhs << "]";
  return lhs;
};
static const sym_t no_sym;
ostream &coin::market_t::header(ostream &lhs, int ind) const
{
  lhs
    << left
    << setw(10) << "name"
    << setw(f_coin.get_width()) << "f_coin"
    << setw(t_coin.get_width()) << "t_coin"
    << setw(bid.get_width()) << "bid"
    << setw(ask.get_width()) << "ask"
    << setw(last.get_width()) << "last"
    ;
  return lhs;
};
ostream &coin::market_t::stream(ostream &lhs, int ind) const
{
  lhs 
    << setprecision(4)
    << left
    << setw(10) << name
    << setw(f_coin.get_width()) << f_coin
    << setw(t_coin.get_width()) << t_coin
    << setw(bid.get_width()) << bid
    << setw(ask.get_width()) << ask
    << setw(last.get_width()) << last
    ;
  return lhs;
};
string market_t::price() const {
  ostringstream str;
  str << fixed << setprecision(10);
  str << setw(6) << ("1 "+f_coin);
  str << " = ";
  str << last << " " << setw(6) << t_coin;
  return str.str();
};

can_str::~ can_str(){
};
market_l::market_l()
{
//  cerr << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl;
};
market_l::~ market_l(){
//  cerr << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl;
};
market_t::~ market_t(){
};
