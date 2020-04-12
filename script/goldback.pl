#!/usr/bin/perl

use strict;
use warnings;

our(%count);
$count{50}=6;
$count{25}=15;
$count{10}=19;
$count{5}=20;
$count{1}=2;

my $total=0;
for( sort { $b <=> $a } keys %count )
{
  my $den=$_;
  my $qty=$count{$den};
  my $sub=$den*$qty;
  $total+=$sub;
  print "$den * $qty = $sub | $total\n";
};
