# RCIS commitment-timing repair (`rcis_gated`)

Continues the standalone RCIS line (`experiments/rcis-standalone/`), independent of DSLA, SCCT and BCR. Tests why RCIS joint closures lost to single-task placement and repairs it with a priority gate.

- [RESULTS_20261027.md](RESULTS_20261027.md): findings, ablations, mechanism evidence and limits. Start here.
- [rcis_gated.cpp](rcis_gated.cpp): RCIS plus `--commit`, `--gate`, `--fallback` options; default flags reproduce `rcis.cpp` exactly.
- [results/dev_results.csv](results/dev_results.csv): all per-instance development measurements (16 configurations x 712 instances).
- [results/vs_single.txt](results/vs_single.txt), [results/vs_gated.txt](results/vs_gated.txt): paired summaries.
- [run.py](run.py), [cmp.py](cmp.py), [trace.py](trace.py): local runner, paired comparison, inversion trace.

Development split only. `main`, baseline sources and workflow YAML are unchanged.
