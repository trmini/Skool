#!/usr/local/bin/perl -w

@pack_size = (1024, 2048, 4096, 8192, 16384, 32768);
foreach $pack_size (@pack_size) {
  for ( $i = 0; $i < 10; $i++) {
    $output_line = `java -classpath ./ client_server -c medusa.cs.utsa.edu 20000 $pack_size 1000`; 
    push @output, $output_line;
  }
}
print @output;
