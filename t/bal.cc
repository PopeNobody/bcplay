#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;
ostream &operator<<(ostream &lhs, const exception &rhs)
{
  return lhs<<rhs.what();
};
ostream &operator<<(ostream &lhs, const type_info &rhs)
{
  return lhs<<demangle(typeid(rhs).name());
};
typedef map<sym_t,double> goals_t;
goals_t goals;
money_t usd_spot =0;
money_t usd_min_size=20;
money_t min_size() {
  return usd_min_size/usd_spot;
};
void load_config()
{
  goals_t res;
  string text=util::read_file("etc/goals.json");
  json data=json::parse(text);
  json jgoals;
  if(data.find("goals")==data.end())
  {
    jgoals=data;
  } else {
    jgoals=data.at("goals");
    usd_min_size=(double)data.at("usd_min_size");
  };
  double sum=0;
  for( auto b(jgoals.begin()), e(jgoals.end()); b!=e; b++ )
  {
    double&val=res[sym_t(b.key())];
    val=(double)b.value();
    sum+=val;
  };
  cout << "sum of goals is " << sum << endl;
  for( auto &pair : res ) {
    pair.second/=sum;
  };
  goals=res;
  for( auto &goal : res ) {
    goal.second*=100000;
  };
  json ndata;
  ndata["goals"]=res;
  ndata["usd_min_size"]=usd_min_size.get();
  stringstream str;
  str << setw(4) << ndata;
  write_file("etc/goals.json.new",str.str());
};

using bittrex::simple_xact;

namespace coin {
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
  typedef std::vector<todo_t> todo_v;
  typedef std::map< sym_t, todo_t > todo_m;
  void to_json(json &dst, const todo_t &src);
};
void coin::to_json(json &dst, const todo_t &src)
{
  try {
    json res;
    res["sym"]=src.sym;
    res["bal"]=src.bal;
    res["btc"]=src.btc;
    res["pct"]=src.pct;
    res["usd"]=src.usd;
    res["pct_goal"]=src.pct_goal;
    res["btc_goal"]=src.btc_goal;
    res["btc_del"]=src.btc_del;
    dst=res;
  } catch ( const exception &ex ) {
    xtrace(ex.what());
    throw;
  };
};
using coin::todo_t;
using coin::todo_m;
using coin::todo_v;
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
typedef vector<string> argv_t;
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
void show_todos(const todo_t &btc, todo_v &todos, const todo_t &tot_all, const string &fname=string())
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
  if(fname.size())
  {
    json fmt=json::parse("[]");
    fmt=todos;
    bittrex::save_json(fname,fmt,false);
  };
};
todo_m todo_map;
todo_v mk_todos()
{
  todo_map.clear();
  if( !goals.size() ) {
    xcarp("you have no goals!");
    return todo_v();
  };
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  market_t::get_markets();
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
          show_todos(btc, todos, tot_all,"etc/todos.json");
        };
        {
          todo_v willdo;
          todo_t tot_all("Total");
          while(i<todos.size())
          {
            auto &todo=todos[i++];
            if( abs(todo.btc_del) >= min_size() ) {
              tot_all+=todo;
              willdo.push_back(todo);
            };
          };
          if(willdo.size())
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
  load_config();
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
  xassert(todos.back().btc_del<=todos.front().btc_del);
  auto itr=todos.begin();
  if(btc.btc_del < 0 ) {
    cout << " " << todos.front() << endl;
    xassert(itr->sym==todos.front().sym);
  } else {
    cout << " " << todos.back()  << endl;
    itr=todos.end()-1;
    xassert(itr->sym==todos.back().sym);
  };
  if( (itr->btc_del<=0) == (btc.btc_del<0) ) {
    cout
      << "BTC and "  << itr->sym
      << " are unbalanced in the same direction"
      << endl;
    return 0;
  };

  auto &todo=*itr;
  auto mkts=market_t::get("BTC",todo.sym);
  auto &mkt=mkts[0];
  cout << endl << mkt << endl << endl;
  xassert(mkt.cur()=="BTC");
  money_t btc_qty=(todo.btc_goal-todo.btc);
  money_t usd_qty=btc_qty*usd_spot;
  if(usd_qty > 100) {
    xtrace("*** limiting purchase to $100");
    usd_qty=100;
    btc_qty=usd_qty/usd_spot;
  };
  money_t price_per_unit;
  if(btc_qty>0) {
    price_per_unit=mkt.yield(1,todo.sym,"BTC",false);
  } else {
    price_per_unit=1/mkt.yield(1,"BTC",todo.sym,false);
  };
  money_t quantity=btc_qty/price_per_unit;
  money_t price=quantity*price_per_unit;
  xtrace( ""
      << "qty: " << quantity << todo.sym
      << " | " << btc_qty << "B$ "
      << usd_qty << "$"
      << " | " << "price_per_unit: " << price_per_unit
      << todo.sym << "/BTC"
      );
  string uuid;
  if( quantity>0 ) {
    uuid=simple_xact( mkt, true, quantity, price_per_unit, true);
  } else {
    quantity=-quantity;
    price=-price;
    uuid=simple_xact( mkt, false, quantity, price_per_unit, true);
  };
  if(uuid=="faked") {
    return 0;
  };
  order_l ords;
  while(true)
  {
    ords=bittrex::get_order(uuid);
    if(ords.size())
      break;
    xcarp("no order returned!");
    sleep(1);
  };
  auto &ord=ords[0];
#define show(x) \
    cout \
    << left << setw(50) << #x << " : " \
    << right << setw(25) << x << endl;
    show(mkt.ask());
    show(mkt.bid());
    show(price_per_unit);
    cout << endl;

    show(ord.data.exchange);
    show(ord.data.opened);
    show(ord.data.closed);
    cout << endl;
    show(ord.data.limit);
    cout << endl;
    show(ord.data.quantity);
    show(quantity);
    show(ord.data.price);
    show(price);
    show(ord.data.price_per_unit);
    show(price_per_unit);
    show(ord.data.quantity*ord.data.price_per_unit);
    show(price_per_unit*quantity);
    show(ord.data.commission_paid);
    show(ord.data.commission_paid / 0.002);
    show(ord.data.quantity_remaining);

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
