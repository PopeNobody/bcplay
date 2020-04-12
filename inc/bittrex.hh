#ifndef bittrex_hh
#define bittrex_hh bittrex_hh
#include <coinfwd.hh>
#include <markets.hh>
#include <balance.hh>
#include <order.hh>

namespace bittrex {
	using namespace coin;
	const market_l load_markets();
	const balance_l &load_balances();

	extern bool fake_buys;
	extern bool show_urls;
	bool xact_limit(
			sym_t fsym,
			sym_t tsym,
			money_t fqty,
			sym_t qunit
			);
	bool dump_orders();	
	void cancel_order(const string &id);	
  bool orders_pending();
	void cancel_orders();	
	void show_deposits();
	void show_withdrawals();
  market_l get_market(const string &id);
  order_l get_order_history(const string &mkt);
};

#endif // bittrex_hh


