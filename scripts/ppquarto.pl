use strict;
use warnings;
use File::Slurp;
use MIME::Base64;

my ($infile, $outfile) = @ARGV;
die "usage: $0 input output\n" unless @ARGV == 2;

my $prefix = "https://raw.githubusercontent.com/UofUEpiBio/epiworldpy/refs/heads/main/docs/";
my $html = read_file($infile);

# Inline <img src="*.svg">
$html =~ s{
    (<img\b[^>]*?\s+src\s*=\s*["'])(?!https?:|data:|/)([^"']+)(["'])
}{
    do {
        my $file = "docs/$2";
        print $file . "\n";
        if (-f $file) {
            local $/;
            open my $fh, "<:raw", $file;
            my $svg = <$fh>;
            close $fh;
            my $b64 = encode_base64($svg, "");
            $1 . "data:image/svg+xml;base64," . $b64 . $3;
        } else {
            # fallback: leave the img tag as-is if file not found
            $&
        }
    }
}egxis;


# Prefix all src attributes
$html =~ s{
    (<img\b[^>]*?\s+src\s*=\s*["'])(?!https?:|data:|/)([^"']+)(["'])
}{$1 . $prefix . $2 . $3}egxis;

write_file($outfile, $html);
