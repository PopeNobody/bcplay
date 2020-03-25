#!/usr/bin/perl

use strict;
use warnings;

use IO::Pipe;
use IO::Select;
use Data::Dumper;
sub xfork() {
  for(fork) {
    die "fork:$!" unless defined $_;
    return $_;
  };
};

my @gpg = qx( gpg < keys.asc 2>&1 );
chomp(@gpg);

print Dumper(\@gpg);
