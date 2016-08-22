# DASVC - De-novo Assembly Structural Variant Caller 
A tool for calling SVs from genome-wide contig alignments.

# What the pipeline does

1. Aligns a genome (query) to a reference genome (target) using LastZ.
2. Chains the query genome against the target genome using UCSC
3. Nets the chains.  This resolves overlapping alignments and inversions.
4. Chains the query genome, to remove fills.
5. Calls Structural variants.


# Dependancies (for the full pipeline)

The dependancies are only required if you'd like to run the whole pipeline.  Otherwise, you can build you own netted SAM file and run the last step.

1. [LASTZ](http://www.bx.psu.edu/miller_lab/dist/README.lastz-1.02.00/README.lastz-1.02.00a.html). For aligning contigs to a reference genome.
2. [LAST](http://last.cbrc.jp). Conversion from MAF to SAM
3. [UCSC](https://github.com/ENCODE-DCC/kentUtils/tree/master/src). File conversion, chaining and netting.
3. [SNAKEMAKE](https://bitbucket.org/snakemake/snakemake/wiki/Home). Running the pipeline.

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
2. "QL" Query length 
3. "MB" Number matching bases in block (=|M)
4. "PI" Percent identity 
5. "BI" Block ID (equivalent to query name)
6. "AI" Alignment block ID sorted by query start "QS"
7. "NB" The number of aligment blocks for the query sequence
8. "TM" Total matching bases in query sequence
9. "FA" Special field. The QS,QE, and strand for the query before and after the current alignment (sorted by query start).
