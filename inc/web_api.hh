#ifndef web_api_hh
#define web_api_hh web_api_hh


#include <coinfwd.hh>
namespace web {
	using std::list;
  struct string_t : public string
  {
    string_t(const string &str=string())
      : string(str)
    {
    }
    string_t(const char*str)
      :string(str)
    {
    }
    virtual ~string_t()
    {
    }
  };
  struct string_list : public std::list<string>
  {
    string_list(const std::list<string> &l=list<string>())
      : list(l)
    {
    };
    virtual ~string_list()
    {
    };
  };
  template<typename str_t, typename list_t>
    const string load_page(const str_t &url, const list_t &hdr);

	const string load_page(const string_t &url );
	const string load_hmac_page( const string &url );
	string pp_json_url(const string &url);
	string pp_json(const string &url);
	extern bool verbose;
};

#endif

