# Worked example for the paper

Files: `example.d3`, `example2.d3` (inputs in d3 format), `topcuoglu.d3`. The figures `gantt_example.pdf` and
`gantt_example2.pdf` are rebuilt with `python3 paper/make_figures.py`.

Names: DSLA is the proposed method; SLAS is its first (rank-order) schedule alone, formerly called BLA-HEFT.
Example 1 shows the state-aware look-ahead (it is decided by SLAS); example 2 shows why DSLA also builds the
earliest-finish-order schedule.

## Example 1: the state-aware look-ahead (6 tasks, 2 processors)

Found by a random search over small layered DAGs (6-8 tasks, 2-3 processors) for the simplest
instance on which SLAS beats HEFT, AEFT and OPPTS and differs from HEFT in a single
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

Makespans (all schedules validated): **SLAS 54** (DSLA also 54: its second schedule is not shorter), AEFT 58, OPPTS 58, HEFT 62.

SLAS step by step (scheduling order T1, T2, T5, T4, T3, T6; lambda = 0.5, R = 3):

| Task | EFT (P1, P2) | Look (P1, P2) | Score = 0.5 EFT + 0.5 Look | SLAS | HEFT (min EFT) |
|---|---|---|---|---|---|
| T1 | 12, 19 | 66, 64 | 39.0, 41.5 | P1 | P1 |
| **T2** | **31, 32** | **77, 58** | **54.0, 45.0** | **P2** | **P1** |
| T5 | 59, 50 | 86, 72 | 72.5, 61.0 | P2 | P2 |
| T4 | 21, 59 | 63, 72 | 42.0, 65.5 | P1 | P1 |
| T3 | 39, 54 | 72, 62 | 55.5, 58.0 | P1 | P1 |
| T6 | 82, 54 | exit task | 82, 54 | P2 | P2 |

(These values are for SLAS's own partial schedule, so after T2 they differ from HEFT's state.)

Story for the text:

1. **Myopia of EFT (vs HEFT).** T2 finishes at 31 on P1 and 32 on P2, so HEFT picks P1 by one time
   unit. That occupies P1 for 19 units and puts T2's child T5 on the critical path. SLAS's
   look-ahead estimates that the path through T5 finishes at 77 if T2 goes to P1 but 58 if it goes
   to P2, so the blended score (54.0 vs 45.0) sends T2 to the fast processor P2.
2. **Stale static look-ahead (vs AEFT).** AEFT also places T2 on P2, but its static table then
   sends T3 to P2, queued behind T5, because the table assumes every processor is idle. SLAS's
   estimate uses the actual processor availability and keeps T3 on P1 (EFT 39 vs 54).

## Example 2: why a second, earliest-finish-order schedule (5 tasks, 2 processors)

Found by the same kind of search, for the smallest instance on which DSLA's earliest-finish-order schedule
is shorter than its rank-order schedule (SLAS) and than every baseline. A fork-join graph with expensive
communication (CCR > 1).

Computation costs W(t, P1), W(t, P2):

| Task | P1 | P2 |
|---|---|---|
| T1 | 20 | 20 |
| T2 | 20 | 17 |
| T3 | 10 | 3 |
| T4 | 17 | 17 |
| T5 | 12 | 10 |

Edges (communication cost): T1->T2 15, T1->T3 8, T1->T4 21, T2->T5 24, T3->T5 27, T4->T5 25.

Makespans (all schedules validated): **DSLA 67**, HEFT 70, LEFT-D 72, SLAS 75, OPPTS 75, PMMS 75, LEFT 82, AEFT 87.

| Schedule | Order | Placement | Makespan |
|---|---|---|---|
| SLAS (rank order, lambda 0.5) | T1, T2, T4, T3, T5 | T1 T2 T4 T5 on P2, T3 on P1 (38) | 75 |
| DSLA's second schedule (earliest-finish order, lambda 0.1) | T1, T3, T2, T4, T5 | everything on P2 | 67 |
| HEFT | T1, T2, T4, T3, T5 | T1 T2 T4 T5 on P1, T3 on P2 | 70 |

Story for the text:

1. **The rank order delays the small task until its processor is taken.** T3 has the lowest upward rank
   of the three middle tasks, so rank order schedules it after T2 and T4. By then P2 is busy until 54,
   so T3 goes to P1 (finish 38), and its 27-unit message to T5 makes T5 wait until 65: makespan 75.
   HEFT and the table-based methods make the same split for the same reason.
2. **The earliest-finish order runs it while it is cheap.** Among the ready tasks after T1, T3 can finish
   first (at 23 on P2, 3 units of work), so the second policy runs it immediately on P2. Every task then
   stays on P2 and no message is ever paid: makespan 67, the length of the sequential schedule on P2.
3. **Why both schedules, not one.** On this graph the second policy wins; on example 1 and on most
   low-communication graphs, the rank order wins. DSLA builds both at the same O(v^2 p) cost and keeps
   the shorter, so it never does worse than SLAS. In the benchmark the second schedule is the one kept
   on 15-26% of the workflow instances, 81-85% of the CCR-10 random graphs, and none of the CCR-0.1 ones.

## The classic 10-task graph (Topcuoglu et al., 2002), for completeness

Non-insertion makespans: OPPTS 74, **DSLA 76**, AEFT 76, HEFT 80, SLAS 80, PEFT 85, IPPTS 96.
DSLA's second schedule improves on SLAS and HEFT here (76 against 80), but OPPTS is shorter on this single
graph. Report it honestly if the classic graph is shown; a single instance says nothing about average
performance.
