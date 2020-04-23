#ifndef order_hh
#define order_hh order_hh

#include <coinfwd.hh>
#include <money.hh>
#include <json.hh>

namespace coin {
	class order_t 
	{
    bool     canceled;
    bool     cond;
    bool     immediate;
    bool     is_conditional;
    money_t  comission;
    money_t  price;
    money_t  price_per_unit;
    money_t  quantity;
    money_t  limit;
    money_t  quantity_remaining;
    string   closed;
    string   order_type;
    string   condition;
    money_t   condition_target;
    bool   immediate_or_cancel;
    string   exchange;
    string   opened;
    string   order_id;
    string   target;
    string   type;
    string   uuid;
    string   timestamp;
    friend void from_json( json& j, order_t& o );
    public:
    virtual ~order_t();
	};
	class order_l : public std::vector<order_t>
  {
    using std::vector<order_t>::vector;

    template <typename ...Args>
      void order_t(Args && ...args)
    {
      g(std::forward<Args>(args)...);
    }
  };
};

#endif

