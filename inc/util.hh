
#ifndef util_hh
#define util_hh util_hh

#include <coinfwd.hh>

namespace util {

	template<typename itr_t, typename sep_t>
	inline string join(const sep_t &sep, itr_t b, itr_t e)
	{
		string res;
		if(b==e)
			return res;
		res=*b++;
		if(b==e)
			return res;
		while(b!=e) {
			res+=sep;
			res+=*b++;
		};
		return res;
	};
	template<typename cont_t, typename itr_t>
	cont_t split(char sep, itr_t b, itr_t e)
	{
		cont_t res;
		if(b!=e) {
			do {
				auto s=find(b,e,sep);
				res.push_back(string(b,s));
				b=s;
				if(b!=e)
					++b;
			} while(b!=e);
		};
		return res;
	};
	template<typename cont_t>
	cont_t split(char sep, c_str str)
	{
		return split<cont_t>(sep,str.begin(),str.end());
	};
	template<typename itr_t, typename val_t>
		bool contains(itr_t b, itr_t e, const val_t &val){
			while(b!=e) {
				if( *b++ == val )
					return true;
			}
			return false;
		};
	template<typename con_t, typename val_t>
		bool contains(const con_t &con, const val_t &val) {
			return contains(begin(con),end(con),val);
		};
	string strip(const string &str);
	bool exists(const char *path);
	string read_file(const char *path);
};

#endif

