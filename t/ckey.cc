#include <coinfwd.hh>
#include <web_api.hh>
#include <algorithm>
#include <cxxabi.h>
#include <json.hh>
#include <util.hh>
#include <bitcoin/system.hpp>



using std::hex;
using std::array;

using namespace std;

using util::join;
using libbitcoin::system::data_chunk;
using libbitcoin::system::decode_base16;
using libbitcoin::system::encode_base16;
using libbitcoin::system::wallet::create_mnemonic;
using libbitcoin::system::wallet::decode_mnemonic;
using libbitcoin::system::wallet::hd_private;

int main(int, char**) {
	try {
    nlohmann::json vecs;
		{
			std::ifstream vecs_file;
			vecs_file.exceptions( std::ifstream::badbit );
			vecs_file.open("t/ckey.vectors.json");
			vecs_file >> vecs;
			vecs=vecs["english"];
		}
		for( auto vec : vecs ) {
			{
				auto b(vec.begin());
				string s_seed=*b++;
				string s_list=*b++;
				string s_data=*b++;
				string s_xprv=*b++;

				cout << "s_seed: " << s_seed << endl;
				data_chunk b_seed;
				decode_base16(b_seed,s_seed);
				assert(encode_base16(b_seed)==s_seed);
				cout << "b_seed: " << encode_base16(b_seed) << endl;
				cout << endl;

				auto b_list=create_mnemonic(b_seed);
				cout << "s_list: " << s_list << endl;
				cout << "b_list: " << join(' ',b_list) << endl;
				cout << endl;
				assert(join(' ',b_list)==s_list);

				auto decoded=decode_mnemonic(b_list,"TREZOR");
				data_chunk b_data(decoded.begin(),decoded.end());
				cout << "b_data: " << encode_base16(b_data) << endl;
				cout << "s_data: " << s_data << endl;
				cout << endl;
				assert(encode_base16(b_data) == s_data);


				auto b_xprv=hd_private(b_data);
				cout << "b_xprv: " << b_xprv << endl;
				cout << "s_xprv: " << s_xprv << endl;
				cout << endl;
				assert(b_xprv.encoded()==s_xprv);

				auto root_prv=b_xprv.derive_private(1);
				auto root_pub=b_xprv.derive_public(1);

				cout 	<< "root_prv: " << root_prv << endl;
				cout 	<< "root_pub: " << root_pub << endl;
			};
		}
		return 0;
	} catch ( const exception &ex ) {
		cerr << "exception: " << ex.what() << endl;
	} catch ( ... ) {
		cerr << "caught shit!" << endl;
	};
  return 1;
};
