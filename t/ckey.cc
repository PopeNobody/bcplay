#include <coinfwd.hh>
#include <bitcoin/bitcoin.hpp>
#include <algorithm>
#include <cxxabi.h>



using std::hex;
using std::array;

using namespace bc;
using namespace std;

ostream &operator<<(ostream &ostream, const ec_compressed &data) {
		return ostream << sizeof(data) << ": " <<  bc::encode_base16(data);
};
template<size_t _n>
ostream &operator<<(ostream &ostream, const std::array<unsigned char,_n> &data) {
		return ostream << sizeof(data) << ": " <<  bc::encode_base16(data);
};
using libbitcoin::wallet::decode_mnemonic;
int main(int, char**) {
	try {
		auto my_secret = base16_literal(
				"f3c8f9a6891cca98f481edde130cc031b1479a81e367b838fe9e0a9db0f5993d"
				);
		
		cout << "secret: " << my_secret << endl;

		auto list	= bc::wallet::create_mnemonic(my_secret);
		auto seed = decode_mnemonic(list);
		cout << seed << endl;
//   		int row=0;
//   		ostringstream sentence;
//   		for(auto b(list.begin()), e(list.end()); b!=e; b++) {
//   			sentence << "  " << left << setw(12) << *b;
//   			if(++row % 4 == 0)
//   				sentence << endl;
//   		};
//   		cout << "(" << endl << sentence.str() << ")" << endl;
//   		auto my_word_list = split(sentence.str(), " \n", true);
//   		auto hd_seed = decode_mnemonic(my_word_list);
//   		for(auto b(my_word_list.begin()), e(my_word_list.end()); b!=e; b++){
//   			cout << "(" << *b << ")" << endl;
//   		};
//   		cout << typeid(hd_seed).name() << endl;	
//		cout << encode_base16(hd_seed) << endl;
		return 0;
	} catch ( const exception &ex ) {
		cerr << "exception: " << ex.what() << endl;
	} catch ( ... ) {
		cerr << "caught shit!" << endl;
	};
  return 1;
};


  
//   	cout << text << endl;
//   	cout << "size: " << sizeof(text) << endl;
//   	bc::ec_secret sec;
//   	char val=0;
//   	std::fill(sec.begin(),sec.end(), 0x28);
//   	cout << bc::encode_base16(sec) << endl << endl;
//   	return 0;
//   	auto beg(sec.begin()), end(sec.end());
//   	char *pos=text;
//   	while(beg!=end){
//   			
//   	};
//   		ec_compressed my_pubkey;
//   		secret_to_public(my_pubkey, my_secret);
//   
//   		cout << "pubkey: " << my_pubkey << endl;
//   	cout << bc::encode_base16(sec) << endl << endl;
//   	ec_compressed public_key;
//   	bc::secret_to_public(public_key,sec);
//   	cout << "public: " << bc::encode_base16(public_key) << endl;
//   	bc::short_hash hash = bc::bitcoin_short_hash(public_key);
//   	cout << "hash: " << bc::encode_base16(hash) << endl;
//   	bc::data_chunk unencoded_address;	
//   	unencoded_address.reserve(25);
//   	unencoded_address.push_back(0);
//   	bc::extend_data(unencoded_address,hash);
//   	bc::append_checksum(unencoded_address);
//   	assert(unencoded_address.size()==25);
//   	string addr=bc::encode_base58(unencoded_address);
//   	cout << "addr: " << addr << endl;
//   	bc::wallet::word_list list;
//   	list	= bc::wallet::create_mnemonic(sec);
//   	int i=0;
//   	for( auto w : list ) {
//   		cout << setw(4) << i++ << w << endl;
//   	};
