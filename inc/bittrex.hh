#ifndef bittrex_hh
#define bittrex_hh bittrex_hh
#include <coinfwd.hh>
#include <markets.hh>
#include <balance.hh>
#include <order.hh>

namespace bittrex {
	using namespace coin;
	const market_l load_markets();
	const balance_l load_balances();
	const order_l load_orders();

	extern bool fake_buys;
	extern bool show_urls;
  extern bool keep_all;
	bool xact_limit(
			sym_t fsym,
			sym_t tsym,
			money_t fqty,
			sym_t qunit,
      bool ioc
			);
  order_l get_order_history(const string &mkt);
	void dump_orders();	
	void cancel_order(const string &id);	
        bool orders_pending();
	void cancel_orders();	
  bool orders_pending();
	void show_deposits();
	void show_withdrawals();
	const market_l load_markets();
	const balance_l load_balances();

  // splits a market name into the currency and product coin names
  bool split_name(const string &name, string &f_coin, string &t_coin);
  pair<string,string> split_name(const string &name);
  money_t ex_rate( sym_t f_sym, sym_t t_sym );
  bool is_trading_pair( sym_t lhs, sym_t rhs );
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
  string simple_xact
    (
     const market_t &market,
     bool buy,    // buy or sell
     money_t qty, // in the symbol to buy or sell
     money_t rate, // in currency per unit
     bool ioc
    );
};

#endif // bittrex_hh


