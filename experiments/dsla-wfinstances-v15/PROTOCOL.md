# DSLA WfInstances v1.5 external-family validation - precommitted protocol

**Date:** 2026-10-24. **No performance outcomes examined for this corpus before selecting it.** Original DSLA is frozen at `alphanum545/scheduling-benchmark@2f078d86b9d39835756f6ed9613279418a52371b`, `algorithms/makespan/dsla.cpp`, SHA-256 `5be7070a3a0428e37babab2c7a932b455eb504c753ce4549c9bc47cccb448799`.

## Source and eligibility (outcome-blind)

Use the **public** `wfcommons/WfInstances` Git tag `v1.5`, specifically all JSON files under these five Pegasus application directories: `1000genome`, `cycles`, `seismology`, `soykb`, `srasearch`. These names are not among the six families in the previous development-exposed benchmark (`FFT`, `GE`, `Genome`, `LA`, `LIGO`, `Montage`). Include **every** listed file that has a uniquely identified nonempty acyclic task specification (<=2500 tasks), consistent parent/child adjacency and a complete finite nonnegative measured task-runtime vector. Exclusions must be itemized with reasons **without observing any scheduling results**. Do not select results after computation, do not tune DSLA or baselines. Raw WfInstances records are from Pegasus execution logs on Chameleon, not direct measurements of this proposed heterogeneous model. Source file and model input SHA-256 per case are retained.

## Deterministic heterogeneity and transfer model

For each observed task runtime `r_i` seconds, processor count `p in {1,2,4,8,16}`, let the task execution cost (integer milliseconds) be `max(1, round_half_up(1000*r_i/(speed_k*affinity(trace_filename,task_id,k))))`; `speed_k` is a fixed prefix of `(0.72,1.00,1.28,0.84,1.44,0.94,1.15,0.77,1.36,0.88,1.09,1.52,0.81,1.20,0.99,1.41)`. Deterministic `affinity` is `(85 + first_32bit_unsigned_integer(SHA256(filename + '|' + task_id + '|' + processor_index)) % 31)/100`. Task identifiers are sorted in lexicographic Kahn topological order before matrix construction. Edge transmission `c_ij` is rounded up integer milliseconds from sizes of **shared parent-output/child-input file IDs** using `125 MiB/s` or `1000 MiB/s`; missing file metadata contributes zero and is counted; same-processor transfer is zero. The complete matrix follows D3 format and is SHA-256 hashed. **These heterogeneous execution/transfer costs are modeled, not measured on real heterogeneous processors.**

## Fixed methods and validity

Evaluate the compiled **original** DSLA and the previously conformance-checked `artifacts/08-dsla-end-to-end/engine.cpp` on the exact same D3 inputs. Check exact agreement of original executable versus engine DSLA schedules. Compare against engine `heft_insertion_reimplementation`, `peft_insertion_reimplementation`, `portfolio_heft_peft` (equal two-build budget), `portfolio_static`, `portfolio_no_offset`, `portfolio_all_children`. All entire schedules must be validated independently for precedence, communication delay, nonpreemptive duration, processor overlap and coverage. Persist every schedule; any discrepancy is a recorded failure, not a silently dropped observation. Original source, benchmark `main` and its cache are unchanged.

## Fixed analysis

The **primary** comparison includes p=2,4,8,16 crossed with both bandwidth values; p=1 is a fixed single-processor reference for modeled speedup/efficiency. For each comparator on each exact trace and cost setting, compute `100*(1-Cmax_DSLA/Cmax_B)`; win/tie/loss; tail minimum; runtime of **both** DSLA builds. Average over p and bandwidth **within trace**, average across traces **within each application family**, then give the five families equal weight in the primary headline. Compute 95% family-stratified bootstrap intervals by resampling traces with replacement within each family (4000 resamples, fixed seed 20261024). Report an instance-weighted sensitivity analysis, all five family results and losses. No universal-superiority claim when CI overlaps zero.

## Claim and independence boundaries

The five source families are **new to the archived DSLA benchmark** but there is no established fact that they were never considered by a human researcher before; call this an **externally sourced application-trace validation with precommitted outcome-blind inclusion**, not a certified pristine blind holdout. The simulated makespan is not the original observed cloud makespan. HEFT and PEFT are explicit insertion-based reimplementations, not author-certified source code. Abstract-level AEFT/BPPTS/MPPTS references are not publication-fidelity-certified and must not be relabeled as such.
