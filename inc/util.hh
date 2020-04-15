
#ifndef util_hh
#define util_hh util_hh

#include <coinfwd.hh>

namespace util {

	template<typename itr_t>
	inline string join(char c, itr_t b, itr_t e)
	{
		string res;
		if(b==e)
			return res;
		res=*b++;
		if(b==e)
			return res;
		while(b!=e) {
			res+=c;
			res+=*b++;
		};
		return res;
	};
	template<typename cont_t>
	inline string join(char c, cont_t cont)
  {
    return join(c,cont.begin(),cont.end());
  };
  inline vector<string> split( char sep, const string &str )
  {
    vector<string> res;
    auto s=str.begin();
    for( auto b(s), e(str.end()); b!=e; b++ ) {
      if( *b == sep )
      {
        res.push_back(string(s,b));
        while(b!=e && *b == sep)
          ++b;
        s=b;
      };
    };
    res.push_back(string(s,str.end()));
      
    return res;
  }
  template<typename cont_t>
    inline void sort(cont_t &cont)
    {
      sort(begin(cont),end(cont));
    };
//   	template<typename cont_t, typename itr_t>
//   	cont_t split(char sep, itr_t b, itr_t e)
//   	{
//   		cont_t res;
//   		if(b!=e) {
//   			do {
//   				auto s=find(b,e,sep);
//   				res.push_back(string(b,s));
//   				b=s;
//   				if(b!=e)
//   					++b;
//   			} while(b!=e);
//   		};
//   		return res;
//   	};
//   	template<typename cont_t>
//   	cont_t split(char sep, c_str str)
//   	{
//   		return split<cont_t>(sep,str.begin(),str.end());
//   	};
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

