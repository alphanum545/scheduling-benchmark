# SCCT fork-neighborhood pilot: development benchmark

Date: 2026-10-26  
Repository: `alphanum545/scheduling-benchmark`  
Branch: `research/scct-joint-pilot-20261026`  
Benchmark run: https://github.com/alphanum545/scheduling-benchmark/actions/runs/40309719850  
Submitted source revision: `0a6f410833b892e29b55f1224c7070b8eba08054`  
Report: `results/reports/scct-joint-dev-v1/summary.md`  
Paired observations: `results/reports/scct-joint-dev-v1/results.csv`  
Scope: **development split only; do not treat as unseen validation.**

## Execution and validity

The existing manual `Benchmark` workflow was used with its unchanged YAML and 20 shards. Workflow status: **success**. All 11 algorithms produced valid schedules on all 712 compared instances, yielding 7,832 report rows. The pilot registry is branch-only; the original `algorithms.toml` was preserved at `experiments/scct-joint/algorithms_original.toml`, and `main` was not modified.

Original SCCT and the refactored `scct_joint_base` have **identical makespans on every one of the 712 instances** (confirmed by matching `path` in `results.csv`). This is a makespan-equivalence check, not an assertion of identical full schedule traces.

## Primary ablation

The mean paired improvement is the unweighted mean over instances of
`100 * (makespan_SCCT - makespan_variant) / makespan_SCCT`, distinct from the percent reduction in the arithmetic average makespan.

| Algorithm | Average makespan | Mean paired gain vs SCCT | Mean runtime (s) |
|:--|--:|--:|--:|
| scct | 4610.77 | baseline | 0.0333 |
| scct_joint_base | 4610.77 | 0 | 0.0399 |
| scct_diamond | 4610.77 | 0 | 0.0403 |
| scct_joint | 4605.78 | +0.094 percent | 0.7093 |
| scct_joint_diamond | 4605.78 | +0.094 percent | 0.7558 |

Across all 712 matched instances, `scct_joint_diamond` is better/tied/worse than original SCCT on **279/212/221**, respectively. The report's two-sided Wilcoxon comparison gives Holm-adjusted `p = 0.0586`, and the dependency-aware per-DAG adjusted comparison gives `p = 0.776` (409 DAGs). Neither supports a statistically robust advantage. The observed runtime overhead is approximately 23-fold for combined vs original SCCT (ratio of mean runtimes).

Exact-makespan equivalence: `scct_diamond` vs `scct` = 712/712; `scct_joint_diamond` vs `scct_joint` = 712/712. Consequently the narrow isolated-diamond predictor contributes no observed makespan improvement here; whether eligible motifs were absent or the added term never changed the selected processor is not established.

## Per-family paired results

Recomputed from `results/reports/scct-joint-dev-v1/results.csv` by exact `path` joins. Positive means `scct_joint_diamond` is shorter. Counts are candidate wins/ties/losses, not win rates. Data are all **development** cases.

| Family | n | Mean paired gain (percent) | W / T / L |
|:--|--:|--:|:--|
| FFT | 10 | +1.19 | 8 / 1 / 1 |
| GE | 16 | +0.075 | 2 / 8 / 6 |
| Genome | 158 | +0.028 | 39 / 95 / 24 |
| LA | 13 | -0.33 | 2 / 8 / 3 |
| LIGO | 156 | +0.25 | 66 / 35 / 55 |
| Montage | 178 | +0.13 | 84 / 24 / 70 |
| R-ccr0.1-b0.1 | 21 | -0.034 | 5 / 11 / 5 |
| R-ccr0.1-b0.5 | 26 | -0.016 | 5 / 15 / 6 |
| R-ccr0.1-b1 | 21 | +0.012 | 5 / 12 / 4 |
| R-ccr1-b0.1 | 17 | -0.19 | 8 / 0 / 9 |
| R-ccr1-b0.5 | 27 | +0.20 | 16 / 2 / 9 |
| R-ccr1-b1 | 16 | +0.56 | 14 / 0 / 2 |
| R-ccr10-b0.1 | 19 | -0.29 | 8 / 0 / 11 |
| R-ccr10-b0.5 | 18 | -0.12 | 9 / 1 / 8 |
| R-ccr10-b1 | 16 | -0.65 | 8 / 0 / 8 |

The high-CCR weakness remains: all three CCR=10 random families show negative mean paired gains. This is particularly problematic because it was the main motivation for the proposed method.

## Comparisons with strong baselines

Using the report's *paired mean gain* convention, `scct_joint_diamond` compared with **DSLA** gives **-4.898 percent** (candidate loses on 527, ties on 14, and wins on 171 of 712). Compared with OPPTS it gives +1.076 percent, with HEFT +1.812 percent, and with LEFT +4.05 percent. These comparisons do not establish superiority to the strongest comparator.

## Research decision

**Do not freeze or submit this pilot as a new algorithm.** The bounded joint search is computationally expensive and its dev-set gain is too small and not robust. The isolated diamond heuristic produces no distinguishable makespan outcomes in this corpus. No held-out test benchmark was run on these candidates, preserving the test split for a substantially revised mechanism.

Potential next hypothesis (unvalidated): state-aware contention and processor availability, or a more general graph-region (non-isolated join) forecast. Construct minimal decision-changing counterexamples first and run development ablations before further test use.

Sources: the linked GitHub Actions run; `results/reports/scct-joint-dev-v1/summary.md`; `results/reports/scct-joint-dev-v1/results.csv`. No new measurements were invented.
