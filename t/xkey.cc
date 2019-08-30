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
class HD_Wallet
{
	public:

		//constructor
		HD_Wallet(const data_chunk Userentropy)
		{
			entropy=Userentropy;
			mnemonic = wallet::create_mnemonic(entropy);
			seed = to_chunk(wallet::decode_mnemonic(mnemonic,"TREZOR"));
			privateKey = wallet::hd_private(seed);
			publicKey = privateKey.to_public();
		}

		HD_Wallet(const wallet::word_list mnemonicSeed)
		{
			mnemonic = mnemonicSeed;
			seed=to_chunk(wallet::decode_mnemonic(mnemonic,"TREZOR"));
			privateKey = wallet::hd_private(seed);
			publicKey = privateKey.to_public();
		}
		wallet::hd_private childPrivateKey(int index) const
		{
			return privateKey.derive_private(index);
		}
		wallet::hd_public childPublicKey(int index) const
		{
			return privateKey.derive_public(index);
		};
		wallet::payment_address childAddress(int index) const {
			auto pubkey = childPublicKey(index);
			auto point = pubkey.point();
			return wallet::ec_public(point).to_payment_address();
		};

	private:
		//members
		data_chunk entropy;
		data_chunk seed;
		wallet::word_list mnemonic;
		wallet::hd_private privateKey;
		wallet::hd_public publicKey;

		friend ostream &operator <<(ostream &lhs, const HD_Wallet &rhs);

};
ostream &operator <<(ostream &lhs, const HD_Wallet &rhs) 
{
	lhs << "entropy: " << encode_base16(rhs.entropy) << endl;
	lhs << "seed: " << encode_base16(rhs.seed) << endl;
	lhs << "words: " << join(rhs.mnemonic) << endl;
	lhs << "priKey: " << rhs.privateKey.encoded() << endl;
	lhs << "addr: " << rhs.childAddress(1) << endl;
	lhs << "pubKey: " << rhs.publicKey.encoded() << endl;
		return lhs;
};
int main(int, char**) {
	try {
//   		json vecs;
//   		{
//   			std::ifstream vecs_file;
//   			vecs_file.exceptions( std::ifstream::badbit );
//   			vecs_file.open("t/ckey.vectors.json");
//   			vecs_file >> vecs;
//   			vecs=vecs["english"];
//   		}
string words = "lava sketch nurse clever write biology vibrant curtain recipe help fly delay butter sad gun forest nature game asthma news diet crime pistol differ";
		wallet::word_list list = split(words);
//   		string s_entr;
//   		for( auto vec : vecs ) {
//   			auto b(vec.begin()), e(vec.end());
//   			s_entr=*b++;
//   			string s_list=*b++;
//   			string s_seed=*b++;
//   			string s_xprv=*b++;
//   			list=split(s_list);
//   		};
		HD_Wallet wallet(list);
		cout << wallet << endl;
//		data_chunk b_entr;
//		decode_base16(b_entr,s_entr);
//   		wallet=HD_Wallet(b_entr);
//   		cout << wallet << endl;
		return 0;
	} catch ( const exception &ex ) {
		cerr << "exception: " << ex.what() << endl;
	} catch ( ... ) {
		cerr << "caught shit!" << endl;
	};
  return 1;
};
