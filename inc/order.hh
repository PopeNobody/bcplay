#ifndef order_hh
#define order_hh order_hh

#include <coinfwd.hh>
#include <money.hh>

namespace coin {

//      {
//         "OrderUuid": "89de4465-714e-4f6a-9f82-5b1c70db9c28",
//         "Closed": "2020-04-12T16:35:05.47",
//         "Commission": 0.03141024,
//         "Condition": "",
//         "ConditionTarget": 0.0,
//         "Exchange": "USDT-RVN",
//         "ImmediateOrCancel": false,
//         "IsConditional": false,
//         "Limit": 0.01899911,
//         "OrderType": "LIMIT_BUY",
//         "Price": 31.41024479,
//         "PricePerUnit": 0.0189991,
//         "Quantity": 1653.24909028,
//         "QuantityRemaining": 0.0,
//         "TimeStamp": "2020-04-12T16:35:05.47"
//      },

  struct date_t
  {
    tm val;
  };
	struct order_t 
	{
		string uuid;
    date_t closed;
    money_t comission;
    string condition;
    string condition_target;
    string exchange;
    bool immediate_or_cancel;
    bool is_conditional;
    money_t limit;
    string order_type;
    money_t price;
    money_t price_per_unit;
    size_t quantity;
    size_t quantity_remaining;
    date_t timestamp;
	};
	class order_l : public std::vector<order_t>
	{
		using std::vector<order_t>::vector;
	};
};

#endif

