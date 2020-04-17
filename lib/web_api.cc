#include <web_api.hh>
#include <coinfwd.hh>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <bitcoin/explorer.hpp>
#include <json.hh>

using namespace curlpp::options;
using namespace std;

using nlohmann::json;
using bc::system::data_chunk;

bool web::verbose =false;
namespace web {
  template<typename str_t, typename list_t>
    const string load_page(const str_t &url, const list_t &hdr)
  {
    cerr << "load_page('" << url << "', ...)" << endl;
    // That's all that is needed to do cleanup of used resources (RAII style).
    curlpp::Cleanup myCleanup;

    // Our request to be sent.
    curlpp::Easy myRequest;

    // Set the URL.
    myRequest.setOpt(new curlpp::options::Url(url));
    //    myRequest.setOpt(new curlpp::options::Proxy("gateway:38383"));
    if(verbose)
      myRequest.setOpt(new curlpp::options::Verbose(true));
    myRequest.setOpt(new curlpp::options::HttpHeader(hdr));


    std::ostringstream os;
    curlpp::options::WriteStream ws(&os);
    myRequest.setOpt(ws);
    myRequest.setOpt(new curlpp::options::FailOnError(true));
    myRequest.perform();

    return os.str();
  }
	const string load_page(const string_t &url )
	{
		return load_page(url,string_list());
	};
  data_chunk str2chunk(const string_t &str) {
    return data_chunk(str.begin(),str.end());
  };
  using bc::system::encode_base16;
  using bc::system::hmac_sha512_hash;

  std::list<string> signurl(const string_t &url, const string_t &key, const string_t &sec)
  {
    list<string> res;
    data_chunk secdat=str2chunk(sec);
    if(url.back()!='&' && url.back()!='?')
      throw runtime_error("url must end with & or ?");
    res.push_back(
        url +"apikey="+key +"&nonce="+lexical_cast<string_t>(time(0))
        );
    data_chunk urldat=str2chunk(res.back());
    string_t sign=encode_base16(hmac_sha512_hash(urldat,secdat));
    res.push_back(url);
    res.push_back("apisign:"+sign);
    return res;
  };
  string_t pp_json(const string_t &page) {
    json jpage;
    try {
      jpage= json::parse(page);
    } catch ( std::exception &e ) {
      cout << "caught: " << e.what() << endl;
      cout << page << endl;
      throw;
    };
    ostringstream res;
    res << setw(2) << jpage << endl;
    return res.str();
  };
  string_t pp_json_url(const string_t &url) {
    string_t page=load_page(url);
    if( !page.length() )
      return "error";
    return pp_json(page);
  };
  const string_t load_hmac_page(
      const string_t &url
      )
  {
    const static string_t key=getenv("KEY");
    const static string_t sec=getenv("SEC");
    // Set the URL.
    string_t fixurl=url;
    string_list headers=signurl(fixurl,key,sec);
    fixurl=headers.front();
    if(verbose)
      cout << "fixurl: " << fixurl << endl;
    headers.pop_front();
    return load_page(fixurl,headers);
  }
}
