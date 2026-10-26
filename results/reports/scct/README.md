# SCCT: Sibling-Contention Cost Table scheduling

A new list-scheduling heuristic built from an analysis of OPPTS's cost repetition. It is independent of
DSLA: no state-conditioned look-ahead, no dual policy, no critical-child subset and no λ blend. DSLA is
not part of any comparison in this report.

Source: `algorithms/makespan/scct.cpp` (id `scct`); ablations are the `scct_*` and `oppts_f*` entries in
`algorithms.toml`.

## 1. Starting point: OPPTS's repetitions are where its strength comes from

OPPTS's table `PCG(t,k) = w(t,k) + max_c min_q (PCG(c,q) + w(c,q) + [q≠k] c(t,c))` counts every
descendant's computation twice (`PCG(c,q)` already contains `w(c,q)`), and its processor rule
`EFT(t,k) + PCG(t,k)` counts the task's own cost twice. Removing them (`oppts_fix.cpp`):

| variant | change | vs OPPTS | shorter | longer |
|:--|:--|--:|--:|--:|
| oppts_ft | table counts each cost once | −0.95% | 42.8% | 54.9% |
| oppts_fs | selection counts own cost once | −8.28% | 18.9% | 78.9% |
| oppts_fb | both | −7.09% | 21.3% | 77.8% |

So the repetitions cannot simply be deleted. SCCT keeps their effect but gives each quantity an explicit
meaning, and replaces the optimistic selection table with one that models contention.

## 2. Algorithm

1. **Contention table T (processor selection)**, each cost counted once. Optimistic tables (PEFT, OPPTS)
   let every child of a task take its own best processor at the same moment; when several children
   prefer the same processor that is impossible. SCCT places the children greedily, most critical first,
   on processors that start free at the parent's finish; a child that picks an occupied processor waits
   for the earlier sibling. `T(t,k) = w(t,k) + max_c F_c`.
2. **Priority table G (task order)**:
   `G(t,k) = w(t,k) + max_c min_q (G(c,q) + μ·w(c,q) + [q≠k] c(t,c))`, `rank(t) = mean_k G(t,k)`.
   Each descendant's computation weighs (1 + μ), communication once. **μ = 1 reproduces OPPTS's priority
   exactly** (verified on all 2814 instances), so OPPTS's double counting is one setting of an explicit
   parameter. SCCT uses **μ = 2**.
3. **Placement** (non-insertion): highest-rank ready task, processor `argmin_k EFT(t,k) + T(t,k)`. As in
   OPPTS the task's own cost enters both terms; removing that costs 5% (`scct_noself`).

Complexity O(e·p² + e log e), the same order as OPPTS.

## 3. Choosing μ (dev split only, 712 instances)

| μ | 0 | 0.5 | 1 (OPPTS) | 1.5 | **2** | 3 | 5 | 10 |
|:--|--:|--:|--:|--:|--:|--:|--:|--:|
| gain vs OPPTS | −0.65% | +0.24% | +0.70% | +0.94% | **+0.97%** | +1.08% | +0.92% | +0.51% |

μ = 2 is the middle of the 1.5–5 plateau, chosen over the single best dev value (μ = 3) to limit overfitting.
Alternatives tried for the priority and rejected on dev: discounting communication instead
(γ = 0.25 / 0.5 / 0.75 with μ = 0: −2.07% / −0.85% / −0.50%), ranking by the contention table (−0.29%),
adding a computation-only path length (−0.24%), and capping contention at p children (−0.25%).

## 4. Results on the held-out test split (2102 instances)

gain = (baseline − SCCT) / baseline; positive = SCCT shorter. SCCT is strictly the best of all 13
algorithms on 15.9% of test instances.

| baseline | mean gain | median gain | SCCT shorter | SCCT longer |
|:--|--:|--:|--:|--:|
| OPPTS | +0.85% | +0.97% | 62.7% | 34.8% |
| HEFT | +1.32% | +1.26% | 59.8% | 39.6% |
| AEFT | +1.84% | +1.54% | 71.0% | 24.4% |
| PMMS | +2.08% | +1.75% | 71.0% | 28.0% |
| LSTD | +2.16% | +2.47% | 64.9% | 34.4% |
| LEFT | +3.31% | +3.19% | 66.9% | 32.8% |
| PPTS | +3.91% | +3.29% | 78.0% | 21.1% |
| IPEFT | +4.20% | +3.59% | 69.5% | 29.7% |
| MPEFT | +5.46% | +3.66% | 73.2% | 25.8% |
| PEFT | +5.86% | +4.65% | 77.9% | 21.3% |
| BPPTS | +17.45% | +14.76% | 93.8% | 5.9% |
| IPPTS | +24.29% | +20.65% | 96.1% | 3.8% |

On all 2814 instances (Wilcoxon signed-rank, Holm-corrected; per-DAG test averages over processor counts):
vs OPPTS +0.88%, median +0.98% [0.85, 1.09], p = 5.8e−62, per-DAG p = 7.2e−24;
vs HEFT +1.41%, median +1.29% [1.08, 1.68], p = 8.3e−30, per-DAG p = 1.3e−6. Full tables: `summary.md`.

## 5. Where it wins and where it does not (all instances)

- **Large workflows:** best of all 13 on Genome (+0.69% vs OPPTS) and Montage (+1.21%); on LIGO +1.47%
  vs OPPTS but −1.53% vs HEFT.
- **Small workflows (FFT, GE, LA; 60 instances each):** beats HEFT by 3–8% but trails OPPTS by 0.4–1.0%.
- **Random DAGs, CCR 0.1 and 1:** within 0.7% of the best baseline in every family; first at CCR 1 with β = 0.5 and 1.
- **Random DAGs, CCR 10:** the clear weakness. Beats OPPTS by 0.9–1.9% but LEFT is 15–18% better and
  HEFT 2.4–4.0% better; SCCT beats only 6–7 of 12 baselines there.

## 6. Component ablations (all instances)

| variant | change | SCCT gain over it | variant vs OPPTS |
|:--|:--|--:|--:|
| scct_noself | no extra weight on own cost in selection | +5.00% | −4.98% |
| scct_rankcont | rank by the contention table (first SCCT version) | +1.31% | −0.51% |
| scct_mu1 | OPPTS priority (μ = 1) | +0.31% | +0.55% |
| scct_nocont | no sibling contention in the selection table | +0.18% | +0.67% |

The priority table and the own-cost weight carry most of the gain; sibling contention adds a smaller,
consistent improvement (+0.18% overall; larger on random DAGs at CCR ≥ 1 in the first version).

## 7. Caveats

- Run locally on 26 Oct 2026 (2 workers), not via the Benchmark workflow; rerun Benchmark with
  `scct,scct_nocont,scct_noself,scct_rankcont,scct_mu1` (and the dev sweeps) before citing.
- Prior-art check was brief: no table modelling sibling contention was found; the closest lines are the
  critical-node / pessimistic cost tables and an FGCS "improved optimistic cost matrix" paper whose
  details were not accessible. A proper literature review is still needed.
- μ is a tuned parameter; report it with the sweep above.

## Files
`tables.md` (all tables above, regenerated by `python3 results/reports/scct/scct_tables.py`), and the
standard report outputs from `tools/report.py --algos scct,heft,peft,ipeft,mpeft,aeft,ppts,ippts,bppts,oppts,pmms,left,lstd --highlight scct --name scct`:
`summary.md`, `summary.xlsx`, `results.csv`, `figures/`.
