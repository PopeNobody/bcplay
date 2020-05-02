#include <bittrex.hh>
#include <fmt.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <xcalls.hh>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

class goals_t : public map< sym_t, pct_t >
{
public:
  mapped_type& operator[]( const key_type& k )
  {
    auto &res=map< sym_t, pct_t >::operator[]( k );
    if(res.get()>0)
     cerr << "warning: " << k << " set" << endl;
    return res;
  };
  mapped_type operator[]( const key_type& k ) const
  {
    auto pos = find( k );
    if ( pos != end() )
      return pos->second;
    else
      return 0;
  };
};

goals_t const& mk_goals()
{
  static goals_t res;
  

res["ADA"]  =  1000;

res["BAT"]  =  2000;   res["DASH"]  =  2000;  res["ETC"]  =  2000;  res["ETH"]  =  2000;
res["ZEC"]  =  2000;   res["ZEN"]   =  2000;  res["LBC"]  =  2000;

res["LTC"]  =  7500;
res["XLM"]  =  7500;
res["BSV"]  =  20000; 
res["BCH"]  =  10000;
res["BTC"]  =  40000; 

  double tot = 0;
  for ( auto goal : res )
  {
    tot = tot + (double)goal.second;
  };
  cout << "tot: " << tot << endl;
  for ( auto& goal : res )
  {
    goal.second = (double)goal.second / tot;
  };
  map<double,vector<string>> ranks;
  for ( auto& goal : res )
  {
    ranks[goal.second.get()].push_back(goal.first);
  };
  int i=0;
  for ( auto &item : ranks ) 
  { 
    auto &rank=item.first;
    auto &ids=item.second;
    sort(ids.begin(),ids.end());
    auto itr(ids.begin());
    while(itr!=ids.end()) {
      cout << setw(8) << pct_t(item.first);
      for( int i = 0; i < 10; i++ )
      {
        if(itr==ids.end())
          break;
        cout << left << setw(10) << *itr++;
      };
      cout << endl;
    };
  };
  cout << endl << endl;
  return res;
};

using bittrex::xact_limit;

struct todo_t
{
  sym_t     sym;
  money_t   bal;
  money_t   btc;
  pct_t     pct;
  money_t   usd;
  pct_t     pct_goal;
  money_t   btc_goal;
  money_t   btc_del;
  todo_t(const sym_t &sym=sym_t())
    :sym(sym)
  {
    assert(!bal);
    assert(!pct);
    assert(!btc);
    assert(!usd);
    assert(!pct_goal);
    assert(!btc_goal);
    assert(!btc_del);
  };
  todo_t &operator+=(const todo_t &rhs)
  {
    if( rhs.sym == sym ) {
      bal+=rhs.bal;
    } else {
      bal=nan("mixed");
    };
    pct += rhs.pct;
    usd += rhs.usd;
    btc += rhs.btc;
    pct_goal += rhs.pct_goal;
    btc_goal += rhs.btc_goal;
    btc_del  += rhs.btc_del;
    return *this;
  };
};
struct todo_more {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) > (rhs.btc_del))
      return true;
    if((rhs.btc_del) > (lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) < (rhs.btc_del))
      return true;
    if((rhs.btc_del) < (lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_size {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(abs(lhs.btc_del) > abs(rhs.btc_del))
      return true;
    if(abs(rhs.btc_del) > abs(lhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
typedef std::vector<todo_t> todo_v;
typedef vector<string> argv_t;
money_t usd_spot =0;
money_t usd_min_size=10;
money_t min_size() {
  return usd_min_size/usd_spot;
};
struct header_t {
  const todo_t &obj;
  bool dashes;
  header_t(const todo_t &obj, bool dashes)
    : obj(obj), dashes(dashes)
  {
  };
};

ostream &operator<<(ostream &lhs, const header_t &head)
{
  auto const& temp = head.obj;
  auto symw=temp.sym.get_width();
  auto monw=temp.btc.get_width();
  auto pctw=temp.pct_goal.get_width();
  ostringstream text;
  text
    << left
    << "|" << setw( symw ) << "SYM "
    << right
    << "|" << setw( monw ) << "spot$ "
    << "|" << setw( monw ) << "spotB$ "
    << "|" << setw( pctw ) << "cur% "
    << "|" << setw( monw ) << "cur$ "
    << "|" << setw( monw ) << "curB$ "
    << "|" << setw( pctw ) << "goal% "
    << "|" << setw( monw ) << "goal$ "
    << "|" << setw( monw ) << "goalB$ "
    << "|" << setw( monw ) << "delB$"
    << "|" << setw( monw ) << "del$"
    << "|" << setw( pctw ) << "del%"
    << "|";
  string str=text.str();
  if(head.dashes) {
    for(auto &ch : str)
    {
      if(ch=='|')
        ch='+';
      else
        ch='-';
    };
  };
  lhs << str;
  return lhs;
};
inline ostream &operator<<(ostream &lhs, todo_t rhs)
{
  auto symw=rhs.sym.get_width();
  auto monw=rhs.btc.get_width();
  auto pctw=rhs.pct_goal.get_width();
  money_t spot_btc;
  money_t spot_usd;
  if(rhs.sym=="Total") {
    spot_usd=spot_btc=nan("total");
    spot_btc=nan("total");
    spot_usd=nan("total");
  } else {
    spot_btc= market_t::conv(money_t(1), rhs.sym,"BTC");
    spot_usd= market_t::conv(spot_btc,   "BTC",  "USD");
  };
  lhs
    << "|" 
    << left
    << setw(symw) << rhs.sym << "|"
    << right
    << setw(monw) << spot_usd << "|"
    << setw(monw) << spot_btc << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(monw) << rhs.usd << "|"
    << setw(monw) << rhs.btc << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(monw) << rhs.btc_goal*usd_spot << "|"
    << setw(monw) << rhs.btc_goal << "|"
    << setw(monw) << rhs.btc_del << "|"
    << setw(monw) << rhs.btc_del*usd_spot << "|"
    << setw(pctw) << pct_t(rhs.btc_del/rhs.btc) << "|"
    ;
  return lhs;
};
todo_v mk_todos()
{
  static auto const& goals = mk_goals();
  typedef std::map< sym_t, todo_t > todo_m;
  todo_m todo_map;
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  market_t::load_markets();
  money_t tot_btc = 0.0;
  for ( auto &b : balance_l::load_balances() )
  {
    if ( 
        ( goals.find( b.sym ) != goals.end() )
        ||
        ( b.usd > 5 )
       )
    {
      auto &todo=todo_map[ b.sym ];
      todo.sym = b.sym;
      todo.btc = b.btc;
      todo.usd = b.btc*usd_spot;
      todo.bal = b.bal;
      tot_btc += b.btc;
    };
  }
  {
    auto &btc=todo_map["BTC"];
    {
      todo_v todos;
      for ( auto &item : todo_map ) {
        auto &sym=item.first;
        auto &todo=item.second;

        todo.pct = todo.btc/tot_btc;
        todo.btc_goal = tot_btc * todo.pct_goal.get();
        todo.btc_del = (todo.btc_goal - todo.btc);
        if(todo.sym != "BTC")
          todos.push_back(todo);
      };
      sort(todos.begin(),todos.end(),todo_less());

      {
        vector<sym_t> avoid;
        todo_v willdo, maydo;
        int i=0;
        todo_t tot_all("Total");
        tot_all+=btc;
        cout << " " << header_t(btc,true) << endl;
        cout << " " << header_t(btc,false) << endl;
        cout << " " << header_t(btc,true) << endl;
        cout << " " << btc << endl;
        money_t min_pos=min_size();
        money_t max_neg=-min_size();
        if( abs(btc.btc_del)>=2*min_size() ) {
          if(btc.btc_del<0) {
            min_pos-=5/usd_spot;
            max_neg-=5/usd_spot;
          } else {
            min_pos+=5/usd_spot;
            max_neg+=5/usd_spot;
          };
        };
        while(i<todos.size())
        {
          auto &todo=todos[i++];
          tot_all+=todo;
          cout << " " << todo << endl;
          auto pos=find(avoid.begin(), avoid.end(), todo.sym);
          if(  pos!= avoid.end() )
            continue;
          if( (todo.btc_del < max_neg) || (todo.btc_del > min_pos) )
            willdo.push_back(todo);
          if( (
                (todo.btc_del < max_neg+8/usd_spot)
                && 
                ( btc.btc_del > min_size() )
              )
              ||
              (
               (todo.btc_del > min_pos-8/usd_spot)
               &&
               ( btc.btc_del < -min_size() )
               )
            )
            maydo.push_back(todo);
        };
        cout << "%" << tot_all << endl;
        cout << " " << header_t(btc,true) << endl;
        xexpose(willdo.size());
        xexpose(maydo.size());
        if(willdo.size())
          return willdo;
      };
    }
    return todo_v();
  };
};
int xmain( const argv_t &args )
{
  bool loop=false;
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else if ( arg == "-l" ) {
      loop=true;
      cout << "gonna loop" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };
  sym_t pivot="BTC";
  int trades=0;
  while(true) {
    time_t now=time(0);
    char buffer[1024];
    usd_spot = market_t::conv(1, "BTC","USD");
    auto todos=mk_todos();
    for( auto & todo: todos )
    {
      if( todo.sym == "BTC" )
        continue;
      market_l marks=market_t::get(todo.sym,"BTC");
      if(marks.size() != 1) {
        cout << "cannot do " << todo.sym << " no BTC market";
        return 1;
      };
    };
    if(todos.size() ) {
      for( auto b(todos.begin()), e(todos.end()); b!=e ; b++ )
      {
        if( b->sym == pivot )
          continue;
        
        pct_t change=abs(b->btc/b->btc_del);
        money_t qty=(b->bal/change.get());
        bool buy=(b->btc_del>0);
        if(buy)
          continue;
        if(b->sym == "BSV")
          continue;
        cout
          << (buy?"buy  ":"sell ")
          << b->sym
          << abs(qty)
          << " for BTC."
          << endl;
        auto mkts=market_t::get(b->sym,"BTC");
        cout << mkts << endl;
        if( abs(b->btc_del) < (5/usd_spot) ) {
          cout << "too small" << endl;
          continue;
        };

//           if(bittrex::fake_buys)
//             continue;
        xact_limit(
            pivot,
            b->sym,
            min(b->btc_del*usd_spot,money_t(100)),
            "USD",
            true
            );
        ++trades;
        break;
      };
      if(!(loop||bittrex::fake_buys))
      {
        mk_todos();
      };
    };
    if(!loop)
      break;
    sleep(15-(time(0)%15));
    
  };
  return 0;
};

int main( int argc, char** argv )
{
  try
  {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    xclose(1);
    mkdir("log",0755);
    int log=open_log("log/bal.log");
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/bal.log:1:started\n";
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << nl << nl << e.what() << nl << nl;
  }
  catch ( ... )
  {
    cout << nl;
    cout << nl;
    cout << nl;
    cout << "wtf?" << nl;
  }
  return 1;
};
