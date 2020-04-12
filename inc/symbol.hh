#ifndef symbol_hh
#define symbol_hh symbol_hh

#include <coinfwd.hh>
#include <fmt.hh>

namespace coin {
	using std::string;
	using std::ostream;
  using fmt::can_str;
	class sym_t : public string , public can_str
	{
		using std::string::string;
		public:
    sym_t(const string &str=string())
      : string(str)
    {
    };
    virtual ~sym_t();
		virtual size_t get_width() const;
    virtual ostream &header(ostream &lhs, int ind=0) const;
		virtual ostream &stream(ostream &lhs, int ind=0) const;
	};
	class sym_l : public vector<sym_t>
 	{
	};
};


#endif

