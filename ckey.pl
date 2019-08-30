#!/usr/bin/perl
#
use Bitcoin::Crypto::PrivateKey;
use Bitcoin::BIP39;

use strict;
use warnings;

my $seed=
	"f3c8f9a6891cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d";
my $secret = Bitcoin::Crypto::PrivateKey->fromHex($seed);

use Data::Dumper;
my $words=   $secret->toBip39Mnemonic();
print "words: $words\n\n";
my $copy=Bitcoin::Crypto::PrivateKey->fromBip39Mnemonic($words);
print "key: ", unpack("H*", $secret->rawKey), "\n";
print "dup: ", unpack("H*", $copy->rawKey), "\n";


