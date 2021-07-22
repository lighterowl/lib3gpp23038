#!/usr/bin/env perl

use strict;
use warnings;
use utf8;

use Encode qw(decode);

# a small tool to make it easier to convert the tables found in the official
# 3GPP TS 23.038 V16.0.0 (2020-07) document. they're not the easiest to work
# with, so I've just copy-and-pasted them one by one into a text document, ran
# this script, and put the output into gentables.pl.
# when using LibreOffice, a row-first representation of the table is produced,
# which this script then converts into the appropriate sequence of anonymous
# Perl hashes.
# a word of warning : Greek letters in the document are not encoded using actual
# code points representing these letters - instead, the code points represent
# Latin letters and they're displayed as Greek because they use a different
# font.

# there's generally no reason to run this script manually unless you want to
# verify the contents of gentables.pl.

sub line2gsm {
    my $line = shift;
    return ( ( $line & 0x7 ) << 4 ) | ( ( $line & 0x78 ) >> 3 );
}

my $line = 0;
for ( my $line = 0 ; <STDIN> ; $line++ ) {
    chomp;
    my $str = decode( 'UTF-8', $_ );
    next if $str eq '' or $str eq ' ' or $str eq '4)';

    my $uni;
    if ( $str =~ /^[0-9A-F]{4}$/ ) {
        $uni = hex($str);
    }
    elsif ( $str eq 'LF' ) {
        $uni = 0x0A;
    }
    elsif ( $str eq 'SP' ) {
        $uni = ord(' ');
    }
    elsif ( $str eq 'CR' ) {
        $uni = 0x0D;
    }
    elsif ( $str eq '3)' ) {
        $uni = 0x0C;
    }
    elsif ( $str eq '1)' ) {

        # escape : omit from mapping
    }
    else {
        $uni = ord($str);
    }
    printf '{gsm=>0x%02x, uni=>0x%04x},' . "\n", line2gsm($line), $uni;
}
