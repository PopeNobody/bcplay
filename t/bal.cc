#include <bittrex.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <xcalls.hh>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

typedef map<sym_t,double> goals_t;
goals_t mk_goals()
{
  goals_t res;
  string text=util::read_file("etc/goals.json"); 
  json data=json::parse(text);
  double sum=0;
  for( auto b(data.begin()), e(data.end()); b!=e; b++ )
  {
    double&val=res[sym_t(b.key())];
    val=(double)b.value();
    sum+=val;
  };
  for( auto &pair : res ) {
    pair.second/=sum;
  };
  return res;
};

using bittrex::simple_xact;

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
    xassert(!bal);
    xassert(!pct);
    xassert(!btc);
    xassert(!usd);
    xassert(!pct_goal);
    xassert(!btc_goal);
    xassert(!btc_del);
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
typedef std::map< sym_t, todo_t > todo_m;
struct todo_more {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) > (rhs.btc_del))
      return true;
    if((lhs.btc_del) < (rhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) < (rhs.btc_del))
      return true;
    if((lhs.btc_del) > (rhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_size {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(abs(lhs.btc_del) > abs(rhs.btc_del))
      return true;
    if(abs(lhs.btc_del) < abs(rhs.btc_del))
      return false;
    return lhs.sym > rhs.sym;
  };
};
typedef std::vector<todo_t> todo_v;
typedef vector<string> argv_t;
money_t usd_spot =0;
money_t usd_min_size=20;
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
    << "|"
    << "|" << setw( pctw ) << "cur% "
    << "|" << setw( pctw ) << "goal% "
    << "|" << setw( pctw ) << "del%"
    << "|"
    << "|" << setw( monw ) << "cur$ "
    << "|" << setw( monw ) << "goal$ "
    << "|" << setw( monw ) << "del$"
    << "|"
    << "|" << setw( monw ) << "curB$ "
    << "|" << setw( monw ) << "goalB$ "
    << "|" << setw( monw ) << "delB$"
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
    << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(pctw) << (rhs.pct_goal-rhs.pct) << "|"
    << "|"
    << setw(monw) << rhs.usd << "|"
    << setw(monw) << rhs.btc_goal*usd_spot << "|"
    << setw(monw) << rhs.btc_del*usd_spot << "|"
    << "|"
    << setw(monw) << rhs.btc << "|"
    << setw(monw) << rhs.btc_goal << "|"
    << setw(monw) << rhs.btc_del << "|"
    ;
  return lhs;
};
void show_todos(const todo_t &btc, todo_v &todos, const todo_t &tot_all)
{
  cout << " " << header_t(btc,true) << endl;
  cout << " " << header_t(btc,false) << endl;
  cout << " " << header_t(btc,true) << endl;
  if(btc.sym.size())
    cout << " " << btc << endl;
  for( auto &todo : todos ) {
    cout << " " << todo << endl;
  };
  cout << "%" << tot_all << endl;
  cout << " " << header_t(btc,true) << endl;
};
todo_m todo_map;
todo_v mk_todos()
{
  todo_map.clear();
  static auto const& goals = mk_goals();
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  market_t::load_markets();
  money_t tot_btc = 0.0;
  auto &btc=todo_map["BTC"];
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
      {
        vector<sym_t> avoid;
        int i=0;
        {
          todo_t tot_all("Total");
          if(btc.btc_del<0) {
            cout << "sort with more" << endl;
            sort(todos.begin(),todos.end(),todo_more());
          } else {
            cout << "sort with less" << endl;
            sort(todos.begin(),todos.end(),todo_less());
          };
          for( auto &todo : todos  )
            tot_all+=todo;
          tot_all+=btc;
          show_todos(btc, todos, tot_all);
        };
        {
          todo_t tot_all("Total");
          todo_v willdo;
          while(i<todos.size())
          {
            auto &todo=todos[i++];
            if( abs(todo.btc_del) >= min_size() ) {
              tot_all+=todo;
              willdo.push_back(todo);
            };
          };
          show_todos(todo_t(),willdo,tot_all);
          return willdo;
        }
      };
    }
    return todo_v();
  };
};
int xmain( const argv_t &args )
{
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };
  int trades=0;
  time_t now=time(0);
  char buffer[1024];
  usd_spot = market_t::conv(1, "BTC","USD");
  auto todos=mk_todos();
  if(!todos.size() ) {
    cout << "nothing to do!" << endl;
    return 0;
  };

  xassert(todo_map.find("BTC")!=todo_map.end());
  auto btc=todo_map["BTC"];
  sort(todos.begin(),todos.end(),todo_more());
  auto itr=todos.begin();
  if(btc.btc_del < 0 ) {
    cout << " " << todos.front() << endl;
    xassert(todos.front().btc_del>0);
    xassert(itr->sym==todos.front().sym);
  } else {
    cout << " " << todos.back()  << endl;
    itr=todos.end()-1;
    xassert(todos.back().btc_del<0);
    xassert(itr->sym==todos.back().sym);
  };

  auto &todo=*itr; 
  auto mkts=market_t::get("BTC",todo.sym);
  auto &mkt=mkts[0];
  cout << endl << mkt << endl << endl;
  xassert(mkt.cur()=="BTC");
  money_t btc_price=(todo.btc_goal-todo.btc);
  money_t usd_price=btc_price*usd_spot;
  if(usd_price > 100) {
    usd_price=100;
    btc_price=usd_price/usd_spot;
  };
  money_t btc_price_per_unit=mkt.yield(1,todo.sym,"BTC",false);
  money_t quantity=btc_price/btc_price_per_unit;
  xexpose(btc_price);
  xexpose(usd_price);
  xexpose(quantity);
  xexpose(btc_price_per_unit);

  auto ords=simple_xact( mkt, true, quantity, btc_price_per_unit, true);
  if(bittrex::fake_buys) {
    xtrace( " trades turned off" );
  } else if(!ords.size()) {
    xcarp("no order returned.  Somethign went wrong.");
  } else if ( ords.size()>1 ) {
    xcarp(">1 order returned.  Somethign went REALLY wrong.");
  } else {
    auto &ord=ords[0];
#define show(x) \
    cout \
    << left << setw(50) << #x << " : " \
    << right << setw(25) << x << endl;


    show(ord.data.exchange);
    show(ord.data.opened);
    show(ord.data.closed);
    cout << endl;
    show(ord.data.limit);
    cout << endl;
    show(ord.data.quantity);
    show(quantity);
    show(ord.data.price);
    show(btc_price);
    show(ord.data.price_per_unit);
    show(btc_price_per_unit);
    show(ord.data.quantity*ord.data.price_per_unit);
    show(btc_price_per_unit*quantity);
    show(ord.data.commission_paid);
    show(ord.data.commission_paid / 0.002);
    show(ord.data.quantity_remaining);
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
