# Results section: figures and tables

Rebuild everything with `python3 paper/results_figures.py`. The figures (`fig_*.png` / `fig_*.pdf`) are
not committed because they are large; the script regenerates them in a few seconds from the committed
benchmark results. The input is `results/reports/full-contention/results.csv`: all 2,814 instances,
every algorithm valid on every instance, and all algorithms run in the same benchmark run. The makespans
are the same as in `full-all-instances-v2`. In that run DSLA appears as `z_best10`, its lab id; the
schedules are identical (report `dsla-equivalence`).

The paper compares DSLA with 13 baselines. SLAS, DSLA's rank-order schedule alone, belongs to the
ablation and is not in these figures.

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
| `tab_efficiency_by_p` | Efficiency per processor count: DSLA, mean and best of the 13 baselines, and DSLA's speedup |
| `tab_gain_by_characteristics` | DSLA's gain per CCR, heterogeneity β, task count and processor count: against the strongest baseline in that group, and against the per-instance best of all 13 |
| `tab_runtime` | Mean wall time per run on the random graphs; see the caution below |

## Points for writing the results

- **Overall.** DSLA has the lowest average makespan in all six workflow families and in seven of the nine
  random families. The two exceptions are CCR 0.1 with β = 0.5 and β = 1, where OPPTS is lower by 0.5%
  and 0.7%. Over all 2,814 instances it averages 4469.6 against 4624.1 for the next best, LEFT-D. In the pairwise table DSLA is better on 79.5%
  (vs OPPTS) to 98.1% (vs IPPTS) of instances.
- **Where the margin is thin.** Against OPPTS on GE and LA, DSLA is better on only 50% and 55% of
  instances, even though its average is lower. On the smallest workflows (9 to 137 tasks), the
  per-group strongest baseline is often level with DSLA or slightly ahead. The gain grows with graph
  size: 5.5% to 8.9% over the strongest baseline for the Montage, Genome and LIGO sizes of 300 to 1,000
  tasks (FFT, GE and LA at 512 to 529 tasks: 0.6% to 2.0%).
- **Against the per-instance best of 13.** Picking, after the fact, the best of all 13 baselines for
  each instance is an oracle no single method can match. DSLA is still within about 1% of that oracle
  in most groups and beats it at CCR 10 (+2.2%).
- **Efficiency vs p (Reviewer 5, point 3).** Efficiency is speedup / p, with speedup = (best single-
  processor sequential time) / makespan, the standard HEFT definition. It can exceed 1 on the
  heterogeneous workflow processors, because the best single processor is slower than the mix of fast
  processors the scheduler uses. On every family efficiency falls once p exceeds the graph's
  exploitable parallelism. The cause is the workload, not DSLA: the mean and the best baseline fall at
  the same rate, and DSLA stays at or above the best baseline at every p.
  - Workflows: efficiency peaks at p = 16 (2.41) and drops to 0.82 at p = 64.
  - Random graphs: efficiency falls from 0.77 at p = 4 to 0.19 at p = 32, where speedup saturates near 6.
  - A practical answer for the reviewer: past p = 16 on the workflows, and past p = 8 on the random
    graphs, extra processors add little speedup for any of the 14 algorithms.
- **Runtime caution.** `runtime_s` is the wall time of a whole run: process start, reading the n × n
  input matrix and scheduling. The baselines' code scans the full communication matrix where DSLA uses
  adjacency lists, so those times compare implementations, not algorithms. Use `../complexity.md` for
  the algorithmic comparison and `../scaling/` for DSLA's own growth. Do not claim DSLA is faster than
  HEFT from this table.
