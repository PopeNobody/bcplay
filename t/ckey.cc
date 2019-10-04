#include <coinfwd.hh>
#include <web_api.hh>
#include <bitcoin/bitcoin.hpp>
#include <algorithm>
#include <cxxabi.h>
#include <json.hh>



using std::hex;
using std::array;

using namespace bc;
using namespace std;

ostream &operator<<(ostream &ostream, const ec_compressed &data) {
		return ostream << bc::encode_base16(data);
};
template<size_t _n>
ostream &operator<<(ostream &ostream, const std::array<unsigned char,_n> &data) {
		return ostream << bc::encode_base16(data);
};
using libbitcoin::wallet::decode_mnemonic;
int main(int, char**) {
	try {
		json vecs;
		{
			std::ifstream vecs_file;
			vecs_file.exceptions( std::ifstream::badbit );
			vecs_file.open("t/ckey.vectors.json");
			vecs_file >> vecs;
			vecs=vecs["english"];
		}
		for( auto vec : vecs ) {
			{
				auto b(vec.begin()), e(vec.end());
				string s_seed=*b++;
				string s_list=*b++;
				string s_data=*b++;
				string s_xprv=*b++;

				cout << "s_seed: " << s_seed << endl;
				data_chunk b_seed;
				bc::decode_base16(b_seed,s_seed);
				assert(encode_base16(b_seed)==s_seed);
				cout << "b_seed: " << encode_base16(b_seed) << endl;
				cout << endl;

				auto b_list=bc::wallet::create_mnemonic(b_seed);
				cout << "s_list: " << s_list << endl;
				cout << "b_list: " << join(b_list) << endl;
				cout << endl;
				assert(join(b_list)==s_list);

				auto decoded=decode_mnemonic(b_list,"TREZOR");
				data_chunk b_data(decoded.begin(),decoded.end());
				cout << "b_data: " << encode_base16(b_data) << endl;
				cout << "s_data: " << s_data << endl;
				cout << endl;
				assert(encode_base16(b_data) == s_data);


				auto b_xprv=wallet::hd_private(b_data);
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
