use List::Util qw(sum max);

sub calc_ints
{
  my $mask = shift;
  my @list = qw//;

  my $b, $c = 1;
  for ($b = 0; $b < 16; $b++) {
    if ($mask & (1 << $b)) {
      $c++;
    }
    else {
      push @list, $c;
      $c = 1;
    }
  }
  return @list;
}

sub print_list
{
  my $mask = shift;
  printf("  /* %04x */", $mask);
  my @list = @{$_[0]};
  my $l = scalar(@list);
  my $s = sum(@list) || 0;
  print "  {$l, $s},\n";
  #my $i = 0;
  #for (; $i < scalar(@list); $i++) {
  #  print $list[$i];
  #  print ", " if ($i < 15);
  #}
  #for (; $i < 16; $i++) {
  #  print "0";
  #  print ", " if ($i < 15);
  #}
  #print "},\n";
}

sub is_too_full
{
  my @list = @_;
  return sum(@list) > 16 || max(@list) > 5;
}

for ($k = 0; $k <= 0xffff; $k++) {
  my @ints = calc_ints($k);
  print_list($k, \@ints); # unless (is_too_full(@ints));
}
