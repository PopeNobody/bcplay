#ifndef balance_hh
#define balance_hh balance_hh
#include <money.hh>
#include <symbol.hh>

namespace coin {
	struct balance_t : public fmt::can_str {
		sym_t sym;
		money_t bal;
		money_t ava;
		money_t pend;
		money_t usd;
		money_t eth;
		money_t btc;
		bool operator<(const balance_t &rhs) const;
		virtual ostream &stream(ostream &lhs, int ind=0) const;
		virtual ~balance_t();
	};
	struct balance_l : public std::vector<balance_t> {
		static balance_l list;
		balance_l()
		{
		};
		template<typename itr_t>
			balance_l(itr_t b, itr_t e)
			: std::vector<balance_t>(b,e)
			{
			};
		static const balance_l &load_balances();
		const balance_t &get(const string &sym) const {
			for( auto &b : *this ) {
				if(b.sym==sym)
					return b;
			};
			throw runtime_error("no such bal!");
		};
		static const balance_l &get_balances() {
			if(list.size()==0)
				list=load_balances();
			return list;
		};
		sym_l syms() const;
	};
};
#endif

