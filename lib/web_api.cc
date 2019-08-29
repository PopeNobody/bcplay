#include <web_api.hh>
#include <coinfwd.hh>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <bitcoin/bitcoin.hpp>
#include <json.hh>

using namespace curlpp::options;
using namespace std;

using nlohmann::json;
using bc::data_chunk;

bool web::verbose	=false;

const string web::load_page(const string &url, const list<string> &headers)
{
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

		// Set the URL.
		myRequest.setOpt(new curlpp::options::Url(url));
//		myRequest.setOpt(new curlpp::options::Proxy("gateway:38383"));
		if(verbose)
			myRequest.setOpt(new curlpp::options::Verbose(true));
		myRequest.setOpt(new curlpp::options::HttpHeader(headers));


		std::ostringstream os;
		curlpp::options::WriteStream ws(&os);
		myRequest.setOpt(ws);
		myRequest.setOpt(new curlpp::options::FailOnError(true));
		myRequest.perform();

		return move(os.str());
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
	string sign=bc::encode_base16(bc::hmac_sha512_hash(urldat,secdat));
	res.push_back(url);
	res.push_back("apisign:"+sign);
	return move(res);
};
string web::pp_json(const string &url) {
	string page=web::load_page(url);
	if( !page.length() )
		return "error";
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
	return move(res.str());
};
const string web::load_hmac_page(
		const string &url
		)
{
	const static string key=getenv("KEY");
	const static string sec=getenv("SEC");
	// Set the URL.
	string fixurl=url;
	std::list<string>	headers=signurl(fixurl,key,sec);
	fixurl=headers.front();
	if(verbose)
		cout << "fixurl: " << fixurl << endl;
	headers.pop_front();
	return load_page(fixurl,headers);
};

