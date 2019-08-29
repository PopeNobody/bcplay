#!/usr/bin/perl
#
use Bitcoin::Crypto::PrivateKey;
use Bitcoin::BIP39;

use strict;
use warnings;

my $seed=
	"f3c8f9a6891cca98f481edde13bcc031b1470a81e367b838fe9e0a9db0f5993d";
my $secret = Bitcoin::Crypto::PrivateKey->fromHex($seed);

print $seed, "\n";
use Data::Dumper;
$Data::Dumper::Useqq=1;
print unpack("H*", $secret->rawKey), "\n";

#print $secret->rawKey;
#print $secret->toBip39Mnemonic();

