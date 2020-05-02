#ifndef markets_hh
#define markets_hh markets_hh

#include <money.hh>
#include <symbol.hh>
#include <fmt.hh>

namespace coin {
  using namespace std;
  class market_l;
  struct market_t : public fmt::can_str 
  {
    struct data_t {
      string name;
      sym_t cur;
      sym_t sym;
      money_t ask;
      money_t bid;
      money_t high;
      money_t last;
      money_t low;
      money_t vol;
    } data;
    static market_l markets;
    market_t();
    market_t(const data_t &);
    market_t(const market_t &);
    market_t(const string &name, money_t bid, money_t ask);
    market_t reverse() const;
   
    explicit operator bool() const; 
    static bool split_name(const string &name, string &cur, string &t_coin);
    bool operator<(const market_t &rhs) const;
    sym_t cur()const{
      return data.cur;
    }
    string name()const{
      return data.name;
    }
    sym_t sym()const{
      return data.sym;
    }
    money_t yield(money_t qty, sym_t f, sym_t t, bool neutral=false);
    money_t ask()const {
      return data.ask;
    };
    money_t bid()const {
      return data.bid;
    };
    virtual ~market_t();
    ostream &stream(ostream &lhs, int ind=0) const;
    static money_t conv(money_t, const sym_t &from, const sym_t &to, bool neutral=true);
    static money_t conv2(money_t, const sym_t &from, const sym_t &to, bool neutral=true);
    static const market_l &get_markets( );
    static const market_l &load_markets();
    static market_l get(const string &from, const string &to, bool exceptions=true);
  };
  struct market_l : public vector<market_t>, public fmt::can_str
  {
    virtual ~market_l();
    ostream &stream(ostream &lhs, int ind=0) const;
  };
}
#endif






























