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
		money_t btc;
		bool operator<(const balance_t &rhs) const;
		virtual ostream &stream(ostream &lhs, int ind=0) const;
		virtual ostream &header(ostream &lhs, int ind=0) const;
    balance_t();
		virtual ~balance_t();
	};
	struct balance_l : public std::vector<balance_t> {
		static balance_l list;
		balance_l();
    virtual ~balance_l();
		template<typename itr_t>
			balance_l(itr_t b, itr_t e);
		static const balance_l &load_balances();
		const balance_t &get(const string &sym) const {
			for( auto &b : *this ) {
				if(b.sym==sym)
					return b;
			};
			throw runtime_error("no such bal!");
		};
    const balance_t *get_ptr(const string &sym) const
    {
			for( auto &b : *this ) {
				if(b.sym==sym)
					return &b;
			};
      return 0;
    };
		static const balance_l &get_balances();
		sym_l syms() const;
	};
  template<typename itr_t>
    balance_l::balance_l(itr_t b, itr_t e)
    : std::vector<balance_t>(b,e)
    {
      cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << endl;
    };
};
#endif

