#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

bedtools intersect -a $1 -b $2 -u | perl -lane 'print "$_\tcoding_variant"'     >  tmp.bed
bedtools subtract  -a $1 -b $2 -A | perl -lane 'print "$_\tnon_coding_variant"' >> tmp.bed

sort -k1,1 -k2,2n tmp.bed > $3


