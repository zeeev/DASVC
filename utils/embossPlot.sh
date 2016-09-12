#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

samtools faidx $1 $2 > $DIR/tmp1.fa
samtools faidx $3 $4 > $DIR/tmp2.fa

dotmatcher -stretch -windowsize $5 -threshold $6 -xygraph ps -asequence $DIR/tmp1.fa -bsequence $DIR/tmp2.fa

#example :  sh embossPlot.sh /net/eichler/vol18/zevk/great_apes/genome_wide_alignments/susie3_human/large_contigs/GSMRT3.2.masked.large.fasta 000887F_quiver_patched:1057097-1057309  ~/shared_resources/assemblies/hg38/ucsc.hg38.fasta chr1:3501776-3502054 200 25