#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

using namespace bc;
using namespace wallet;

int main(int, char**) {
    // ******* part 1 *******
    // Begin with a private key
    auto my_secret = base16_literal(
				"f3c8f9a6891cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d"
				);

    // Derive pubkey
    ec_compressed my_pubkey;
    secret_to_public(my_pubkey, my_secret);

    // Pubkeyhash: sha256 + hash160
    auto my_pubkeyhash = bitcoin_short_hash(my_pubkey);

    // Prefix for mainnet = 0x00
    one_byte addr_prefix = { { 0x00 } }; //Testnet 0x6f

    // Byte sequence = prefix + pubkey + checksum(4-bytes)
    data_chunk prefix_pubkey_checksum(to_chunk(addr_prefix));
    extend_data(prefix_pubkey_checksum, my_pubkeyhash);
    append_checksum(prefix_pubkey_checksum);

    // Base58 encode byte sequence -> Bitcoin Address
    std::cout << encode_base58(prefix_pubkey_checksum) << std::endl;

    // You can directly generate Bitcoin addresses
    // with Libbitcoin wallet types: ec_private/ec_public
    // described in the following section


    //******* part 2 *******

    // WIF encoded private key
    // Additional Information: Mainnet, PubKey compression
    one_byte secret_prefix = { { 0x80 } }; //Testnet Prefix: 0xEF
    one_byte secret_compressed = { { 0x01} }; //Omitted if uncompressed

    // Apply prefix, suffix & append checksum
    auto prefix_secret_comp_checksum = to_chunk(secret_prefix);
    extend_data(prefix_secret_comp_checksum, my_secret);
    extend_data(prefix_secret_comp_checksum, secret_compressed);
    append_checksum(prefix_secret_comp_checksum);

    // WIF (mainnet/compressed)
    std::cout << encode_base58(prefix_secret_comp_checksum) << std::endl;

    // ******* part 3 *******
    // Instantiate ec_private object
    // ec_private::mainnet = 0x8000 (Mainnet Prefixes 0x80,0x00)
    // ec_private::testnet = 0xEF6F (Testnet Prefixes 0xEF,0x6F)
    ec_private my_private(my_secret, ec_private::mainnet, true);
    std::cout << my_private.to_payment_address() << std::endl;
    std::cout << my_private.encoded() << std::endl; //WIF private key

    // ******* part 4 *******

    // ec_public from ec_private
    // (compression implied from ec_private input)
    ec_public my_public(my_private);

    // ec_public from point
    // (compression not implied, supplied as arguments)
    ec_public my_public2(my_pubkey, true); //compression = true

    // Payment addresses:
    // Will always default to mainnet if no argument supplied
    // regardless of version in ec_private constructor argument
    payment_address my_addr = my_public.to_payment_address();
    payment_address my_addr2 = my_public2.to_payment_address(); //0x00, 0x6f
    std::cout << (my_addr.encoded() == my_addr2.encoded()) << std::endl;

}

