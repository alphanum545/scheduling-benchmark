# Bounded bottleneck-closure repair (BCR)

**Status: development benchmark and supplemental audit completed successfully. Publication novelty and unseen-corpus validation remain open.**

Start with [DEV_RESULTS_20261026.md](DEV_RESULTS_20261026.md) for the measured findings, runtime tradeoff and limitations. [PAIRED_STATISTICS.md](PAIRED_STATISTICS.md) preserves small p-values in scientific notation; the first automatic supplemental Markdown renderer rounded them to zero.

## Evidence

- [PROTOCOL.md](PROTOCOL.md): pre-execution questions, frozen budgets and controls.
- [Candidate source](../../algorithms/makespan/bcr.cpp): standalone SCCT/DSLA seed controls and exact bounded repair.
- [test_bcr.cpp](test_bcr.cpp) and [UNIT_TESTS.txt](UNIT_TESTS.txt): correctness tests, not benchmark performance data.
- [Measured report](../../results/reports/bcr-dev-v1/summary.md): validity, primary metrics and tests.
- [Pairwise numeric source](../../results/reports/bcr-dev-v1/bcr_pairwise.csv), [family table](../../results/reports/bcr-dev-v1/bcr_families.csv), and [manifest](../../results/reports/bcr-dev-v1/bcr_analysis_manifest.json).
- [analyze.py](analyze.py) and [POSTPROCESSING.md](POSTPROCESSING.md): reproducible supplementary analysis and report-only provenance.

Measurement run: https://github.com/alphanum545/scheduling-benchmark/actions/runs/40312108945

Report-only audit: https://github.com/alphanum545/scheduling-benchmark/actions/runs/40312825348

The measurement run forcibly executed all selected methods. The audit scheduled zero new measurements and preserved the raw CSV blob. No test-split evaluation occurred in this study. No changes to main, baseline source files, workflow YAML, or private paper repositories.

## Reproduce correctness checks

```sh
g++ -O1 -g -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer experiments/bottleneck-repair/test_bcr.cpp -o /tmp/test-bcr
/tmp/test-bcr
```

The measured source Git blob is b53cb158b69baacada2eafca44fb06c34240d46a. The source SHA-256 and all observed algorithm hashes are retained in the audit manifest. Correctness and non-worsening concern the stated static cost model, not noisy execution or a claim of global optimality.
