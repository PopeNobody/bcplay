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
	bool xact_limit(
			sym_t fsym,
			sym_t tsym,
			money_t fqty,
			sym_t qunit
			);
	void dump_orders();	
	void show_deposits();
	void show_withdrawals();
};

#endif // bittrex_hh


