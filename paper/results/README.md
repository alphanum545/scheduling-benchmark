# Results section: figures and tables

Rebuild everything with `python3 paper/results_figures.py`. The figures (`fig_*.png` / `fig_*.pdf`) and
`.csv` files are not committed because they are large; the script regenerates them in a few seconds from
the committed benchmark results. The input is `results/reports/full-contention/results.csv`: all 2,814
instances, every algorithm valid on every instance, and all algorithms run in the same benchmark run. The
makespans are the same as in `full-all-instances-v2`. In that run DSLA appears as `z_best10`, its lab id;
the schedules are identical (report `dsla-equivalence`).

## Algorithms compared

The paper compares DSLA with six baselines:

| Baseline | Year, venue | Why it is in |
|---|---|---|
| HEFT | 2002, IEEE TPDS | The standard reference; DSLA uses its upward rank |
| PPTS | 2019 | The look-ahead-table family (predict cost matrix) |
| IPPTS | 2021, IEEE TPDS | PPTS's successor; raised by Reviewer 3 |
| PMMS | 2023, J. Grid Computing | Requested by Reviewer 5 |
| AEFT | 2025, FGCS | The newest table-based method; DSLA reuses its IOCT table |
| LEFT | 2025, Cluster Computing | The strongest baseline in the benchmark; global earliest-finish selection |

The benchmark also contains PEFT (2014), IPEFT (2017), MPEFT (2021), LSTD (2020), BPPTS (2025), LEFT-D
(2025, LEFT with entry-task duplication) and OPPTS (our earlier, unpublished method). They belong in the
related work only. To add any of them to the figures, add it to `NAMES` in the script. SLAS, DSLA's
rank-order schedule alone, belongs to the ablation.

## Figures

| File | Content |
|---|---|
| `fig_makespan_vs_tasks_workflows` | Average makespan vs number of tasks, one panel each for Montage, Genome, LIGO, FFT, GE, LA |
| `fig_makespan_vs_processors_workflows` | Average makespan vs number of processors, same panels |
| `fig_slr_vs_{tasks,processors}_workflows` | Average SLR, same panels |
| `fig_speedup_vs_{tasks,processors}_workflows` | Average speedup, same panels |
| `fig_efficiency_vs_{tasks,processors}_workflows` | Average efficiency, same panels |
| `fig_*_random` | The same eight figures for the random graphs, one panel per CCR (0.1, 1, 10) |
| `fig_gain_by_ccr` | DSLA's mean makespan reduction over each baseline on the random graphs, per CCR |

## Tables

| File | Content |
|---|---|
| `tab_average_makespan` | Average makespan per family, per algorithm, plus all workflows, all random, all instances |
| `tab_pairwise` | DSLA vs each baseline: % of instances better / equal / worse, per workflow family, random graphs and Combined |
| `tab_significance` | Friedman test with Nemenyi critical difference; Wilcoxon signed-rank, DSLA vs each baseline (Holm-corrected, also with one observation per DAG) |
| `tab_significance_by_family` | The Wilcoxon test within each family |
| `tab_efficiency_by_p` | Efficiency per processor count: DSLA, mean and best of the six baselines, and DSLA's speedup |
| `tab_gain_by_characteristics` | DSLA's gain per CCR, heterogeneity β, task count and processor count: against the strongest baseline in that group, and against the per-instance best of the six |
| `tab_runtime` | Mean wall time per run on the random graphs; see the caution below |

## Points for writing the results

- **Overall.** DSLA has the lowest average makespan in all 15 families: 6 workflow families and 9 random
  families. Over all 2,814 instances it averages 4469.6, against 4627.9 for the strongest baseline, LEFT.
  Per instance it is shorter on 85.5% (vs AEFT) to 98.1% (vs IPPTS) of instances.
- **Significance.** Friedman: χ² = 7748.6, p ≈ 0; Nemenyi critical difference 0.170 ranks. DSLA's average
  rank is 1.63, and the next are AEFT 3.63, PMMS 3.70 and HEFT 3.76. Against every baseline the Wilcoxon
  test is significant after Holm correction and still significant with one observation per DAG. The
  mean gains are 6.3% (HEFT), 6.4% (AEFT), 6.6% (PMMS), 8.3% (LEFT), 8.3% (PPTS) and 27.7% (IPPTS).
  Within single families, 86 of the 90 family × baseline tests are significant. The 4 exceptions are
  ties in DSLA's favour: GE vs PPTS (p = 0.09), CCR 0.1 / β 0.1 vs AEFT and vs PPTS, and CCR 1 / β 0.1 vs
  HEFT. DSLA is never significantly worse.
- **Where the margin is thin.** Against AEFT and PPTS on GE and LA, DSLA is better on only 58% to 73% of
  instances, even though its average is lower. On the smallest workflows (9 to 137 tasks) the gain over
  the strongest baseline is small, 0.3% to 4%. It grows with graph size: 5.5% to 8.9% for the Montage,
  Genome and LIGO sizes of 300 to 1,000 tasks. FFT, GE and LA at 512 to 529 tasks gain 1.1% to 3.7%.
- **Graph characteristics.** On the random graphs the gain over the strongest baseline grows with CCR:
  0.55% at CCR 0.1, 1.1% at CCR 1, 3.2% at CCR 10. It also grows with heterogeneity β: 5.6%, 5.9% and 6.9%.
  The strongest baseline changes with the group: PMMS at CCR 0.1, HEFT at CCR 1, LEFT at CCR 10.
- **Against the per-instance best of the six.** Picking, after the fact, the best of the six baselines for
  each instance is an oracle no single method can match. DSLA still beats that oracle on average in most
  groups (for example +3.2% at CCR 10, +1.1% to +1.6% for workflows of 600 to 1,000 tasks). It is level or slightly
  behind only on the smallest workflows and at p = 2 and p = 64.
- **Efficiency vs p (Reviewer 5, point 3).** Efficiency is speedup / p, with speedup = (best single-
  processor sequential time) / makespan, the standard HEFT definition. It can exceed 1 on the
  heterogeneous workflow processors, because the best single processor is slower than the mix of fast
  processors the scheduler uses. On every family efficiency falls once p exceeds the graph's
  exploitable parallelism. The cause is the workload, not DSLA: the mean and the best baseline fall at
  the same rate, and DSLA stays at or above the best baseline at every p.
  - Workflows: efficiency peaks at p = 16 (2.41) and drops to 0.82 at p = 64.
  - Random graphs: efficiency falls from 0.77 at p = 4 to 0.19 at p = 32, where speedup saturates near 6.
  - A practical answer for the reviewer: past p = 16 on the workflows, and past p = 8 on the random
    graphs, extra processors add little speedup for any of the seven algorithms.
- **Runtime caution.** `runtime_s` is the wall time of a whole run: process start, reading the n × n
  input matrix and scheduling. The baselines' code scans the full communication matrix where DSLA uses
  adjacency lists, so those times compare implementations, not algorithms. Use `../complexity.md` for
  the algorithmic comparison and `../scaling/` for DSLA's own growth. Do not claim DSLA is faster than
  HEFT from this table.
