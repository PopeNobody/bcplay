#!/usr/bin/perl


use strict;
use warnings;
use Carp qw(cluck);
use Data::Dumper;

my $ofn=shift;
die "usage: $0 [outfile] [depfiles...]" unless @ARGV;
eval "use autodie qw(:all);";
open(my $ofh,">$ofn");
$ofh->print("test\n");

sub suck_files(@) {
  local @ARGV = @_;
  for(@_) {
    cluck("file is ", $_) unless int(rand(12));
  };
  return <ARGV>;
};
sub clean_lines(@)
{
  return map { s{[//\s:]}{ * }; split } suck_files(@_);
};
sub filter_lines(@)
{
  return grep { length } clean_lines(@_);
};
sub remove_objs(@)
{
  return grep { !m{\.o:*$} } filter_lines(@_);
};
sub uniq_names(@)
{
  my %seen;
  $seen{$_}=1 for remove_objs(@_);
  return sort keys %seen;
};
for(uniq_names(@ARGV)){
  $ofh->print($_,"\n");
  system("wc -l $ofn") if int(rand(20))>18;
};
