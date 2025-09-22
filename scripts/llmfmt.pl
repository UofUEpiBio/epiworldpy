use strict;
use warnings;
use File::Spec;

my $directory = shift @ARGV // ".";
opendir(my $dh, $directory) or die "Cannot open directory $directory: $!";

my @cpp_files = sort grep { /\.cpp$/ && -f File::Spec->catfile($directory, $_) } readdir($dh);
closedir($dh);

for my $i (0 .. $#cpp_files) {
    my $filename = $cpp_files[$i];
    my $filepath = File::Spec->catfile($directory, $filename);

    print "// $filename\n";

    open(my $fh, "<:encoding(UTF-8)", $filepath) or die "Cannot open $filepath: $!";
    while (my $line = <$fh>) {
        print $line;
    }
    close($fh);

    if ($i < $#cpp_files) {
        print "\n---\n\n";
    }
}
