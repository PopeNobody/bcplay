#include <coinfwd.hh>
#include <bitcoin/bitcoin.hpp>
#include <bitset>

using std::hex;
using std::bitset;
using std::array;

using namespace bc;

int main(int, char**) {
  auto my_secret = base16_literal(
	  "f3c8f9a6198cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d"
	  );
  cout <<  bc::encode_base16(my_secret) << endl;;
  ec_compressed my_pubkey;
//  cout << "pubkey: " << my_pubkey << endl;
  secret_to_public(my_pubkey, my_secret);
  cout << bc::encode_base16(my_secret) << endl;
//  cout << "secret: " << my_secret << endl;
  return 0;
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
