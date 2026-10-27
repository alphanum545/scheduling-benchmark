# CGS: Contention-table, Gated join-closure Scheduling

Unifies two new methods, SCCT (sibling-contention cost table) and RCIS (joint placement of a join's parents), with insertion placement and a communication-credited priority gate. Without sibling contention, its cost table is AEFT's IOCT (Wang et al., FGCS 164 (2025) 107576, doi:10.1016/j.future.2024.107576); see RESULTS.

- [RESULTS_20261027.md](RESULTS_20261027.md): algorithm, development ablations, held-out test results and limits. Start here.
- [PROTOCOL.md](PROTOCOL.md): configuration frozen before the single test-split run (commit `0001f93`).
- [cgs.cpp](cgs.cpp): the scheduler; options documented in its header.
- [aeft_ins.cpp](aeft_ins.cpp): AEFT with the paper's insertion policy (fair comparator); reproduces the paper's worked example (makespan 76); `append` reproduces repository AEFT.
- [peft_ins.cpp](peft_ins.cpp), [ippts_ins.cpp](ippts_ins.cpp), [bppts_ins.cpp](bppts_ins.cpp): repository baselines with insertion placement only; `append` reproduces each original.
- [heft_ins.cpp](heft_ins.cpp): repository HEFT with insertion (fair comparator); `append` reproduces repository HEFT.
- [scct_x.cpp](scct_x.cpp): SCCT with `ins`, `lam=`, `rank=heft` options used in the dev study; defaults reproduce `algorithms/makespan/scct.cpp`.
- [external/](external/): WfInstances benchmark (287 real workflows, 3,444 instances): protocol, converter, runner, [results](external/RESULTS.md).
- `results/`: per-instance dev (`dev_results.csv`) and test (`test_results.csv`) measurements and text summaries.
- [run.py](run.py), [cmp.py](cmp.py), [fam.py](fam.py), [summ.py](summ.py): local runner and analysis scripts.

Runs were local, with every schedule checked by `tools/evaluate.cpp`. `main`, baseline sources and workflow YAML are unchanged.
