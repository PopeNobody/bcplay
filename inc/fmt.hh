#ifndef fmt_hh
#define fmt_hh fmt_hh

#include <coinfwd.hh>
namespace fmt {
	class fp_val {
		protected:
		double val;
		virtual ~fp_val ();
		fp_val(double val)
			: val(val)
		{
		};
		public:
		virtual string fmt() const=0;
		virtual int get_width() const=0;
		double get() const {
			return val;
		};
		explicit operator double()const{
			return get();
		};
    bool operator!() const
    {
      return !val;
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
		virtual int get_width() const;
		string fmt() const;
    pct_t &operator+=(const pct_t&rhs)
    {
      val+=rhs.val;
      return *this;
    };

	};
	inline ostream &operator<<(ostream &lhs, const fp_val &rhs)
	{
		return lhs<<setw(rhs.get_width())<<rhs.fmt();
	};
	struct nl_t {
		friend ostream &operator<<(ostream &lhs, nl_t nl)
		{
			return lhs.put('\n');
		}
	};
	const static nl_t nl;
	struct can_str {
		virtual ostream &stream(ostream &lhs, int ind=0) const=0;
		virtual ~can_str();
	};
	inline ostream &operator<<( ostream &lhs, const fmt::can_str &rhs )
	{
		return rhs.stream(lhs,0);
	};
}
#endif

