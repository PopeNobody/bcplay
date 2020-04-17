#include <bittrex.hh>
#include <fmt.hh>
#include <markets.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using coin::market_l;
using coin::market_t;

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

  res[ "BCH" ] = 1;
  res[ "BSV" ] = 1;
  res[ "BTC" ] = 1;
  res[ "USDT" ] = 1;

  double tot = 0;
  for ( auto goal : res )
  {
    tot = tot + (double)goal.second;
  };
  for ( auto& goal : res )
  {
    goal.second = (double)goal.second / tot;
  };
  for ( auto& goal : res )
  {
    cout << goal.first << " " << goal.second << endl;
  };
  return res;
};
auto const& goals = mk_goals();

using bittrex::xact_limit;

inline money_t abs( money_t rhs )
{
  return ( rhs < 0 ) ? -rhs : rhs;
};

int xmain( int argc, char** argv )
{
  struct coin_data
  {
    balance_t bal;
    pct_t goal;
  };
  std::map< sym_t, coin_data > data;
  for ( auto g : goals )
  {
    data[ g.first ].bal.sym = g.first;
    data[ g.first ].goal = g.second;
  };
  money_t tot = 0.0000001;
  for ( auto b : balance_l::load_balances() )
  {
    if ( goals.find( b.sym ) != goals.end() )
    {
      ;
    }
    else if ( b.usd > 0.01 )
    {
      ;
    }
    else
    {
      continue;
    };
    data[ b.sym ].bal = b;
    tot += b.usd;
  }
  cout << "tot: " << tot << "USD" << endl;
  {
    auto const& temp = data[ "BTC" ];
    cout << "|" << setw( temp.bal.sym.get_width() ) << "SYM "
         << "|" << setw( temp.bal.usd.get_width() ) << "cur$ "
         << "|" << setw( temp.goal.get_width() ) << "cur% "
         << "|" << setw( temp.goal.get_width() ) << "goal% "
         << "|" << setw( temp.bal.usd.get_width() ) << "goal$ "
         << "|" << endl;
    for ( auto d : data )
    {
      pct_t per_goal( d.second.goal.get() );
      money_t usd_goal = tot * per_goal.get();
      d.second.bal.sym = d.first;
      cout << "|" << d.first << "|" << d.second.bal.usd << "|"
           << pct_t( d.second.bal.usd / tot ) << "|" << per_goal << "|"
           << usd_goal << "|" << endl;
    };
  };
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    if ( xmain( argc, argv ) )
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
