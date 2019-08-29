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
		explicit operator double()const{
			return val;
		};
	};
//   	class price_t : public fp_val  {
//   		public:
//   		price_t(double val=0)
//   			: fp_val(val)
//   		{
//   		};
//   		double get_val() const {
//   			return val;
//   		};
//   		string fmt(bool b0, bool b1) const;
//   		string fmt() const;
//   		static int get_width() {
//   			return 16;
//   		};
//   	};
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
		static int get_width() {
			return 7;
		};
		string fmt() const;
	};
	inline ostream &operator<<(ostream &lhs, const fp_val &rhs)
	{
		return lhs<<rhs.fmt();
	};
	struct nl_t {
		friend ostream &operator<<(ostream &lhs, nl_t nl)
		{
			return lhs.put('\n');
		}
	};
	const static nl_t nl;
}
#endif

