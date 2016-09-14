#!/bin/bash

bedtools closest -t all -d -a $1 -b $2 | perl -lane '$F[-4] =~ s/\|[0-9].*//g; print join "\t", (@F[0..10], $F[-4], $F[-1])' | bedtools groupby -g 1,2,3,4,5,6,7,8,9,10,11,12 -c 13 -o min > $3