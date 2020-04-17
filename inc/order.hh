#ifndef order_hh
#define order_hh order_hh

#include <coinfwd.hh>
#include <money.hh>

namespace coin {

//     "result" : [{
//   			"Uuid" : null,
//   			"OrderUuid" : "09aa5bb6-8232-41aa-9b78-a5a1093e0211",
//   			"Exchange" : "BTC-LTC",
//   			"OrderType" : "LIMIT_SELL",
//   			"Quantity" : 5.00000000,
//   			"QuantityRemaining" : 5.00000000,
//   			"Limit" : 2.00000000,
//   			"CommissionPaid" : 0.00000000,
//   			"Price" : 0.00000000,
//   			"PricePerUnit" : null,
//   			"Opened" : "2014-07-09T03:55:48.77",
//   			"Closed" : null,
//   			"CancelInitiated" : false,
//   			"ImmediateOrCancel" : false,
//   			"IsConditional" : false,
//   			"Condition" : null,
//   			"ConditionTarget" : null
//   		}, {
//   			"Uuid" : null,
//   			"OrderUuid" : "8925d746-bc9f-4684-b1aa-e507467aaa99",
//   			"Exchange" : "BTC-LTC",
//   			"OrderType" : "LIMIT_BUY",
//   			"Quantity" : 100000.00000000,
//   			"QuantityRemaining" : 100000.00000000,
//   			"Limit" : 0.00000001,
//   			"CommissionPaid" : 0.00000000,
//   			"Price" : 0.00000000,
//   			"PricePerUnit" : null,
//   			"Opened" : "2014-07-09T03:55:48.583",
//   			"Closed" : null,
//   			"CancelInitiated" : false,
//   			"ImmediateOrCancel" : false,
//   			"IsConditional" : false,
//   			"Condition" : null,
//   			"ConditionTarget" : null
//   		}
//   	]

	class order_t 
	{
		string uuid;
		string order_id;
		string exchange;
		string type;
		money_t quanity;
		money_t comission;
		money_t price;
		money_t price_per;
		string opened;
		string closed;
		bool canceled;
		bool immediate;
		bool cond;
		string condition;
		string target;
	};
	class order_l : public std::vector<order_t>
	{
		using std::vector<order_t>::vector;
	};
};

#endif

