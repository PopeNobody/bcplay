#ifndef dbcode_hh
#define dbcode_hh

#include <coinfwd.hh>
#include <prices.hh>

namespace dbase {
	using std::shared_ptr;
	struct price_t {
		string sym;
		double  usd;
	};
	class price_l : public std::vector<price_t> {
	};
	class dbcon {
		public:	
		class body;
		typedef shared_ptr<body> body_ref;
		dbcon();
		void add_hist_prices(int daysago,const coin::res_v &data);
		void add_prices(const price_l &data);
		time_t last_hist_time();
		private:
		body_ref m_body;
	};
};
#endif

