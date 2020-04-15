#ifndef order_hh
#define order_hh order_hh

#include <coinfwd.hh>
#include <money.hh>
#include <boost/date_time.hpp>

namespace coin {

	struct order_t 
	{
		string uuid;
    string closed;
    money_t comission;
    string condition;
    money_t condition_target;
    string exchange;
    bool immediate_or_cancel;
    bool is_conditional;
    money_t limit;
    string order_type;
    money_t price;
    money_t price_per_unit;
    size_t quantity;
    size_t quantity_remaining;
    string timestamp;
	};
	class order_l : public std::vector<order_t>
	{
		using std::vector<order_t>::vector;
	};
};

#endif

