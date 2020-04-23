#include <bittrex.hh>
#include <util.hh>
#include <json.hh>
#include <web_api.hh>
#include <cmath>
#include <dbg.hh>

using namespace coin;
using namespace util;
using namespace fmt;

bool bittrex::fake_buys=true;
bool bittrex::show_urls=false;
const static string api_url = "https://bittrex.com/api/v1.1/";

namespace coin {
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
#define extract( x, y )  cout << y << endl; o.x = j.at( y );
    list( extract );


#define display( x, y )                                                   \
  cout << left << setw( 20 ) << y << right << setw( 20 ) << "*" << o.x    \
       << endl;
    list( display );
  };
  void from_json(json &j, market_t &m)
  {
    m.name=j.at("MarketName");
    if( !market_t::split_name(m.name,m.t_coin,m.f_coin) )
      throw runtime_error("split_name failed");
#define decode(x,y) if(j.at(#x).is_null()){m.y=0;}else{m.y=j.at(#x);}
#define do_decode(x,y) do{ decode(x,y) }while(0)
    do_decode(Last,last);
    do_decode(Bid,bid);
    do_decode(Ask,ask);
    do_decode(High,high);
    do_decode(Low,low);
    do_decode(Volume,vol);
    m.buy=false;
  };
  void from_json(json &j, market_l &ml)
  {
    for( auto it = j.begin(); it != j.end();it++ ) 
    {
      market_t fwd;
      from_json(*it,fwd);
      market_t rev=fwd.reverse();
      if(isinf(fwd.last.get()))
        continue;
      if(isinf(rev.last.get()))
        continue;
      ml.push_back(fwd);
      ml.push_back(rev);
    };
  };
  void from_json(const json &j, money_t &val) {
    if( j.is_null() )
      val=0;
    else
      val=(double)j;
  };
  void from_json(const json &j, balance_t &bal) {
    using coin::market_l;
    auto const &markets=market_l::get_markets();

    balance_t res;
    res.sym=j.at("Currency");
    res.bal=j.at("Balance");
    res.ava=j.at("Available");
    res.pend=j.at("Pending");
    res.usd=res.bal*markets.conv(res.sym,"USDT");
    res.btc=res.bal*markets.conv(res.sym,"BTC");
    bal=res;
  };
};
namespace bittrex {
  bool buy_limit(
      const coin::market_t &market,
      sym_t sym,
      coin::money_t tqty,
      bool ioc
      );
  bool sell_limit(
      const coin::market_t &market,
      sym_t sym,
      coin::money_t fqty,
      bool ioc
      );
};
bool bittrex::xact_limit(
    sym_t fsym,
    sym_t tsym,
    money_t qty,
    sym_t qunit,
    bool ioc
    )
{
  assert(qunit=="USDT");
  cout << "-- " << fsym << " " << tsym << " " << qty << " " << qunit << endl;
  if( qty < 0 )
    return xact_limit(tsym,fsym,-qty,qunit,ioc);
  while(orders_pending()) {
    cancel_orders();
  };
  assert(qty>0);
  market_l marks=market_l::get_conv(fsym,tsym);
  if(marks.size()==1) {
    market_t market=marks[0];
    if(market.buy) {
      money_t tqty=qty*market_l::conv(qunit,tsym);
      return buy_limit(market,tsym,tqty,ioc);
    } else {
      money_t fqty=qty*market_l::conv(qunit,fsym);
      return sell_limit(market,fsym,fqty,ioc);
    };
  } else {
    assert(marks.size()==1);
    abort();
  };
};
bool bittrex::buy_limit(
    const market_t &market,
    sym_t sym,
    coin::money_t qty,
    bool ioc
    )
{
  assert(market.t_coin==sym);
  money_t rate(1/market.ask);
  cout << "market: " << market << endl;
  money_t total(qty*rate);
  cout 
    << "buy  " << qty << sym
    << " at " << rate
    << " on " << market.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;
  const static string sl_url=
    "https://bittrex.com/api/v1.1/market/buylimit?";
  string url=sl_url;
  url+="market="+market.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(rate));
  if(ioc)
    url+="&timeInForce=IOC";
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys){
    cout << " not gonna do it!" << endl;
    return true;
  };

  string page = web::load_hmac_page(url);
  auto jpage=json::parse(page);
  cout << setw(4) << jpage << endl;
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in buylimit result\n\n"+page
        );
  };
  return true;
};
bool bittrex::sell_limit(
    const market_t &market,
    sym_t sym,
    coin::money_t qty,
    bool ioc
    )
{
  assert(market.f_coin==sym);
  money_t rate(market.bid);
  cout << "market: " << market << endl;
  money_t total(qty*rate);
  cout 
    << "sell " << money_t(qty) << sym
    << " at " << money_t(market.bid)
    << " on " << market.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;
  const static string sl_url=
    "https://bittrex.com/api/v1.1/market/selllimit?";
  string url=sl_url;
  url+="market="+market.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(money_t(market.bid)));
  if( ioc )
    url+="&timeInForce=IOC";
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys){
    cout << " not gonna do it!" << endl;
    return true;
  };

  string page = web::load_hmac_page(url);
  auto jpage=json::parse(page);
  cout << setw(4) << jpage << endl;
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in selllimit result\n\n"+page
        );
  };
  return true;
};

void bittrex::show_withdrawals() {
  cout << "with" << endl;
  const static string gw_url=
    "https://bittrex.com/api/v1.1/account/getwithdrawalhistory?";
  string page = web::load_hmac_page(gw_url);
  //	cout << string('-',20) << endl;
  cout << "--------------" << endl << page << endl << endl;
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getwithdrawalhistory result\n\n"+page
        );
  };
  cout << endl << endl;
  cout << setw(4) << jpage;
  cout << endl << endl;
};
void bittrex::show_deposits() {
  cout << "dep" << endl;
  const static string gd_url=
    "https://bittrex.com/api/v1.1/account/getdeposithistory?";
  string page = web::load_hmac_page(gd_url);
  //	cout << string('*',20) << endl;
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getdeposithistory result\n\n"+page
        );
  };
  cout << endl << endl;
  cout << setw(4) << jpage;
  cout << endl << endl;
};
namespace {
  void save_json(const string &fname, const json &json) {
    assert(fname.length());
    ofstream ofile(fname.c_str());
    if(!ofile) {
      cerr << "unable to save json!";
    } else {
      ofile << setw(4) << json << endl;
      if(!ofile) {
        cerr << "unable to save json!";
      }
    }
  };
}
const coin::balance_l bittrex::load_balances()
{
  balance_l temp;
  const static string gb_url=
    "https://bittrex.com/api/v1.1/account/getbalances?";
  string page = web::load_hmac_page(gb_url);
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getbalances result\n\n"+page
        );
  };
  jpage=*jpage.find("result");
  save_json("balance.json",jpage);
  for( json bal : jpage ) {
    temp.push_back(balance_t(bal));
  };
  sort(temp.begin(),temp.end());
  //cout << "read " << temp.size() << " balances" << nl;
  return balance_l(temp.rbegin(),temp.rend());
};
const market_l bittrex::load_markets() {
  const static string gms_url=
    "https://bittrex.com/api/v1.1/public/getmarketsummaries";
  json jpage = json::parse(web::load_page(gms_url));


  assert(jpage.type() == value_t::object);
  auto it=jpage.find("success");
  if( it != jpage.end() ) {
    if( !it.value() ) {
      throw runtime_error("no success in page");
    }
  };
  auto res_it=jpage.find("result");
  if( res_it == jpage.end() ) {
    throw runtime_error("no result in page");
  };
  jpage=*res_it;
  save_json("markets.json",jpage);
  market_l markets;
  from_json(jpage,markets);
  return markets;
};
void bittrex::cancel_order(const string &id)
{
  const static string b_url("https://bittrex.com/api/v1.1/market/cancel?");
  string url = b_url+"uuid="+id+"&";
  string page = web::load_hmac_page(url);
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
};
void bittrex::cancel_orders() {
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  int onum=0;
  for( auto order : jpage ) {
    cout << onum << endl;
    cout << setw(4) << order << endl;
    cout << endl;
    cancel_order(order["OrderUuid"]);
  };
}
bool bittrex::orders_pending() {
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  auto res = ( jpage.begin() != jpage.end() );
  if(res)
    cout << "orders pending" << endl;
  else
    cout << "no orders pending" << endl;
  return res;
};
void bittrex::dump_orders() {
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  string page = web::load_hmac_page(url);
  cout << setw(4) << json::parse(page) << endl;
};

order_t::~order_t() {
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
  save_json("orders.json",jpage);
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
