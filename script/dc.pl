#!/usr/bin/perl

use strict;
use warnings;

use GnuPG qw( :algo );
 
my $gpg = new GnuPG();
 
$gpg->encrypt(  plaintext    => "file.txt",    output        => "file.gpg",
        armor    => 1,         sign    => 1,
        passphrase  => $secret );
 
#    $gpg->decrypt( ciphertext    => "file.gpg",    output        => "file.txt" );
#     
#    $gpg->clearsign( plaintext => "file.txt", output => "file.txt.asc",
#             passphrase => $secret,   armor => 1,
#            );
#     
#    $gpg->verify( signature => "file.txt.asc", file => "file.txt" );
#     
#    $gpg->gen_key( name => "Joe Blow",        comment => "My GnuPG key",
#           passphrase => $secret,
#            );
