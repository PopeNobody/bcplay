#ifndef web_api_hh
#define web_api_hh web_api_hh


#include <coinfwd.hh>
namespace web {
	using std::list;
	const string load_page(const string &url, const list<string> &headers);
	inline const string load_page(const string &url )
	{
		return load_page(url,list<string>());
	};
	const string load_hmac_page( const string &url );
	string pp_json_url(const string &url);
	string pp_json(const string &url);
	extern bool verbose;
};

#endif

