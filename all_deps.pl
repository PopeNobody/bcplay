#!/usr/bin/perl


use strict;
use warnings;
use Carp qw(cluck);
use Data::Dumper;

my $ofn=shift;
die "usage: $0 [outfile] [depfiles...]" unless @ARGV;
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

print "$_\n" for sort keys %seen;
