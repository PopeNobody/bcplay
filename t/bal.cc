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


using bittrex::xact_limit;

inline money_t abs( money_t rhs )
{
  return ( rhs < 0 ) ? -rhs : rhs;
};

struct todo_t : public fmt::can_str
{
  sym_t sym;
  money_t usd_bal;
  pct_t pct_goal;
  money_t usd_goal;
  money_t usd_delta() const
  {
    return -usd_bal+usd_goal;
  };
  int delta_sign() const
  {
    return usd_delta()/abs(usd_delta());
  };
  ostream &stream(ostream &lhs, int ind=0) const
  {
    return lhs
      << setw(ind) << ""
      << sym
      << " " << usd_bal
      << " " << pct_goal
      << " " << usd_goal
      << " " << usd_delta();
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(lhs.usd_delta() < rhs.usd_delta())
      return true;
    if(rhs.usd_delta() < lhs.usd_delta())
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_more {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if(rhs.usd_delta() < lhs.usd_delta())
      return true;
    if(lhs.usd_delta() < rhs.usd_delta())
      return false;
    return rhs.sym < lhs.sym;
  };
};
typedef std::vector<todo_t> todo_v;
typedef std::map< sym_t, todo_t > todo_m;
static void show_todos( const todo_v &todo, const money_t &tot );
typedef vector<string> arg_v;
goals_t const& mk_goals()
{
  static goals_t res;
  res[ "BAT" ] = 1;
  res[ "BCH" ] = 1;
  res[ "BSV" ] = 1;
  res[ "BTC" ] = 1;
  res[ "DASH" ] = 1;
  res[ "LBC" ] = 1;
  res[ "RVN" ] = 1;
  res[ "XLM" ] = 1;
  res[ "XMR" ] = 1;
  res[ "ZEN" ] = 1;
  res[ "SBD" ] = 1;
  res[ "HIVE" ] = 1;

  pct_t tot = 0;
  for( auto g : res ) {
    tot = tot.get()+g.second.get();
  };
  res[ "USDT" ] = tot.get()/3*2;
  tot = tot.get()+tot.get()/3*2;
  cout << "goals: " << endl;
  for( auto & g : res ) {
    g.second = pct_t(g.second.get()/tot.get());
    cout << g.first << " " << g.second << endl;
  };
  cout << endl;
  return res;
};

static const todo_v mk_todo( )
{
  todo_m todo_sym;
  auto const &balances = balance_l::load_balances();
  auto const &goals = mk_goals();
  money_t tot_usd;
  for ( auto const &b : balances )
    tot_usd += b.usd;
  cout << "tot_usd: " << tot_usd << endl;
  int i=1;
  for ( auto const &g : goals ) 
  {
    auto &todo = todo_sym[g.first];
    todo.sym=g.first;
    auto ptr = balances.get_ptr(g.first);
    if(ptr)
      todo.usd_bal=ptr->usd;
    todo.pct_goal=g.second;
    todo.usd_goal=tot_usd*g.second.get();
  };
  for ( auto const &b : balances )
  {
    if(b.usd < 0.01)
      continue;
    if(todo_sym.find(b.sym)!=todo_sym.end())
      continue;
    auto &todo = todo_sym[b.sym];
    todo.sym=b.sym;
    todo.usd_bal=0;
    todo.pct_goal=0;
    todo.usd_goal=0;
  };
  todo_v todos;
  for ( auto const &todo : todo_sym )
  {
    todos.push_back(todo.second);
  };
  return todos;
};

int xmain( const arg_v &args )
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
  if(!bittrex::fake_buys) {
    while(bittrex::orders_pending())
      bittrex::cancel_orders();
  }
  sym_l base_syms;
  base_syms.push_back("BTC");
  base_syms.push_back("ETC");
  base_syms.push_back("USD");
  base_syms.push_back("USDT");
  string base_str = join(',',base_syms);


  todo_v todos;
  todo_m bases;
  {
    auto todo_l = mk_todo();
    int i =1;
    for( auto const &todo : todo_l )
    {
      if(contains(base_syms,todo.sym))
      {
        bases[todo.sym]=todo;
      } else {
        todos.push_back(todo);
      };
    };
    sort(todos.begin(),todos.end(),todo_less());
  };
  {
    int i = 0;
    while( i < todos.size() )
    {
      auto todo = todos[i];
      if(todo.delta_sign()>=0)
        break;
      for( auto &base_pair : bases )
      {
        auto &base = base_pair.second;
        if(market_l::get_conv(todo.sym,base.sym).size()!=1)
          continue;
        if(base.delta_sign()<=0)
          continue;
        money_t qty = min(-todo.usd_delta(),base.usd_delta())/2;
        qty = min(money_t(100),qty);
        if(qty >= money_t(5)) {
          xact_limit(
              todo.sym,
              base.sym,
              qty,
              "USDT"
              );
          todo.usd_bal -= qty;
          base.usd_bal += qty;
        };
      };
      i++;
      cout << endl << endl;
    };
    while( i < todos.size() )
    {
      auto todo = todos[i];
      if(todo.delta_sign()==0)
        continue;
      if(todo.delta_sign()<0)
        break;
      for( auto &base_pair : bases )
      {
        auto &base = base_pair.second;
        if(market_l::get_conv(todo.sym,base.sym).size()!=1)
          continue;
        if(base.delta_sign()>=0)
          continue;
        money_t qty = min(todo.usd_delta(),-base.usd_delta())/2;
        cout << todo << endl;
        cout << base << endl;
        cout << todo.sym << " qty=" << qty << endl;
        qty = min(money_t(100),qty);
        cout << todo.sym << " qty=" << qty << endl;
        if(qty >= money_t(5)) {
          xact_limit(
              base.sym,
              todo.sym,
              qty,
              "USDT"
              );
          todo.usd_bal -= qty;
          base.usd_bal += qty;
        };
      };
      i++;
      cout << endl << endl;
    };
  };

#if 0
  todo_m data;
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
  todo_v todo;
  for ( auto d : data )
  {
    pct_t pct_goal( d.second.pct_goal.get() );
    d.second.usd_goal = tot * pct_goal.get();
    d.second.bal.sym = d.first;
    d.second.delta = d.second.usd_goal - d.second.bal.usd;
    todo.push_back(d.second);
  };
  sort(todo.begin(),todo.end(),coin_less());
  show_todos(todo, tot);

  auto pivot=data["BTC"].bal;
  auto b(todo.begin()), e(todo.end());
  for( ; b!=e && b->delta < 0; b++ )
  {
    if( b->bal.sym == pivot.sym ) {
      cout << "skip: " << b->bal.sym << " sym 1" << endl;
      continue;
    };
    if( b->delta > -5 ) {
      cout << "skip: " << b->bal.sym << " <$5 1" << endl;
      continue;
    };
    cout 
      << -b->delta 
      << " in " 
      << b->bal.sym 
      << " to " 
      << pivot.sym 
      << endl;
    xact_limit(
        pivot.sym,
        b->bal.sym,
        b->delta,
        "USDT"
        );
  };
  for( ; b!=e; b++ ) {
    if( b->bal.sym == pivot.sym ) {
      cout << "skip: " << b->bal.sym << " sym 2" << endl;
      continue;
    };
    if( b->delta < 5 ) {
      cout << "skip: " << b->bal.sym << " <$5 2" << endl;
      continue;
    };
    cout 
      << b->delta 
      << " in " 
      << pivot.sym 
      << " to " 
      << b->bal.sym 
      << endl;
    xact_limit(
        pivot.sym,
        b->bal.sym,
        b->delta,
        "USDT"
        );
  };
#endif
  return 0;
};
void show_todos( const todo_v &todo, const money_t &tot ) {
  if(!todo.size()) {
    cerr << "todo is empty" << endl;
    return;
  };
#if 0
  auto const& temp = todo[0];
  cout << "|" << setw( temp.bal.sym.get_width() ) << "SYM "
    << "|" << setw( temp.bal.usd.get_width() ) << "cur$ "
    << "|" << setw( temp.pct_goal.get_width() ) << "cur% "
    << "|" << setw( temp.pct_goal.get_width() ) << "goal% "
    << "|" << setw( temp.bal.usd.get_width() ) << "goal$ "
    << "|" << setw( temp.bal.usd.get_width() ) << "delta "
    << "|" << endl;
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
  };
  for ( auto d : data )
  {
    cout
      << "|" 
      << d.first << "|"
      << d.second.bal.usd << "|"
      << pct_t( d.second.bal.usd / tot ) << "|"
      << pct_goal << "|"
      << usd_goal << "|"
      << delta << "|"
      << "|" << endl;
  };
#endif
};
int main( int argc, char** argv )
{
  try
  {
    arg_v args( argv+1, argv+argc );
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
