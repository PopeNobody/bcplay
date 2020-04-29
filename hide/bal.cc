#include <bittrex.hh>
#include <fmt.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

class goals_t : public map< sym_t, pct_t >
{
public:
  mapped_type& operator[]( const key_type& k )
  {
    return map< sym_t, pct_t >::operator[]( k );
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
  res["XLM"]=40;  res["BCH"]=40;  res["BSV"]=40;  res["BTC"]=40;  res["DASH"]=40;
  res["RVN"]=40;  res["KMD"]=40;  res["XMR"]=40;  res["ZEC"]=40;  res["ZEN"]=40;
  

  res["DGB"]=20;  res["DOGE"]=20;  res["ETH"]=20;  res["LTC"]=20;  res["PAX"]=20;
  double tot = 0;
  for ( auto goal : res )
  {
    tot = tot + (double)goal.second;
  };
  for ( auto& goal : res )
  {
    goal.second = (double)goal.second / tot;
  };
  int i=0;
  cout << res.size() << endl;
  for ( auto& goal : res )
  {
    cout << goal.first << " " << goal.second;
    if(++i%5)
      cout << " | ";
    else
      cout << endl;
  };
  cout << endl << endl;
  return res;
};
auto const& goals = mk_goals();

using bittrex::xact_limit;

inline money_t abs( money_t rhs )
{
  return ( rhs < 0 ) ? -rhs : rhs;
};

struct coin_data
{
  balance_t bal;
  pct_t     pct_goal;
  money_t   usd_goal;
  money_t   delta;
};
struct coin_less {
  bool operator()( const coin_data &lhs, const coin_data &rhs )
  {
    if(lhs.delta < rhs.delta)
      return true;
    if(rhs.delta < lhs.delta)
      return false;
    return lhs.bal.sym < rhs.bal.sym;
  };
};
typedef std::vector<coin_data> todo_t;
static void show_todos( const todo_t &todo, const money_t &tot );
typedef vector<string> argv_t;
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
  typedef std::map< sym_t, coin_data > data_m;
  data_m data;
  for ( auto g : goals )
  {
    data[ g.first ].bal.sym = g.first;
    data[ g.first ].pct_goal = g.second;
  };
  money_t tot = 0.0000001;
  for ( auto b : balance_l::load_balances() )
  {
    if ( 
        ( goals.find( b.sym ) != goals.end() )
        ||
        ( b.usd > 0.0099 )
       )
    {
      data[ b.sym ].bal = b;
      tot += b.usd;
    };
  }
  cout << "tot: " << tot << "USD" << endl;
  todo_t todo;
  auto pivot=data["BTC"].bal;
  for ( auto d : data )
  {
    pct_t pct_goal( d.second.pct_goal.get() );
    d.second.usd_goal = tot * pct_goal.get();
    d.second.bal.sym = d.first;
    d.second.delta = (d.second.usd_goal - d.second.bal.usd);
    todo.push_back(d.second);
    pivot.bal += d.second.delta;
  };
  sort(todo.begin(),todo.end(),coin_less());
  show_todos(todo, tot);

  auto pivot=data["BTC"].bal;
  auto b(todo.begin()), e(todo.end());
  for( ; b!=e && b->delta < 0; b++ )
  {
    if( b->bal.sym == pivot.sym ) {
      continue;
    };
    if( b->delta > -5 ) {
      continue;
    };
    cout 
      << -b->delta 
      << " in " 
      << b->bal.sym 
      << " to " 
      << pivot.sym 
      << endl;
    if(bittrex::fake_buys)
      continue;
    xact_limit(
        pivot.sym,
        b->bal.sym,
        b->delta,
        "USDT"
        );
    time_t dead=time(0)+5;
    while(bittrex::orders_pending()) {
      if(time(0)>dead)
        bittrex::cancel_orders();
    };
  };
  for( ; b!=e; b++ ) {
    if( b->bal.sym == pivot.sym ) {
      continue;
    };
    if( b->delta < 5 ) {
      continue;
    };
    cout 
      << b->delta 
      << " in " 
      << pivot.sym 
      << " to " 
      << b->bal.sym 
      << endl;
    if(bittrex::fake_buys)
      continue;
    xact_limit(
        pivot.sym,
        b->bal.sym,
        b->delta,
        "USDT"
        );
    time_t dead=time(0)+3;
    while(bittrex::orders_pending()) {
      if(time(0)>dead)
        bittrex::cancel_orders();
    };
  };
  return 0;
};
void show_todos( const todo_t &todo, const money_t &tot ) {
  if(!todo.size()) {
    cerr << "todo is empty" << endl;
    return;
  };
  auto const& temp = todo[0];
  cout << "|" << setw( temp.bal.sym.get_width() ) << "SYM "
    << "|" << setw( temp.bal.usd.get_width() ) << "cur$ "
    << "|" << setw( temp.pct_goal.get_width() ) << "cur% "
    << "|" << setw( temp.pct_goal.get_width() ) << "goal% "
    << "|" << setw( temp.bal.usd.get_width() ) << "goal$ "
    << "|" << setw( temp.bal.usd.get_width() ) << "delta "
    << "|" << endl;

  pct_t   tot_goal=0;
  money_t tot_delta=0;
  money_t tot_bal=0;
  for ( auto t : todo ) {
    cout
      << "|" 
      << t.bal.sym << "|"
      << t.bal.usd << "|"
      << pct_t( t.bal.usd / tot ) << "|"
      << t.pct_goal << "|"
      << t.usd_goal << "|"
      << t.delta << "|"
      << "|" << endl;

    tot_bal=tot_bal+t.bal.usd;
    tot_goal=tot_goal.get()+t.pct_goal.get();
    tot_delta+=t.delta;
  };
  cout << endl
    << "tot_goal: " << setw(12) << fixed << setprecision(8) << tot_goal
    << "tot_bal: " << setw(12) << fixed << setprecision(8) << tot_bal
    << "tot_delta: " << setw(12) << fixed << setprecision(8) << tot_delta
    << endl
    << endl;
};
int main( int argc, char** argv )
{
  try
  {
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
