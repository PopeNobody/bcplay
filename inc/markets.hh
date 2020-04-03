#ifndef markets_hh
#define markets_hh markets_hh

#include <money.hh>
#include <symbol.hh>
#include <fmt.hh>

namespace coin {
  using namespace std;
  struct market_t : public fmt::can_str 
  {
    string name;
    sym_t f_coin;
    sym_t t_coin;
    bool buy;
    money_t ask;
    money_t bid;
    money_t high;
    money_t last;
    money_t low;
    money_t vol;
    market_t();
    market_t reverse() const {
      market_t res(*this);
      swap(res.f_coin,res.t_coin);
      res.buy=!res.buy;
      res.ask=1/res.ask;
      res.bid=1/res.bid;
      res.high=1/res.last;
      res.last=1/res.last;		
      res.low=1/res.low;
      res.vol=1/res.vol;
      return res;
    }
    static bool split_name(const string &name, string &f_coin, string &t_coin);
    string price() const;
    bool operator<(const market_t &rhs) const {
      if(last<rhs.last)
        return true;
      if(last>rhs.last)
        return false;
      if(f_coin<rhs.f_coin)
        return true;
      if(f_coin>rhs.f_coin)
        return false;
      return t_coin<rhs.t_coin;
    }
    sym_t from_coin()const{
      return f_coin;
    }
    sym_t to_coin()const{
      return t_coin;
    }
    virtual ~market_t();
    ostream &stream(ostream &lhs, int ind=0) const;
  };
  struct market_l : public vector<market_t> //, public fmt::can_str
  {
    market_l()
    {
    }
    sym_t from_coin()const;
    sym_t to_coin()const;
    virtual ~market_l();
    static market_l get_conv(const string &from, const string &to);
    static market_l markets;
    static const market_l &get_markets();
    static const market_l &load_markets();
    static market_l all_conv(const string &from, const string &to);
    static money_t conv(const string &from, const string &to);
    static money_t conv1(const string &from, const string &to);
    //ostream &stream(ostream &lhs, int ind=0) const;
  };
}
#endif






























