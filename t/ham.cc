#include <coinfwd.hh>
#include <bitcoin/bitcoin.hpp>
#include <bitset>
using std::hex;
using std::bitset;
using std::array;

using namespace bc;

int main(int, char**) {
	try {
		std::ifstream str("t/ham.txt");
		char ch;
		int i=0;
		bc::ec_secret raw;
		auto b(raw.begin());
		auto e(raw.end());
		assert(b!=e);
		while(str>>ch) {
			assert(b!=e);
			*b=*b*2;
			if(ch&1)
				++*b;
			++i;
			if(i%8)
				continue;
			if(++b==e)
				break;
		};
		bc::ec_secret sec;
		copy(raw.rbegin(),raw.rend(),sec.begin());
		cout << bc::encode_base16(sec) << endl << endl;
		ec_compressed public_key;
		bc::secret_to_public(public_key,sec);
		cout << "public: " << bc::encode_base16(public_key) << endl;
		bc::short_hash hash = bc::bitcoin_short_hash(public_key);
		cout << "hash: " << bc::encode_base16(hash) << endl;
//   		bc::data_chunk unencoded_address;	
//   		unencoded_address.reserve(25);
//   		unencoded_address.push_back(0);
//   		bc::extend_data(unencoded_address,hash);
//   		bc::append_checksum(unencoded_address);
//   		assert(unencoded_address.size()==25);
//   		string addr=bc::encode_base58(unencoded_address);
//   		cout << "addr: " << addr << endl;
		bc::wallet::word_list list;
		list	= bc::wallet::create_mnemonic(sec);
		for( auto w : list ) {
			cout << "  " << w << endl;
		};

		return 0;
	} catch ( const std::exception &e ) {
		cout << "exception: " << e.what() << endl;
	};
	return 1;
};



