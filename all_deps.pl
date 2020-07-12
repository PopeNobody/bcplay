#!/usr/bin/perl


use strict;
use warnings;
use Carp qw(cluck);
use Data::Dumper;

die "usage: $0 [outfile] [depfiles...]" unless @ARGV > 1;
my $ofn=shift;
eval "use autodie qw(:all);";
open(my $ofh,">$ofn");
my %seen;
while(<ARGV>) {
  s{[\\\s:]}{ }g;
  for(split) {
    next if m{\.o$};
    $seen{$_}++;
  };
};

$ofh->print("$_\n") for sort keys %seen;
