#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

samtools view $1 $2 | perl -lane 'print join "\t", ($F[0], $F[1], $F[2], $F[3], $F[11], $F[12], $F[13], $F[14], $F[15], $F[16], $F[17], $F[18], $F[19], $F[20], $F[21], $F[22])'  | perl -lane '$_ =~ s/[A-Z][A-Z]:i://g; print' > $DIR/tmp.dot.dat.txt

R --vanilla < $DIR/plotDot.R --args $DIR/tmp.dot.dat.txt

#rm $DIR/tmp.dot.dat.txt