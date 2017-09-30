#!/usr/local/bin/perl
###
# This program was written by and is copyright Alec Muffett 1991,
# 1992, 1993, 1994, 1995, and 1996, and is provided as part of the
# Crack v5.0 Password Cracking package.
#
# The copyright holder disclaims all responsibility or liability with
# respect to its usage or its effect upon hardware or computer
# systems, and maintains copyright as set out in the "LICENCE"
# document which accompanies distributions of Crack v5.0 and upwards.
###

# number of crypts()/second for one CPU

#$crypts_per_second = 1710; # 486dx2/66 + 128Kb cache, libdes, gcc-2.7.2
$crypts_per_second = 3430; # Pentium P75, libdes, gcc-2.7.2
#$crypts_per_second = 1; # Sparcstation 10/40, libdes, gcc-2.7.2
#$crypts_per_second = 1; # Ultrastation 1, libdes, gcc-2.7.2

# gcc flags: gcc -O4 -fomit-frame-pointer -funroll-loops

$one_kb = 1024;
$one_mb = 1024 ** 2;
$one_gb = 1024 ** 3;
$one_tb = 1024 ** 4;

sub Expand {
	($seconds) = @_;

	$retval = "";

	$years = int($seconds/(365 * 24 * 60 * 60));
	$seconds -= ($years * (365 * 24 * 60 * 60));
	if ($years) { $retval .= $years . "y" };

	$days = int($seconds/(24 * 60 * 60));
	$seconds -= ($days * (24 * 60 * 60));
	if ($days) { $retval .= $days . "d" };

	return ($retval) if ($years >= 1);

	$hours = int($seconds/(60 * 60));
	$seconds -= ($hours * (60 * 60));
	if ($hours) { $retval .= $hours . "h" };

	return ($retval) if ($days >= 1);

	$minutes = int($seconds/60);
	$seconds -= ($minutes * 60);
	if ($minutes) { $retval .= $minutes . "m" };

	$retval .= $seconds . "s";
	return ($retval);
}

sub Storage
{
    $bytes = shift;

    if ($bytes > $one_tb)
    {
	sprintf("%.2fTb", ($bytes / $one_tb));
    }
    elsif ($bytes > $one_gb)
    {
	sprintf("%.2fGb", ($bytes / $one_gb));
    }
    elsif ($bytes > $one_mb)
    {
	sprintf("%.2fMb", ($bytes / $one_mb));
    }
    elsif ($bytes > $one_kb)
    {
	sprintf("%.2fKb", ($bytes / $one_kb));
    }
    else
    {
	sprintf("%d bytes", $bytes);
    }
}

print "For a CPU capable of $crypts_per_second crypts per second:\n\n";

foreach $pwlen (1..8)
{
	print "    password length=$pwlen\n";

	foreach $setsize (10, 26, 36, 52, 62, 90)
	{
		$dwords = $setsize ** $pwlen;
		$exhaust = int($dwords / $crypts_per_second);
		$exhaust = &Expand($exhaust);
		$storage = &Storage(($pwlen + 1) * $dwords);

		printf "\tfor %d characters, words=%g\n", $setsize, $dwords;
		printf "\t\texhaustive storage=%s (without compression)\n", $storage;
		printf "\t\texhaustive search=%s (in cpu-years)\n", $exhaust;
	}

	print "\n";
}



