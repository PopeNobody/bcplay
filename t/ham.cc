#include <bitcoin/system.hpp>
#include <coinfwd.hh>
#include <bitset>
using std::hex;
using std::bitset;
using std::array;

using namespace libbitcoin::system;
using namespace libbitcoin::system::wallet;

int main(int, char**) {
	try {
		std::ifstream str("t/ham.txt");
		char ch;
		int i=0;
		ec_secret raw;
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
		ec_secret sec;
		copy(raw.rbegin(),raw.rend(),sec.begin());
		cout << encode_base16(sec) << endl << endl;
		ec_compressed public_key;
		secret_to_public(public_key,sec);
		cout << "public: " << encode_base16(public_key) << endl;
		short_hash hash = bitcoin_short_hash(public_key);
		cout << "hash: " << encode_base16(hash) << endl;
//   		bc::data_chunk unencoded_address;	
//   		unencoded_address.reserve(25);
//   		unencoded_address.push_back(0);
//   		bc::extend_data(unencoded_address,hash);
//   		bc::append_checksum(unencoded_address);
//   		assert(unencoded_address.size()==25);
//   		string addr=bc::encode_base58(unencoded_address);
//   		cout << "addr: " << addr << endl;
		word_list list;
		list	= create_mnemonic(sec);
		for( auto w : list ) {
			cout << "  " << w << endl;
		};

		return 0;
	} catch ( const std::exception &e ) {
		cout << "exception: " << e.what() << endl;
	};
	return 1;
};



