#ifndef fmt_hh
#define fmt_hh fmt_hh

#include <coinfwd.hh>
namespace fmt {
	struct can_str {
    virtual ostream &header(ostream &lhs, int ind=0) const=0;
		virtual ostream &stream(ostream &lhs, int ind=0) const=0;
		virtual size_t get_width() const=0;
		virtual ~can_str();
	};
	class fp_val : public can_str
  {
		protected:
		double val;
		virtual ~fp_val ();
		fp_val(double val)
			: val(val)
		{
		};
		public:
//       virtual ostream &header(ostream &lhs, int ind=0) const;
//   		virtual ostream &stream(ostream &lhs, int ind=0) const;
//   		virtual size_t get_width() const;
		double get() const {
			return val;
		};
		explicit operator double()const{
			return get();
		};
	};
	class pct_t : public fp_val  {
		public:
			pct_t(double val=0)
				: fp_val(val)
			{
			};
			template<typename lhs_t, typename rhs_t>
				pct_t( lhs_t lhs, rhs_t rhs )
				: fp_val(lhs/rhs)
				{
				};
		virtual size_t get_width() const;
    virtual ostream &header(ostream &lhs, int ind=0) const;
		virtual ostream &stream(ostream &lhs, int ind=0) const;
	};
  inline bool operator<(const pct_t &lhs, const pct_t &rhs)
  {
    return lhs.get()<rhs.get();
  };
  inline bool operator==(const pct_t &lhs, const pct_t &rhs)
  {
    return lhs.get()==rhs.get();
  };
	struct nl_t {
		friend ostream &operator<<(ostream &lhs, nl_t nl)
		{
			return lhs.put('\n');
		}
	};
	const static nl_t nl;
	inline ostream &operator<<( ostream &lhs, const fmt::can_str &rhs )
	{
		return rhs.stream(lhs,0);
	};
}
#endif

