# Worked example for the paper

Files: `example.d3` (input in d3 format), `topcuoglu.d3`. The figure `gantt_example.pdf` is rebuilt with `python3 paper/make_figures.py`.

## Illustrative example (6 tasks, 2 processors)

Found by a random search over small layered DAGs (6-8 tasks, 2-3 processors) for the simplest
instance on which BLA-HEFT beats HEFT, AEFT and OPPTS and differs from HEFT in a single
processor choice. It is an illustration of the mechanism, not evidence of performance (that is
the benchmark).

Computation costs W(t, P1), W(t, P2):

| Task | P1 | P2 |
|---|---|---|
| T1 | 12 | 19 |
| T2 | 19 | 4 |
| T3 | 18 | 4 |
| T4 | 9 | 9 |
| T5 | 11 | 18 |
| T6 | 16 | 4 |

Edges (communication cost): T1->T2 16, T1->T3 8, T1->T4 11, T2->T5 16, T3->T6 11, T4->T6 15, T5->T6 16.

Makespans (all schedules validated): **BLA-HEFT 54**, AEFT 58, OPPTS 58, HEFT 62.

BLA-HEFT step by step (scheduling order T1, T2, T5, T4, T3, T6; lambda = 0.5, R = 3):

| Task | EFT (P1, P2) | Look (P1, P2) | Score = 0.5 EFT + 0.5 Look | BLA-HEFT | HEFT (min EFT) |
|---|---|---|---|---|---|
| T1 | 12, 19 | 66, 64 | 39.0, 41.5 | P1 | P1 |
| **T2** | **31, 32** | **77, 58** | **54.0, 45.0** | **P2** | **P1** |
| T5 | 59, 50 | 86, 72 | 72.5, 61.0 | P2 | P2 |
| T4 | 21, 59 | 63, 72 | 42.0, 65.5 | P1 | P1 |
| T3 | 39, 54 | 72, 62 | 55.5, 58.0 | P1 | P1 |
| T6 | 82, 54 | exit task | 82, 54 | P2 | P2 |

(These values are for BLA-HEFT's own partial schedule, so after T2 they differ from HEFT's state.)

Story for the text:

1. **Myopia of EFT (vs HEFT).** T2 finishes at 31 on P1 and 32 on P2, so HEFT picks P1 by one time
   unit. That occupies P1 for 19 units and puts T2's child T5 on the critical path. BLA-HEFT's
   look-ahead estimates that the path through T5 finishes at 77 if T2 goes to P1 but 58 if it goes
   to P2, so the blended score (54.0 vs 45.0) sends T2 to the fast processor P2.
2. **Stale static look-ahead (vs AEFT).** AEFT also places T2 on P2, but its static table then
   sends T3 to P2, queued behind T5, because the table assumes every processor is idle. BLA-HEFT's
   estimate uses the actual processor availability and keeps T3 on P1 (EFT 39 vs 54).

## The classic 10-task graph (Topcuoglu et al., 2002), for completeness

Non-insertion makespans: OPPTS 74, AEFT 76, HEFT 80, **BLA-HEFT 80**, PEFT 85, IPPTS 96.
BLA-HEFT does not improve on HEFT for this single graph. Report it honestly if the classic graph
is shown; a single instance says nothing about average performance.
