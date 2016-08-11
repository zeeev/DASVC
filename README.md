# DASVC
A tool for calling SVs from genome-wide contig alignments to a reference genome genome alignments.

# Dependancies (for the full pipeline)

The dependancies are only required if you'd like to run the whole pipeline.  Otherwise, you can build you own netted SAM file and run the last step.

1. [LASTZ](http://www.bx.psu.edu/miller_lab/dist/README.lastz-1.02.00/README.lastz-1.02.00a.html). For aligning contigs to a reference genome.
2. [LAST](http://last.cbrc.jp). Conversion from MAF to SAM
3. [UCSC](https://github.com/ENCODE-DCC/kentUtils/tree/master/src). File conversion, chaining and netting.
3. [SNAKEMAKE](https://bitbucket.org/snakemake/snakemake/wiki/Home). Running the pipeline.
