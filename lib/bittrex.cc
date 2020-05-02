#include <bittrex.hh>
#include <util.hh>
#include <json.hh>
#include <web_api.hh>
#include <cmath>
#include <dbg.hh>

using namespace coin;
using namespace util;
using namespace fmt;
using nlohmann::detail::value_t;

bool bittrex::fake_buys=true;
bool bittrex::show_urls=true;
const static string api_url = "https://bittrex.com/api/v1.1/";

namespace coin {
  void from_json(const json &j, balance_t &val);
  void from_json(const json &j, money_t &val);
  void from_json(const json &j, market_l &ml);
  void from_json(const json &j, market_t &ml);
  void from_json(const json &j, order_t& val );
  void from_json(const json &j, string &val);
  void from_json(const json &j, bool &val);
};
//#define trace_from_json(x) xtrace(x)
#define trace_from_json(x)
void coin::from_json(const json &j, order_t& o )
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
#define list( item )                                  \
  item(  uuid,                 "OrderUuid"          ) \
//  item(  account_id,           "AccountId"          ) \
  item(  exchange,             "Exchange"           ) \
  item(  type,                 "Type"               ) \
  item(  opened,               "Opened"             ) \
  item(  closed,               "Closed"             ) \
  item(  is_open,              "IsOpen"             ) \
  \
  item(  limit,                "Limit"              ) \
  item(  quantity,             "Quantity"           ) \
  item(  price,                "Price"              ) \
  item(  price_per_unit,       "PricePerUnit"       ) \
  item(  quantity_remaining,   "QuantityRemaining"  ) \
  \
  item(  cancel_initiated,     "CancelInitiated"    ) \
  \
  item(  commission_paid,      "CommissionPaid"     ) \
  item(  commission_remain,    "CommissionReserveRemaining" ) \
  item(  commission_reserved,  "CommissionReserved" ) \
  \
  item(  is_conditional,       "IsConditional"      ) \
  item(  condition,            "Condition"          ) \
  item(  condition_target,     "ConditionTarget"    ) \
  \
  item(  immediate_or_cancel,  "ImmediateOrCancel"  ) \
  item(  reserved,             "QuantityRemaining"  ) \
  item(  sentinel,             "Sentinel"           ) \
  nop()

#define extract( x, y )  coin::from_json(j.at(y),tmp.x);
  order_t::data_t tmp;
  list( extract );
  o=order_t(tmp);
};
void coin::from_json(const json &j, bool &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if(j.is_null())
    val=false;
  else
    j.get_to(val);
};
void coin::from_json(const json &j, string &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if(j.is_null())
    val=string();
  else if(j.is_number())
    val=lexical_cast<string>((double)j);
  else
    j.get_to(val);
};


//   #define display( x, y )                                                   \
//     cout << left << setw( 20 ) << y << right << setw( 20 ) << "*" << o.x    \
//          << endl;
//       list( display );
//     ;
void coin::from_json(const json &j, market_l &ml)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  for( auto it = j.begin(); it != j.end();it++ ) 
  {
    market_t tmp;
    coin::from_json(*it,tmp);
    ml.push_back(tmp);
  };
};
void coin::from_json(const json &j, money_t &val) {
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if( j.is_null() )
    val=0;
  else
    val=(double)j;
};
void coin::from_json(const json &j, balance_t &bal) {
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  using coin::market_l;
  try {
    balance_t res;
    from_json(j.at("Currency"),res.sym);
    from_json(j.at("Balance"),res.bal);
    from_json(j.at("Available"),res.ava);
    from_json(j.at("Pending"),res.pend);
    res.btc=market_t::conv(res.bal,res.sym,"BTC");
    res.usd=market_t::conv(res.btc,"BTC","USD");
    bal=res;
  } catch(exception &ex) {
    cerr << "while reading json: " << setw(4) << j << endl;
    xexpose(ex.what());
    throw;
  };
};
void coin::from_json(const json &j, market_t &m)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  string name=j.at("MarketName");
  money_t bid=j.at("Bid");
  money_t ask=j.at("Ask");
  market_t tmp(name,bid,ask);
  assert(tmp.name()==tmp.cur()+"-"+tmp.sym());
  coin::from_json(j.at("Last"),tmp.data.last);
  coin::from_json(j.at("High"),tmp.data.high);
  coin::from_json(j.at("Low"),tmp.data.low);
  coin::from_json(j.at("Volume"),tmp.data.vol);
  m=tmp;
};
  string bittrex::simple_xact
(
 const market_t &market,
 bool buy,    // buy or sell
 money_t qty, // in the symbol to buy or sell
 money_t rate, // in currency per unit
 bool ioc
 )
{
  string act=(buy?"buy":"sell");
  money_t total=qty*rate*1.002;
  cout 
    << setw(5) << act
    << "  " << qty 
    << "  " << market.sym()
    << " at " << rate << "BTC/unit"
    << " on " << market.data.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;
  const static string sl_url=
    "https://bittrex.com/api/v1.1/market/";
  string url=sl_url+act+"limit?";
  url+="market="+market.data.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(rate));
  if(ioc)
    url+="&timeInForce=IOC";
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys){
    cout << " not gonna do it!" << endl;
    return string();
  };

  string page = web::load_hmac_page(url);
  auto jpage=json::parse(page);
  cout << setw(4) << jpage << endl;
  if(!jpage.at("success")) {
    throw runtime_error( "no success in buylimit result\n\n"+page);
  };
  jpage=jpage.at("result");
  cout << setw(4) << jpage << endl;
  jpage=jpage.at("uuid");
  cout << setw(4) << jpage << endl;
  return jpage;
};
bool bittrex::xact_limit(
    sym_t fsym,
    sym_t tsym,
    money_t qty,
    sym_t qunit,
    bool ioc
    )
{
  assert(qunit=="USD");
  cout << "-- " << fsym << " " << tsym << " " << qty << " " << qunit << endl;
  if( qty < 0 )
    return xact_limit(tsym,fsym,-qty,qunit,ioc);
  while(orders_pending()) {
    cancel_orders();
  };
  assert(qty>0);
  market_l marks=market_t::get(fsym,tsym);
  if(marks.size()==1) {
    market_t market=marks[0];
    cout << market << endl;
    if(market.cur()==fsym && market.sym()==tsym) {
      money_t tqty=market_t::conv2(qty,qunit,tsym);
      xtrace( xnv(tqty) << " | " << xnv(qty) << " | " << xnv(qunit) << " | " << xnv(tsym) );
      xexpose(market.yield(qty,qunit,tsym));
      return buy_limit(market,tsym,tqty,ioc);
    } else if (market.sym()==fsym && market.cur()==tsym) {
      money_t fqty=qty*market_t::conv2(qty,qunit,fsym);
      return sell_limit(market,fsym,fqty,ioc);
    } else {
      xthrow(runtime_error, "fuck");
    };
  } else {
    cout << "got " << marks.size() << " markets for " << fsym << "-" << tsym << endl;
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
  assert(market.data.cur==sym);
  money_t rate(1/market.data.ask);
  cout << "market: " << market << endl;
  money_t total(qty*rate);
  cout 
    << "buy  " << qty << sym
    << " at " << rate
    << " on " << market.data.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;
  const static string sl_url=
    "https://bittrex.com/api/v1.1/market/buylimit?";
  string url=sl_url;
  url+="market="+market.data.name;
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
  assert(market.data.sym==sym);
  money_t rate(market.data.bid);
  cout << "market: " << market << endl;
  money_t total(qty*rate);
  balance_t const &cur_bal=balance_t::get(market.data.cur);
  balance_t const &sym_bal=balance_t::get(market.data.sym);
  cout << "qty; " << qty << endl;
  cout << "bal: " << sym_bal.bal << endl;
  qty=min(qty,sym_bal.bal);
  cout 
    << "sell " << money_t(qty) << sym
    << " at " << money_t(market.data.bid)
    << " on " << market.data.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;

  const static string sl_url=
    "https://bittrex.com/api/v1.1/market/selllimit?";
  string url=sl_url;
  url+="market="+market.data.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(money_t(market.data.bid)));


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
    if(!ofile)
      xthrowre("open:"+fname+strerror(errno));
    stringstream str;
    str << setw(4) << json << endl;
    string text=str.str();
    ofile<<text;
    if(!ofile)
      xthrowre("error writing "+fname);
  };
}
const coin::balance_l bittrex::load_balances()
{
  const static string gb_url=
    "https://bittrex.com/api/v1.1/account/getbalances?";
  cout  << "about to send request" << endl;
  string page = web::load_hmac_page(gb_url);
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getbalances result\n\n"+page
        );
  };
  jpage=*jpage.find("result");
  save_json("balance.json",jpage);
  balance_l temp;
  for( json bal : jpage ) {
    if(bal.at("Currency")=="BTXCRD")
      continue;
    balance_t obj(bal);
    temp.push_back(obj);
  };
  sort(temp.begin(),temp.end());
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
  coin::from_json(jpage,markets);
  cout << markets.size() << " markets loaded" << endl;
  market_l selected;
  for( auto &market : markets )
  {
    if( market.data.cur == "BTC" || market.data.sym=="BTC" )
      selected.push_back(market);
  };
  cout << selected.size() << " markets selected" << endl;
  return selected;
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
  for( auto order : jpage )
    cancel_order(order["OrderUuid"]);
}
bool bittrex::orders_pending() {
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  return jpage.begin() != jpage.end();
};
void bittrex::dump_orders() {
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  string page = web::load_hmac_page(url);
  cout << setw(4) << json::parse(page) << endl;
};

namespace bittrex {
  order_l get_order( const string& uuid );
};
order_l bittrex::get_order( const string& uuid )
{
  const static string url( api_url + "account/getorder?" );
  string my_url=url+"uuid="+uuid+"&";
  if ( show_urls )
  {
    xexpose(my_url);
  };
  string page = web::load_hmac_page( my_url );
  auto jpage = json::parse( page );
  save_json("order.json",jpage);
  jpage = jpage[ "result" ];
  order_l orders;
  order_t order;
  coin::from_json( jpage, order );
  orders.push_back( order );
  return orders;
};
order_l bittrex::get_order_history( const string& msg )
{
  const static string url( api_url + "account/getorderhistory?" );
  if ( show_urls )
  {
    xexpose(url);
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
