# DASVC - De-novo Assembly Structural Variant Caller 
A tool for calling SVs from genome-wide contig alignments.

# What does the pipeline do?

1. Aligns a genome (query) to a reference genome (target) using LastZ.
2. Chains the query genome against the target genome using UCSC
3. Nets the chains.  This resolves overlapping alignments and inversions.
4. netToMaf mafToSam samToBam
5. Sorts the bam by query 
6. Annotates the bam file and chains the query.
7. Calls Structural Variants.

# Dependancies (for the full pipeline)

The dependancies are only required if you'd like to run the whole pipeline.  Otherwise, you can build you own netted SAM file and run the last step.

1. [LASTZ](http://www.bx.psu.edu/miller_lab/dist/README.lastz-1.02.00/README.lastz-1.02.00a.html). For aligning contigs to a reference genome.
2. [LAST](http://last.cbrc.jp). Conversion from MAF to SAM
3. [UCSC](https://github.com/ENCODE-DCC/kentUtils/tree/master/src). File conversion, chaining and netting.
4. [SNAKEMAKE](https://bitbucket.org/snakemake/snakemake/wiki/Home). Running the pipeline.

# Installing
0. Install dependancies
1. get and build code

```
git clone --recursive https://github.com/zeeev/DASVC.git
cd DASVC
make
```

2. Export paths to binaries 
3. Modify the config file: "pipeline/config.json"
4. Run Snakemake


# Annotations added to BAM file

1. "QS" Query start
2. "QE" Query end
3. "QL" Query length 
4. "TS" Target start
5. "TE" Target end
4. "MB" Number matching bases in block (=|M)
5. "PI" Percent identity 
6. "BI" Block ID (equivalent to query name)
9. "TM" Total matching bases in query sequence
10. "AI" Query order 1,2,3....
