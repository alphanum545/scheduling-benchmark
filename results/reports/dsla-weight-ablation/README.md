# DSLA score-weight ablation: is the extra λ·w(t,k) term needed?

**Question.** DSLA's processor score is

    Score(t,k) = (1 − λ)·EFT(t,k) + λ·[ max_{c∈K(t)} L(c|k) + w(t,k) ]

`L(c|k)` starts the critical child after `EFT(t,k)`, and `EFT(t,k)` already contains `w(t,k)`.
So `w(t,k)` enters the λ-term twice. Writing `L = EFT + Δ` (Δ = predicted tail beyond t's finish),
the score is `EFT + λ·Δ + λ·w(t,k)`: EFT keeps a unit coefficient, and the extra part is an explicit
`λ·w(t,k)` penalty on processors that are slow for t. This report tests whether removing it helps.

**Variant.** `algorithms/makespan/dsla_fix.cpp` (`dsla_fix`, `slas_fix` in `algorithms.toml`) is
`dsla.cpp` with one change: `score = (1 − λ)·EFT + λ·max L(c|k)`. Everything else is identical
(same λ = 0.5 / 0.1, R = 3, IOCT table, ranks, tie-breaking). `dsla.cpp` itself is unchanged, so its
stored results (hash `4d0137b23a7e`) remain valid.

**Note on where cost repetition matters.** Every quantity in DSLA that estimates a *time*
(EFT, IOCT, L(c|k)) counts each task's cost exactly once; unlike OPPTS's PCG table, which adds each
child's computation cost twice. The λ·w(t,k) term lives only in the processor-selection score,
which is a weighted decision criterion, not a time estimate.

## Result: removing the term makes DSLA worse

All 2814 instances (6 workflow families + 9 random CCR × β families), every run valid.
gain = (variant − original) / variant; positive = original DSLA has the shorter makespan.

| Comparison | DSLA shorter | Equal | Variant shorter | Mean gain | Median gain [95% CI] | Wilcoxon (Holm) | Per-DAG p |
|:--|--:|--:|--:|--:|--:|--:|--:|
| DSLA vs DSLA_fix | 1544 (54.9%) | 355 (12.6%) | 915 (32.5%) | +1.03% | +0.27% [0.18, 0.34] | 3.4e−44 | 4.7e−32 |
| SLAS vs SLAS_fix | 63.6% | 6.4% | 30.0% | +2.35% | — | — | — |

The effect is the same on the dev split (+0.96%) and the held-out test split (+1.05%).

Against baselines, removing the term shrinks DSLA's advantage everywhere (mean gain, `compare` convention
(baseline − algo)/baseline): HEFT +6.32% → +5.14%, OPPTS +5.44% → +4.40%, PEFT +10.18% → +9.34%;
per-instance wins against the best of 12 baselines fall from 47.5% to 35.4%.

### Where the term helps and where it doesn't
- Strongest on large scientific workflows: Genome +1.40%, LIGO +1.47%, Montage +1.45% (SLAS: +3.4%, +3.7%, +2.8%).
- Slightly harmful on GE (−1.20%) and LA (−0.32%), which are small (60 instances each).
- Random DAGs, CCR 0.1 and 1: the benefit grows with processor heterogeneity β
  (CCR 0.1: −0.02% → +0.40% → +0.93% for β = 0.1, 0.5, 1; CCR 1: −0.01% → +0.33% → +0.67%).
  This is the signature of a heterogeneity penalty: with near-identical processors w(t,k) barely varies by k and the term is inert.
- Random DAGs, CCR 10: effect is small and not monotone in β (DSLA: +0.10%, +0.18%, −0.32%);
  communication dominates and the look-ahead/EFT terms drive decisions.

Full per-family and per-split tables: run `python3 results/reports/dsla-weight-ablation/ablation_tables.py`
(reproduced below). The standard report files (`summary.md`, `summary.xlsx`, `results.csv`, `figures/`)
were produced by `tools/report.py --algos dsla,dsla_fix,slas,slas_fix,heft,peft,oppts --highlight dsla`.

## Conclusion
Keep the term, and describe it as a deliberate heterogeneity weight rather than part of the look-ahead:

    Score(t,k) = (1 − λ)·EFT(t,k) + λ·LA(t,k) + λ·w(t,k),   LA(t,k) = max_{c∈K(t)} L(c|k)

It is not an accidental cost repetition: the time estimates are clean, and the ablation shows the explicit
weight on w(t,k) reduces makespan significantly (p < 1e−31 per DAG), mainly on heterogeneous, low-to-moderate-CCR instances.

## Provenance
- Run locally on 26 Oct 2026 (2 workers, 222 s), not via the GitHub Actions Benchmark workflow.
  Rerun publicly with **Benchmark** → algos `dsla,dsla_fix,slas,slas_fix` before citing in the paper;
  the stored results will be reused for `dsla`/`slas` and the variants will be recomputed identically.
- Random families regenerated from `tools/generated.json` (`gen:random-v1`, seed 1); lower bound and
  sequential time matched `data_index.csv` for all 2814 instances.
- New rows appended to `results/store.csv`: `dsla_fix` and `slas_fix`, 2814 each.


### dsla vs dsla_fix

| group | n | original shorter % | equal % | variant shorter % | mean gain % |
|:--|--:|--:|--:|--:|--:|
| all | 2814 | 54.9 | 12.6 | 32.5 | +1.028 |
| split dev | 712 | 54.9 | 14.2 | 30.9 | +0.960 |
| split test | 2102 | 54.9 | 12.1 | 33.1 | +1.051 |
| FFT | 60 | 60.0 | 6.7 | 33.3 | +0.840 |
| GE | 60 | 30.0 | 21.7 | 48.3 | -1.200 |
| Genome | 600 | 63.7 | 6.0 | 30.3 | +1.395 |
| LA | 60 | 36.7 | 16.7 | 46.7 | -0.319 |
| LIGO | 660 | 53.5 | 10.6 | 35.9 | +1.471 |
| Montage | 654 | 62.8 | 11.2 | 26.0 | +1.445 |
| R-ccr0.1-b0.1 | 80 | 48.8 | 8.8 | 42.5 | -0.019 |
| R-ccr0.1-b0.5 | 80 | 45.0 | 16.2 | 38.8 | +0.400 |
| R-ccr0.1-b1 | 80 | 50.0 | 16.2 | 33.8 | +0.928 |
| R-ccr1-b0.1 | 80 | 47.5 | 13.8 | 38.8 | -0.008 |
| R-ccr1-b0.5 | 80 | 60.0 | 3.8 | 36.2 | +0.328 |
| R-ccr1-b1 | 80 | 62.5 | 2.5 | 35.0 | +0.669 |
| R-ccr10-b0.1 | 80 | 18.8 | 68.8 | 12.5 | +0.099 |
| R-ccr10-b0.5 | 80 | 35.0 | 36.2 | 28.7 | +0.175 |
| R-ccr10-b1 | 80 | 35.0 | 20.0 | 45.0 | -0.315 |

### slas vs slas_fix

| group | n | original shorter % | equal % | variant shorter % | mean gain % |
|:--|--:|--:|--:|--:|--:|
| all | 2814 | 63.6 | 6.4 | 30.0 | +2.345 |
| split dev | 712 | 64.6 | 6.9 | 28.5 | +2.281 |
| split test | 2102 | 63.2 | 6.2 | 30.5 | +2.367 |
| FFT | 60 | 65.0 | 3.3 | 31.7 | +1.715 |
| GE | 60 | 38.3 | 15.0 | 46.7 | -0.690 |
| Genome | 600 | 70.3 | 2.5 | 27.2 | +3.427 |
| LA | 60 | 38.3 | 15.0 | 46.7 | -0.066 |
| LIGO | 660 | 68.0 | 2.3 | 29.7 | +3.723 |
| Montage | 654 | 72.3 | 4.7 | 22.9 | +2.757 |
| R-ccr0.1-b0.1 | 80 | 48.8 | 8.8 | 42.5 | -0.019 |
| R-ccr0.1-b0.5 | 80 | 45.0 | 16.2 | 38.8 | +0.400 |
| R-ccr0.1-b1 | 80 | 50.0 | 16.2 | 33.8 | +0.928 |
| R-ccr1-b0.1 | 80 | 48.8 | 12.5 | 38.8 | +0.015 |
| R-ccr1-b0.5 | 80 | 60.0 | 2.5 | 37.5 | +0.388 |
| R-ccr1-b1 | 80 | 62.5 | 2.5 | 35.0 | +0.634 |
| R-ccr10-b0.1 | 80 | 33.8 | 33.8 | 32.5 | -0.001 |
| R-ccr10-b0.5 | 80 | 42.5 | 17.5 | 40.0 | -0.182 |
| R-ccr10-b1 | 80 | 58.8 | 13.8 | 27.5 | +0.658 |
