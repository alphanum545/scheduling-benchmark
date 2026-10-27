# External benchmark protocol: WfInstances (written before any full run)

Purpose: test the frozen CGS on workflows that are not part of this repository, with no tuning.

## Data

- Source: [WfCommons WfInstances](https://github.com/wfcommons/WfInstances), commit `4c3958e94f99e12c2663d4e093eb41731ca7496c`
  (WfFormat schema 1.6 execution traces of real scientific workflows). Not redistributed here; clone it to reproduce.
- Selection: every trace with 20 ≤ n ≤ 2000 tasks (287 workflows in 17 families: Pegasus 1000Genome, Cycles,
  Epigenomics, Montage, Seismology, SoyKB, SRASearch; Makeflow BLAST, BWA; Nextflow legacy, ro-crate, Sarek
  germline and somatic; Snakemake RASflow, RNA-seq STAR DESeq2, varlociraptor; StreamFlow). The bound keeps the
  dense n × n input format manageable and matches this repository's largest instances (2,081 tasks).
- Settings per workflow: P ∈ {4, 16} processors × CCR ∈ {0.1, 1, 10} × heterogeneity β ∈ {0.5, 1.0}: 12 instances,
  3,444 in total.

## Conversion (`wf2d3.py`, fixed in this commit)

Task graph and runtimes from the trace; edge data = sizes of files passed from parent to child; runtimes
scaled to mean 100; w(i,k) = base · U(1 − β/2, 1 + β/2) per task and processor (Topcuoglu et al. 2002);
c(i,j) proportional to edge data, scaled so that mean edge cost / mean computation cost = CCR; uniform links;
seeds from the trace name, P and β. Every schedule is checked with `tools/evaluate.cpp check`.

## Algorithms (sources as of this commit; none is modified for this benchmark)

- Candidate: `cgs` (frozen `experiments/cgs/cgs.cpp`, SHA-256 `2f6db80d…`), defaults.
- Ablations: `cgs single` (no joint placement), `cgs credit=0` (plain rank gate).
- Published baselines with insertion: `heft_ins`, `aeft_ins`, `peft_ins`, `bppts_ins`, `ippts_ins`, `la_heft`
  (Look-ahead HEFT); `scct_ins` (SCCT with insertion); repository `heft` (non-insertion) for reference.

## Analysis (fixed now)

- Primary: mean paired gain of CGS, 100 · (baseline − CGS) / baseline, against `aeft_ins` (strongest published
  baseline on the internal test split) and `heft_ins`, with wins / ties / losses.
- Test: two-sided Wilcoxon signed-rank on per-workflow means (each workflow averaged over its 12 settings,
  287 pairs); Holm adjustment across all baseline comparisons reported.
- Because Sarek germline and somatic contribute 85 of 287 workflows, also report the family-macro mean
  (each of the 17 families weighted equally) and per-family gains.
- Secondary: gains by CCR, P and β; the two ablations; runtime (sandbox, not citable).
- Reported whatever the outcome; no configuration of any algorithm is changed after seeing results.
