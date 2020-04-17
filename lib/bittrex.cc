#include <bittrex.hh>
#include <cmath>
#include <json.hh>
#include <order.hh>
#include <regex>
#include <util.hh>
#include <web_api.hh>
#include <boost/date_time/posix_time/ptime.hpp>

#undef checkin
#define checkin()

using boost::gregorian::from_simple_string;
using boost::gregorian::date;
using boost::date_time::Feb;
using boost::date_time::not_a_date_time;
using boost::posix_time::time_duration;
using boost::posix_time::ptime;
using boost::local_time::local_date_time;

using namespace coin;
using namespace util;
using namespace fmt;
using namespace nlohmann;
using namespace nlohmann::detail;

bool bittrex::fake_buys = true;
bool bittrex::show_urls = true;
bool bittrex::keep_all = true;
const static string api_url = "https://bittrex.com/api/v1.1/";

  //using boost::posix_time::ptime;
namespace boost
{
  namespace posix_time
  {
    void from_json( const json& j, ptime& val )
    {
      checkin();
      cout << j << endl;
    };
  };
};   // namespace boost
namespace coin
{
  void from_json( const json& j, money_t& val )
  {
    checkin();
    val = 0;
    if ( !j.is_null() )
      val = (double)j;
  };
  void from_json( json& j, order_t& o )
  {
    checkin();
    cout << setw(4) << j << endl;
#define list( item )                                                       \
  item(  uuid,                 "OrderUuid"          ) \
  item(  closed,               "Closed"             ) \
  item(  comission,            "Commission"         ) \
  item(  condition,            "Condition"          ) \
  item(  condition_target,     "ConditionTarget"    ) \
  item(  exchange,             "Exchange"           ) \
  item(  immediate_or_cancel,  "ImmediateOrCancel"  ) \
  item(  is_conditional,       "IsConditional"      ) \
  item(  limit,                "Limit"              ) \
  item(  order_type,           "OrderType"          ) \
  item(  price,                "Price"              ) \
  item(  price_per_unit,       "PricePerUnit"       ) \
  item(  quantity,             "Quantity"           ) \
  item(  quantity_remaining,   "QuantityRemaining"  ) \
  item(  timestamp,            "TimeStamp"          )
#define extract( x, y ) o.x = j.at( y );
    list( extract );


#define display( x, y )                                                   \
  cout << left << setw( 20 ) << y << right << setw( 20 ) << "*" << o.x    \
       << endl;
    list( display );
  };
  void from_json( json& j, market_t& m )
  {
    checkin();
    m.name = j.at( "MarketName" );
    make_tuple(m.f_coin,m.t_coin)=bittrex::split_name(m.name);
#define decode( x, y )                                                    \
  if ( j.at( #x ).is_null() )                                             \
  {                                                                       \
    m.y = 0;                                                              \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    m.y = j.at( #x );                                                     \
  }
#define do_decode( x, y )                                                 \
  do                                                                      \
  {                                                                       \
    decode( x, y )                                                        \
  } while ( 0 )
    do_decode( Last, last );
    do_decode( Bid, bid );
    do_decode( Ask, ask );
    do_decode( High, high );
    do_decode( Low, low );
    do_decode( Volume, vol );
  };
  static vector<string> keep_list = { "BCH", "BSV", "DASH", "XMR", "ZEN", "XRP", "USDT", "XLM", "RVN","BTC", "USD" };
  void from_json( json& j, market_l& ml )
  {
    checkin();
    for ( auto it = j.begin(); it != j.end(); it++ )
    {
      market_t fwd;
      from_json( *it, fwd );
      if( !bittrex::keep_all ) {
        if( !contains(keep_list,fwd.t_coin) )
          continue;
        if( !contains(keep_list,fwd.f_coin) )
          continue;
      };

      if ( isinf( fwd.last.get() ) ) {
        cout << "isinf(" << fwd.name << ")" << endl;
        continue;
      };

      ml.push_back( fwd );
    };
  };
  void from_json( const json& j, balance_t& bal )
  {
    checkin();
    balance_t res;
    res.sym = j.at( "Currency" );
    res.bal = j.at( "Balance" );
    res.pend = j.at( "Pending" );
    res.btc = res.bal * bittrex::ex_rate( "BTC", res.sym );
    res.usd = res.bal * bittrex::ex_rate( "USDT", res.sym );
    if(!res.usd)
      res.usd=res.btc * bittrex::ex_rate( "USDT", "BTC" );
    bal = res;
  };
}; // namespace coin
namespace bittrex
{
  money_t ex_rate( sym_t f_sym, sym_t t_sym )
  {
    if(f_sym == t_sym)
      return 1;
    market_l conv=market_l::get_conv(f_sym,t_sym);
    if( !conv.size() )
      return 0;
    return conv[0].last;
  };
  bool is_trading_pair( sym_t lhs, sym_t rhs )
  {
    return market_l::get_conv(lhs,rhs).size()==1;
  };
  void buy_limit( const coin::market_t& market,
                  sym_t sym,
                  coin::money_t tqty );
  void sell_limit( const coin::market_t& market,
                   sym_t sym,
                   coin::money_t fqty );
  pair<string,string> split_name(const string &name) {
    auto b(begin(name)), e(end(name));
    while(b!=e && isalnum(*b))
      ++b;
    if(b==e)
      throw runtime_error("bad name: "+name+" missing dash");
    auto dash=b;
    if(*b!='-')
      throw runtime_error("bad name: "+name+" non-alpha,non-numeric,non-dask");
    while(b!=e && isalnum(*++b))
      ;
    if(b!=e)
      throw runtime_error("bad name: "+name+" bad characters at end" );
    return make_pair(string(name.begin(),dash),string(dash+1,name.end()));
  };
//       
//       if(pos==name.end())
//       auto words = split('-',name);
//       assert(words.size()==2);
//       const auto b(name.begin()), e(name.end());
//       auto s(find(b,e,'-'));
//       f_coin.assign(b,s);
//       t_coin.assign(++s,e);
//       assert(f_coin.length()>1);
//       assert(t_coin.length()>1);
} // namespace bittrex
void bittrex::xact_limit( sym_t fsym,
    sym_t tsym,
    money_t qty,
    sym_t qunit )
{
  assert(qunit=="USDT");
  if( qty < 0 ) {
    cout << "forcing qty positive." << endl;
    swap(fsym,tsym);
    qty=-qty;
  }
  if( qty > money_t(50) ) {
    cout << "limiting risk" << endl;
    qty=50;
  };
  cout
    << "I want to get out of " << fsym
    << " and into " << tsym << " to the tune of "
    << qty << " " << qunit << endl << endl;
  market_l marks = market_l::get_conv( fsym, tsym );
  if( marks.size() != 1 ) {
    cout << "But there is no market for that!" << endl;
    cout << marks << endl;
    abort();
  };
  market_t mark = marks[0];
  cout << "using market: " << mark.name << endl;
  money_t fqty=qty/ex_rate(qunit,fsym);
  money_t tqty=qty/ex_rate(qunit,tsym);
  if ( isinf( tqty.get() ) ) {
    cout << "tqty isinf" << endl;
    cout << fsym << " -> " << tsym << endl;
    cout << qty << endl;
    cout << ex_rate(qunit,fsym) << endl;
    cout << ex_rate(qunit,tsym) << endl;
    cout << mark << endl;
    return;
  };
  if ( isinf( fqty.get() ) ) {
    cout << "fqty isinf" << endl;
    cout << fsym << " -> " << tsym << endl;
    cout << qty << endl;
    cout << ex_rate(qunit,fsym) << endl;
    cout << ex_rate(qunit,tsym) << endl;
    cout << mark << endl;
    return;
  };
  cout
    << mark << endl << endl
    << "The market treats " << mark.f_coin << " as money, and " << endl
    << mark.t_coin << " as product.  Since I want to yeet " << fsym
    << " and gain " << tsym << ", this is a " << endl;
  if( fsym == mark.f_coin ) {
    cout
      << "buy transavtion." << endl << endl;
    cout
      << "We will specify the price in " << mark.f_coin
      << endl
      << "We will specify the quantity in " << mark.t_coin
      << "and it works out to " << tqty << mark.t_coin << endl;
    
    cout
      << "For this we expect to spend " << fqty << mark.f_coin
      << endl;

    buy_limit( mark, tsym, tqty );

  } else {
    cout << "sell transaction." << endl;
    cout
      << "We will specify the quantity in " << mark.t_coin
      << "and it works out to " << fqty << mark.t_coin << endl;

    cout
      << "For this we expect to spend " << tqty << mark.f_coin
      << endl;

    sell_limit( mark, fsym, fqty );
  };
};
void bittrex::buy_limit( const market_t& market,
                         sym_t sym,
                         coin::money_t qty )
{
  money_t rate( market.last );
  if(!rate) {
    rate=(market.bid+market.ask)/2;
  };
  assert( market.t_coin == sym );

  const static string sl_url = api_url + "market/buylimit?";

  string url = sl_url;
  url += "market=" + market.name;
  url += "&quantity=" + strip( lexical_cast< string >( qty ) );
  url += "&rate=" + strip( lexical_cast< string >( rate ) );
  url += "&";
  if ( show_urls )
    cout << "url: " << url << endl;
  if ( fake_buys )
  {
    cout << " not gonna do it!" << endl;
    return;
  };

  string page = web::load_hmac_page( url );
  auto jpage = json::parse( page );
  if ( !jpage.at( "success" ) )
    throw runtime_error( "no success in buylimit result\n\n" + page );
  return;
};
void bittrex::sell_limit( const market_t& market,
                          sym_t sym,
                          coin::money_t qty )
{
  assert( market.t_coin == sym );
  money_t rate = market.last;
  if(!rate) {
    rate=(market.bid+market.ask)/2;
  };

  const static string sl_url = api_url + "market/selllimit?";

  string url = sl_url;
  url += "market=" + market.name;
  url += "&quantity=" + strip( lexical_cast< string >( qty ) );
  url += "&rate=" + strip( lexical_cast< string >( rate ) );
  url += "&";
  if ( show_urls )
    cout << "url: " << url << endl;
  if ( fake_buys )
  {
    cout << " not gonna do it!" << endl;
    return;
  };

  string page = web::load_hmac_page( url );
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
  auto jpage = json::parse( page );
  cout << setw( 4 ) << jpage << endl;
  if ( !jpage.at( "success" ) )
  {
    throw runtime_error( "no success in selllimit result\n\n" + page );
  };
  return;
};

void bittrex::show_withdrawals()
{
  cout << "with" << endl;
  const static string gw_url = api_url + "account/getwithdrawalhistory?";
  string page = web::load_hmac_page( gw_url );
  //	cout << string('-',20) << endl;
  cout << "--------------" << endl << page << endl << endl;
  json jpage = json::parse( page );
  if ( !jpage.at( "success" ) )
  {
    throw runtime_error( "no success in getbalances result\n\n" + page );
  };
  cout << endl << endl;
  cout << setw( 4 ) << jpage;
  cout << endl << endl;
};
void bittrex::show_deposits()
{
  cout << "dep" << endl;
  const static string gd_url = api_url + "account/getdeposithistory?";
  string page = web::load_hmac_page( gd_url );
  //	cout << string('*',20) << endl;
  json jpage = json::parse( page );
  if ( !jpage.at( "success" ) )
  {
    throw runtime_error( "no success in getdeposithistory result\n\n" +
                         page );
  };
  cout << endl << endl;
  cout << setw( 4 ) << jpage;
  cout << endl << endl;
};
namespace
{
  void save_json( const string& fname, const json& json )
  {
    assert( fname.length() );
    ofstream ofile( fname.c_str() );
    if ( !ofile )
    {
      cerr << "unable to save json!";
    }
    else
    {
      cerr << "writing to file" << endl;
      ofile << setw( 4 ) << json << endl;
      if ( !ofile )
      {
        cerr << "unable to save json!";
      }
    }
  };
} // namespace
const coin::balance_l bittrex::load_balances()
{
  coin::balance_l res;
  const static string gb_url = api_url + "account/getbalances?";
  string page = web::load_hmac_page( gb_url );
  json jpage = json::parse( page );
  if ( !jpage.at( "success" ) )
  {
    throw runtime_error( "no success in getbalances result\n\n" + page );
  };
  jpage = *jpage.find( "result" );
  save_json( "balance.json", jpage );
  for ( json bal : jpage )
    res.push_back( balance_t( bal ) );
  sort( res.begin(), res.end() );
  return res;
};
const market_l bittrex::load_markets()
{
  const static string gms_url = api_url + "public/getmarketsummaries";
  json jpage = json::parse( web::load_page( gms_url ) );


  assert( jpage.type() == value_t::object );
  auto it = jpage.find( "success" );
  if ( it != jpage.end() )
  {
    if ( !it.value() )
    {
      throw runtime_error( "no success in page" );
    }
  };
  auto res_it = jpage.find( "result" );
  if ( res_it == jpage.end() )
  {
    throw runtime_error( "no result in page" );
  };
  jpage = *res_it;
  save_json( "markets.json", jpage );
  market_l markets;
  coin::from_json( jpage, markets );
  sort(markets);
  //cout << markets << endl;
  return markets;
};
void bittrex::cancel_order( const string& id )
{
  const static string b_url( api_url + "market/cancel?" );
  string url = b_url + "uuid=" + id + "&";
  string page = web::load_hmac_page( url );
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
};
void bittrex::cancel_orders()
{
  cout << __PRETTY_FUNCTION__ << endl;
  const static string url( api_url + "market/getopenorders?" );
  string page = web::load_hmac_page( url );
  auto jpage = json::parse( page );
  jpage = jpage[ "result" ];
  int onum = 0;
  for ( auto order : jpage )
  {
    cout << onum << endl;
    cout << setw( 4 ) << order << endl;
    cout << endl;
    cancel_order( order[ "OrderUuid" ] );
  };
}
bool bittrex::orders_pending()
{
  const static string url( api_url + "market/getopenorders?" );
  string page = web::load_hmac_page( url );
  auto jpage = json::parse( page );
  jpage = jpage[ "result" ];
  auto res = ( jpage.begin() != jpage.end() );
  return res;
};
bool bittrex::dump_orders()
{
  const static string url( api_url + "market/getopenorders?" );
  string page = web::load_hmac_page( url );
  auto jpage = json::parse( page );
  jpage = jpage[ "result" ];
  auto res = ( jpage.begin() != jpage.end() );
  if ( res )
    cout << setw( 4 ) << jpage << endl << endl;
  return res;
};
order_l bittrex::get_order_history( const string& msg )
{
  const static string url( api_url + "account/getorderhistory?" );
  if ( show_urls )
  {
    cout << "url: " << url << endl;
  };
  string page = web::load_hmac_page( url );
  auto jpage = json::parse( page );
  jpage = jpage[ "result" ];
  order_l orders;
  for ( auto b( begin( jpage ) ), e( end( jpage ) ); b != e; b++ )
  {
    order_t order;
    coin::from_json( *b, order );
    orders.push_back( order );
  };
  return orders;
};
