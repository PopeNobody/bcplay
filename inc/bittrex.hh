#ifndef bittrex_hh
#define bittrex_hh bittrex_hh
#include <coinfwd.hh>
#include <markets.hh>
#include <balance.hh>
#include <order.hh>

namespace bittrex {
	using namespace coin;
//   	const market_l load_markets();
//   	const balance_l &load_balances();

	extern bool fake_buys;
	extern bool show_urls;
	void xact_limit(
			sym_t fsym,
			sym_t tsym,
			money_t fqty,
			sym_t qunit
			);

  order_l get_order_history(const string &mkt);
	bool dump_orders();	
	void cancel_order(const string &id);	
	void cancel_orders();	
  bool orders_pending();

	void show_deposits();
	void show_withdrawals();

	const market_l load_markets();
	const balance_l load_balances();

  money_t ex_rate( sym_t f_sym, sym_t t_sym );
  bool is_trading_pair( sym_t lhs, sym_t rhs );
};

#endif // bittrex_hh


