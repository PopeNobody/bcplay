#ifndef markets_hh
#define markets_hh markets_hh

#include <money.hh>
#include <symbol.hh>
#include <fmt.hh>
#include <json.hh>

namespace coin {
  using namespace std;
  struct market_l;
  struct market_t : public fmt::can_str 
  {
    friend void from_json( nlohmann::json& j, market_l& ml );
    string name;
    sym_t f_coin;
    sym_t t_coin;
    money_t ask;
    money_t bid;
    money_t high;
    money_t last;
    money_t low;
    money_t vol;
    private:
    market_t();
    public:

    virtual ~market_t();

    market_t reverse() const;
    size_t get_width() const
    {
      return string::npos;
    };
    static bool split_name(const string &name, string &f_coin, string &t_coin);
    string price() const;
    bool operator<(const market_t &rhs) const {
      if(name<rhs.name)
        return true;
      if(rhs.name<name)
        return false;
      if(f_coin<rhs.f_coin)
        return true;
      if(f_coin>rhs.f_coin)
        return false;
      return t_coin<rhs.t_coin;
    }
    ostream &header(ostream &lhs, int ind=0) const;
    ostream &stream(ostream &lhs, int ind=0) const;
  };
  struct market_l : public vector<market_t>, public fmt::can_str
  {
    market_l();
    virtual ~market_l();
    static const market_l &get_markets();
    ostream &stream(ostream &lhs, int ind=0) const;
    ostream &header(ostream &lhs, int ind=0) const;
    static market_l get_conv(const sym_t &f, const sym_t &t);
    size_t get_width() const { return string::npos; }
  };
}
#endif






























