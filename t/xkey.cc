#include <bitcoin/bitcoin.hpp>
#include <coinfwd.hh>
#include <web_api.hh>
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
	lhs << "addr: " << rhs.childAddress(0) << endl;
	lhs << "pubKey: " << rhs.publicKey.encoded() << endl;
		return lhs;
};
int xmain(int, char**) {
	try {
		string words = getenv("WALLET_WORDS");
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

int main(int, char**) {

		string my_sentence = getenv("WALLET_WORDS");
    // Load mnemonic sentence into word list
    auto my_word_list = split(my_sentence, " ", true);

    // Create an optional secret passphrase
    std::string my_passphrase = "my secret passphrase";

    // Create 512bit seed (without optional secret passphrase)
    auto hd_seed = decode_mnemonic(my_word_list);

    // Create 512bit seed (with optional secret passphrase)
    // Requires: Libbitcoin compiled with ICU.
    // auto hd_seed = decode_mnemonic(my_word_list, my_passphrase);

    // ******* part 2 *******

		using bc::wallet::hd_private;
		using bc::wallet::hd_public;
    // We reuse 512 bit hd_seed from the previous example
    // Derivation of master private key m
    data_chunk seed_chunk(to_chunk(hd_seed));
    hd_private m(seed_chunk, hd_private::mainnet);

    // Derivation of master public key M
    hd_public M = m.to_public();


    // ******* part 3 *******

		using bc::wallet::ec_public;
		auto M44 = M.derive_public(44);
		auto M44_0 = M44.derive_public(0);
		auto M44_0_0 = M44_0.derive_public(0);
		cout << M44 << endl;
		cout << ec_public(M44.point()).to_payment_address();

}
