#include <bittrex.hh>
#include <util.hh>
#include <json.hh>
#include <web_api.hh>
#include <cmath>

using namespace coin;
using namespace util;
using namespace fmt;

bool bittrex::fake_buys=true;
bool bittrex::show_urls=true;
const static string api_url="https://bittrex.com/api/v1.1/";

namespace coin {
  void from_json(json &j, order_t &o)
  {
    o.uuid=j.at("OrderUuid");
    o.closed=j.at("Closed");
    o.comission=j.at("Commission");
    o.condition=j.at("Condition");
    o.condition_target=j.at("ConditionTarget");
    o.exchange=j.at("Exchange");
    o.immediate_or_cancel=j.at("ImmediateOrCancel");
    o.is_conditional=j.at("IsConditional");
    o.limit=j.at("Limit");
    o.order_type=j.at("OrderType");
    o.price=j.at("Price");
    o.price_per_unit=j.at("PricePerUnit");
    o.quantity=j.at("Quantity");
    o.quantity_remaining=j.at("QuantityRemaining");
    o.timestamp=j.at("TimeStamp");
//  "OrderUuid":           "89de4465-714e-4f6a-9f82-5b1c70db9c28",
//  o.uuid                 =j.at("OrderUuid");
//  "Closed":              "2020-04-12T16:35:05.47",
//  o.closed               =j.at("Closed");
//  "Commission":          0.03141024,
//  o.comission            =j.at("Commission");
//  "Condition":           "",
//  o.condition            =j.at("Condition");
//  "ConditionTarget":     0.0,
//  o.condition_target     =j.at("ConditionTarget");
//  "Exchange":            "USDT-RVN",
//  o.exchange             =j.at("Exchange");
//  "ImmediateOrCancel":   false,
//  o.immediate_or_cancel  =j.at("ImmediateOrCancel");
//  "IsConditional":       false,
//  o.is_conditional       =j.at("IsConditional");
//  "Limit":               0.01899911,
//  o.limit                =j.at("Limit");
//  "OrderType":           "LIMIT_BUY",
//  o.order_type           =j.at("OrderType");
//  "Price":               31.41024479,
//  o.price                =j.at("Price");
//  "PricePerUnit":        0.0189991,
//  o.price_per_unit       =j.at("PricePerUnit");
//  "Quantity":            1653.24909028,
//  o.quantity             =j.at("Quantity");
//  "QuantityRemaining":   0.0,
//  o.quantity_remaining   =j.at("QuantityRemaining");
//  "TimeStamp":           "2020-04-12T16:35:05.47"
//  o.timestamp            =j.at("TimeStamp");
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
    val=0;
    if( !j.is_null() )
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
    res.usd=res.ava*markets.conv(res.sym,"USDT");
    res.btc=res.ava*markets.conv(res.sym,"BTC");
    bal=res;
  };
};
namespace bittrex {
  bool buy_limit(
      const coin::market_t &market,
      sym_t sym,
      coin::money_t tqty
      );
  bool sell_limit(
      const coin::market_t &market,
      sym_t sym,
      coin::money_t fqty
      );
};
bool bittrex::xact_limit(
    sym_t fsym,
    sym_t tsym,
    money_t qty,
    sym_t qunit
    )
{
  if( qty < 0 )
    return xact_limit(tsym,fsym,-qty,qunit);
  assert(qty>0);
  cout
    << "trade " << qty << qunit
    << " worth of " << fsym
    << " for " << tsym
    << endl;
  market_l marks=market_l::get_conv(fsym,tsym);
  if(marks.size()==1) {
    market_t market=marks[0];
    cout << "  " << (market.buy?"buy":"sell") << " on market " << market << endl;
    if(market.buy) {
      money_t tqty=qty*market_l::conv(qunit,tsym);
      cout << tqty << endl;
      return buy_limit(market,tsym,tqty);
    } else {
      money_t fqty=qty*market_l::conv(qunit,fsym);
      cout << fqty << endl;
      return sell_limit(market,fsym,fqty);
    };
  } else {
    cout << "gonna die" << endl;
    cout << "  fsym: " << fsym << endl;
    cout << "  tsym: " << tsym << endl;
    cout << "  qty:  " << qty << endl;
    cout << "  qunit: " << qunit << endl;
    cout << marks << endl;
    assert(marks.size()==1);
    abort();
  };
};
bool bittrex::buy_limit(
    const market_t &market,
    sym_t sym,
    coin::money_t qty
    )
{
  money_t rate(1/market.ask);
  assert(market.t_coin==sym);

  const static string sl_url=
    api_url+"market/buylimit?";

  string url=sl_url;
  url+="market="+market.name;
  url+="&quantity="+strip(lexical_cast<string>(qty.get()));
  url+="&rate="+strip(lexical_cast<string>(rate.get()));
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys){
    cout << " not gonna do it!" << endl;
    return true;
  };

  string page = web::load_hmac_page(url);
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
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
    coin::money_t qty
    )
{
  assert(market.f_coin==sym);
  money_t rate=market.bid;

  const static string sl_url=
    api_url+"market/selllimit?";

  string url=sl_url;
  url+="market="+market.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(rate));
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys){
    cout << " not gonna do it!" << endl;
    return true;
  };

  string page = web::load_hmac_page(url);
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
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
    api_url+"account/getwithdrawalhistory?";
  string page = web::load_hmac_page(gw_url);
  //	cout << string('-',20) << endl;
  cout << "--------------" << endl << page << endl << endl;
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getbalances result\n\n"+page
        );
  };
  cout << endl << endl;
  cout << setw(4) << jpage;
  cout << endl << endl;
};
void bittrex::show_deposits() {
  cout << "dep" << endl;
  const static string gd_url=
    api_url+"account/getdeposithistory?";
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
      cerr << "writing to file" << endl;
      ofile << setw(4) << json << endl;
      if(!ofile) {
        cerr << "unable to save json!";
      }
    }
  };
}
const coin::balance_l &bittrex::load_balances()
{
  coin::balance_l temp;
  const static string gb_url=
    api_url+"account/getbalances?";
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
  static coin::balance_l res;
  swap(res,temp);
  return res;
};
market_l bittrex::get_market(const string &id) {
  const static string gms_url=
    api_url+"public/getmarketsummary?market=";
  string url = gms_url+id;
  if(show_urls)
    cout << "url=" << url << endl;
  json jpage = json::parse(web::load_page(url));
  cout << setw(4) << jpage << endl;

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
  market_l markets;
  from_json(jpage,markets);
  return markets;
};
const market_l bittrex::load_markets() {
  const static string gms_url=
    api_url+"public/getmarketsummaries";
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
  const static string b_url(api_url+"market/cancel?");
  string url = b_url+"uuid="+id+"&";
  string page = web::load_hmac_page(url);
  cout << "res: " << endl;
  cout << page << endl;
  cout << endl;
};
void bittrex::cancel_orders() {
  cout << __PRETTY_FUNCTION__ << endl;
  const static string url(api_url+"market/getopenorders?");
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
  const static string url(api_url+"market/getopenorders?");
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  auto res = ( jpage.begin() != jpage.end() );
  return res;
};
bool bittrex::dump_orders() {
  const static string url(api_url+"market/getopenorders?");
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  auto res = ( jpage.begin() != jpage.end() );
  if(res)
    cout << setw(4) << jpage << endl << endl;
  return res;
};
order_l bittrex::get_order_history(const string &msg)
{
  const static string url(api_url+"account/getorderhistory?");
  if(show_urls){
    cout << "url: " << url << endl;
  };
  string page = web::load_hmac_page(url);
  auto jpage = json::parse(page);
  jpage = jpage["result"];
  order_l orders;
  return orders;
};

