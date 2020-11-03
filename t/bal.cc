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

typedef map<sym_t,double> goals_t;
goals_t goals;

static money_t _usd_min_size=10;
static money_t _usd_max_size=200;
static money_t _usd_spot;

const money_t & usd_spot()
{
  return _usd_spot;
};
const money_t & usd_spot(money_t new_spot)
{
  money_t res=_usd_spot;
  _usd_spot=new_spot;
  return _usd_spot;
};
const money_t & usd_min_size() {
  return _usd_min_size;
};
const money_t & usd_max_size() {
  return _usd_max_size;
};
money_t max_size() {
  return usd_max_size()/usd_spot();
};
money_t min_size() {
  return usd_min_size()/usd_spot();
};

void load_config()
{
  goals_t res;
  try {
    string text=util::read_file("etc/goals.json");
    json data=json::parse(text);
    json jgoals=data.at("goals");
    json jlims=data.at("limits");
    _usd_min_size=(double)jlims.at("usd_min_size");
    _usd_max_size=(double)jlims.at("usd_max_size");
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
  } catch ( exception &e ) {
    cout << e << endl;
  };
  json ndata;
  ndata["goals"]=res;
  json ldata;
  ldata["usd_min_size"]=usd_min_size().get();
  ldata["usd_max_size"]=usd_max_size().get();
  ndata["limits"]=ldata;
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
    money_t   btc_goal;
    money_t   btc_del;
   
    pct_t     pct;
    pct_t     pct_goal;
    todo_t(const sym_t &sym=sym_t())
      :sym(sym)
    {
      xassert(!bal);
      xassert(!pct);
      xassert(!btc);
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
    << "|" << setw( monw ) << "rawBal"
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
  } else {
    spot_btc= market_t::conv(money_t(1), rhs.sym,"BTC");
    spot_usd= market_t::conv(spot_btc,   "BTC",  "USD");
  };
  lhs
    << "|"
    << left
    << setw(symw) << rhs.sym << "|"
    << right
    << setw(monw) << rhs.bal << "|"
    << setw(monw) << spot_usd << "|"
    << setw(monw) << spot_btc << "|"
    << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(pctw) << (rhs.pct_goal-rhs.pct) << "|"
    << "|"
    << setw(monw) << rhs.btc*usd_spot() << "|"
    << setw(monw) << rhs.btc_goal*usd_spot() << "|"
    << setw(monw) << rhs.btc_del*usd_spot() << "|"
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
  if( !goals.size() )
    xthrowre("you have no goals!");
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
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
      todo.bal = b.bal;
      tot_btc += b.btc;
    };
  }
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
      auto &btc=todo_map["BTC"];
      {
        if(btc.btc_del<0) {
          sort(todos.begin(),todos.end(),todo_more());
        } else {
          sort(todos.begin(),todos.end(),todo_less());
        };
        todo_t tot_all("Total");
        for( auto &todo : todos  )
          tot_all+=todo;
        tot_all+=btc;
        show_todos(btc, todos, tot_all,"etc/todos.json");
      };
      {
        todo_v willdo;
        todo_t tot_all("Total");
        for( auto &todo:todos ) {
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

#define xverbose(x) xtrace(x)
void adjust(const todo_t &todo)
{
  const auto mkt_l=market_t::get("BTC",todo.sym);
  const auto &mkt=mkt_l[0];
  const auto &btc=todo_map["BTC"];

  sym_t qty_unit=mkt.sym();
  sym_t pri_unit=mkt.cur();

  money_t qty, tot, unitp; 
  bool is_buy;
  money_t btc_del=todo.btc_del;
  xexpose(todo.bal);
  if(btc_del > max_size()) {
    btc_del=max_size();
  } else if ( btc_del < -max_size() ) {
    btc_del=-max_size();
  };
  if(qty_unit == todo.sym && pri_unit=="BTC") 
  {
    xexpose(qty_unit);
    xexpose(pri_unit);
    xexpose(btc_del);

    if(btc_del>=0) {
      tot=btc_del;
      unitp=mkt.ask();
      qty=tot/unitp;
      xverbose(
          "AAAA buy " << qty << qty_unit << " for "
          << unitp << pri_unit
          << " per "  << qty_unit
          << "  Total: "
          << tot << pri_unit
          );
      is_buy=true;
    } else {
      tot=-btc_del;
      unitp=mkt.bid();
      qty=tot/unitp;
      xverbose(
          "BBBB sell " << qty << qty_unit << " for " << unitp
          << pri_unit
          << " per " << qty_unit
          << " total: "
          << tot << pri_unit
          );
      is_buy=false;
    }
  } else if(qty_unit=="BTC" && pri_unit==todo.sym) {
    xexpose(qty_unit);
    xexpose(pri_unit);
    xexpose(btc_del);

    if(btc_del>=0) {
      tot=btc_del;
      unitp=mkt.bid();
      qty=tot;
      xverbose(
          "DDDD sell " << qty << qty_unit << " for " << unitp
          << "each.  total "
          << tot << qty_unit
          );
      is_buy=false;
    } else {
      tot=-btc_del;
      unitp=mkt.ask();
      qty=tot;
      xverbose(
          "CCCC buy " << qty << qty_unit << " for " << unitp
          << "each.  total: "
          << tot << qty_unit
          );
      is_buy=true;
    };
  } else {
    xthrowre(
        "WTF? mkt symbols: cur="<<mkt.cur()<<" and prod=" << mkt.sym()
        );
  };
  string uuid=simple_xact(mkt, is_buy, qty, unitp, true );
  if(uuid.size() && uuid!="faked")
  {
    order_l ords=bittrex::get_order(uuid);
    cout << setw(4) << ords[0] << endl;
  };
  cout << endl << endl;
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
  usd_spot(market_t::conv(1, "BTC","USD"));
  auto todos=mk_todos();
  if(!todos.size() )
    return 0;

  sort(todos.begin(),todos.end(),todo_more());
  for(auto &todo:todos) 
    adjust(todo);
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    split_stream("log/bal.log");
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << e << nl;
  }
  catch ( ... )
  {
    cout << nl << "wtf?" << nl;
  }
  return 1;
};
