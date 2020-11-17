#include <web_api.hh>
#include <coinfwd.hh>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <json.hh>
//#include <bitcoin/system/math/hash.hpp>
//#include <bitcoin/system/formats/base_16.hpp>
#include <hash.hh>
#include <util.hh>

using namespace curlpp::options;
using namespace std;

using nlohmann::json;

//using bc::system::encode_base16;
//using bc::system::hmac_sha512_hash;

bool web::verbose =false;

const string web::load_page(const string &url, const list<string> &headers)
{
  // That's all that is needed to do cleanup of used resources (RAII style).
  curlpp::Cleanup myCleanup;

  // Our request to be sent.
  curlpp::Easy myRequest;

  // Set the URL.
  myRequest.setOpt(new curlpp::options::Url(url));
  //    myRequest.setOpt(new curlpp::options::Proxy("gateway:38383"));
  if(verbose)
    myRequest.setOpt(new curlpp::options::Verbose(true));
  myRequest.setOpt(new curlpp::options::HttpHeader(headers));


  std::ostringstream os;
  curlpp::options::WriteStream ws(&os);
  myRequest.setOpt(ws);
  myRequest.setOpt(new curlpp::options::FailOnError(true));
  myRequest.perform();

  return os.str();
}
data_chunk str2chunk(const string &str) {
  return data_chunk(str.begin(),str.end());
};

std::list<string> signurl(const string &url, const string &key, const string &sec)
{
  list<string> res;
  data_chunk secdat=str2chunk(sec);
  if(url.back()!='&' && url.back()!='?')
    throw runtime_error("url must end with & or ?");
  res.push_back(
      url +"apikey="+key +"&nonce="+lexical_cast<string>(time(0))
      );
  data_chunk urldat=str2chunk(res.back());
  string sign=encode_base16(hmac_sha512_hash(urldat,secdat));
  res.push_back(url);
  res.push_back("apisign:"+sign);
  return res;
};
static json parse_json(const string &page)
{
  try {
    json jpage= json::parse(page);
    return jpage;
  } catch ( std::exception &e ) {
    cout << "caught: " << e.what() << endl;
    cout << page << endl;
    throw;
  };
};
string web::pp_json(const string &page) {
  json jpage = json::parse(page);
  ostringstream res;
  res << setw(2) << jpage << endl;
  return move(res.str());
};
string web::pp_json_url(const string &url) {
  string page=web::load_page(url);
  if( !page.length() )
    return "error";
  return web::pp_json(page);
};

static const pair<string,string> read_creds()
{
  static int first=0;
  cerr << "first: " << first << endl;
  xassert(!first++);
  string text=util::read_gpg_file("etc/cred.json.asc");
  json js = parse_json(text);
  pair<string,string> res;
  res.first=js["key"];
  res.second=js["sec"];
  return res;
};
const string web::load_hmac_page(
    const string &url
    )
{
  const static pair<string,string> creds=read_creds();
  const string &key=creds.first;
  const string &sec=creds.second;
  // Set the URL.
  string fixurl=url;
  std::list<string> headers=signurl(fixurl,key,sec);
  fixurl=headers.front();
  if(verbose)
    cout << "fixurl: " << fixurl << endl;
  headers.pop_front();
  return load_page(fixurl,headers);
};

