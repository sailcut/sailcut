#!/usr/bin/perl

use strict;
use warnings;

use XML::DOM;

my $file = shift;

if ( ! -f $file) {
  die("Could not find file '$file'");
}

my $parser = XML::DOM::Parser->new();
my $doc = $parser->parsefile($file);

my %all_fmts = (
  'PNG' => ['png', 'html'],
  'EPS' => ['eps', 'pdf'],
);
my $ref_fmt = 'PNG';
foreach my $media($doc->getElementsByTagName('mediaobject'))
{
  my %have_fmts;
  foreach my $imgobject($media->getElementsByTagName('imageobject'))
  {
    my $imgdata = imgData($imgobject);
    my $fmt = $imgdata->getAttribute('format');
    $have_fmts{$fmt} = $imgobject;
  }
  if (!$have_fmts{$ref_fmt}) {
    die("Found mediaobject with no $ref_fmt imagedata : ".$media->toString);
  }
  my $filebase = imgData($have_fmts{$ref_fmt})->getAttribute('fileref');
  $filebase =~ s/\.$all_fmts{$ref_fmt}[0]$//;
  foreach my $fmt (keys %all_fmts) {
    if (!$have_fmts{$fmt}) {
      my $fileref = $filebase . "." . $all_fmts{$fmt}[0];
      print STDERR "Format not present : $fmt ($fileref)\n";

      my $newimg = $have_fmts{$ref_fmt}->cloneNode(1);
      my $newdata = $newimg->getElementsByTagName('imagedata')->item(0);
      $newdata->setAttribute('fileref', $fileref);
      $newdata->setAttribute('format', $fmt);
      $media->appendChild($newimg);

      $have_fmts{$fmt} = $newimg;
    }
    $have_fmts{$fmt}->setAttribute('role', $all_fmts{$fmt}[1]);
  }
}
print $doc->toString;

sub imgData
{
  my $imageobject = shift;
  return $imageobject->getElementsByTagName('imagedata')->item(0);
}
