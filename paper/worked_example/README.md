# Worked example for the paper

Files: `example.d3`, `example2.d3`, `topcuoglu.d3`, `topcuoglu_variant.d3` (inputs in d3 format). The figures
(`gantt_example*.pdf`, `gantt_topcuoglu_variant.pdf`, `dag_topcuoglu_variant.pdf`) are rebuilt with
`python3 paper/make_figures.py`.

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

## Recommended main example: the classic graph's structure with new costs (`topcuoglu_variant.d3`)

The same 10 tasks, 3 processors and 15 edges as Topcuoglu et al. (2002): T1 -> T2..T6; T2 -> T8, T9;
T3 -> T7; T4 -> T8, T9; T5 -> T9; T6 -> T8; T7, T8, T9 -> T10. Only the costs are new, drawn from the
classic graph's own ranges (computation 5-21, communication 9-27). It was found by a random search over
cost assignments for one where both of DSLA's components help: SLAS alone already beats every baseline,
and the second schedule improves on SLAS. Figures: `dag_topcuoglu_variant.pdf` (graph and cost table) and
`gantt_topcuoglu_variant.pdf`. Like the other examples, it illustrates the mechanism; the benchmark is the
evidence of performance.

Computation costs W(t, P1), W(t, P2), W(t, P3):

| Task | P1 | P2 | P3 | Upward rank |
|---|---|---|---|---|
| T1 | 15 | 20 | 12 | 113.0 |
| T2 | 11 | 14 | 8 | 69.3 |
| T3 | 20 | 12 | 8 | 73.0 |
| T4 | 18 | 15 | 18 | 75.3 |
| T5 | 5 | 21 | 9 | 76.0 |
| T6 | 14 | 6 | 7 | 72.0 |
| T7 | 9 | 18 | 15 | 40.7 |
| T8 | 9 | 10 | 6 | 41.0 |
| T9 | 10 | 7 | 6 | 45.3 |
| T10 | 5 | 6 | 21 | 10.7 |

Edges (communication cost): T1->T2 16, T1->T3 9, T1->T4 22, T1->T5 21, T1->T6 21, T2->T8 13, T2->T9 13,
T3->T7 19, T4->T8 14, T4->T9 13, T5->T9 19, T6->T8 22, T7->T10 16, T8->T10 22, T9->T10 27.

Makespans (non-insertion, every schedule validated):

| Algorithm | Makespan | | Algorithm | Makespan |
|---|---|---|---|---|
| **DSLA** | **72** | | HEFT | 90 |
| SLAS (DSLA's rank-order schedule) | 77 | | LEFT-D | 89 |
| LSTD | 82 | | IPPTS | 92 |
| LEFT | 83 | | MPEFT | 92 |
| BPPTS | 85 | | AEFT | 94 |
| IPEFT | 86 | | PMMS | 94 |
| | | | PPTS | 96 |
| | | | OPPTS | 102 |
| | | | PEFT | 104 |

DSLA's two schedules:

| Schedule | Order | Makespan |
|---|---|---|
| S1 = SLAS (rank order, lambda 0.5) | T1, T5, T4, T3, T6, T2, T9, T8, T7, T10 (the same order as HEFT) | 77 |
| S2 (earliest-finish order, lambda 0.1) | T1, T6, T3, T2, T5, T4, T7, T8, T9, T10 | **72**, kept |

SLAS decisions (EFT on P1, P2, P3 | blended score; the chosen processor has the lowest score):

| Task | EFT | Score | SLAS | HEFT |
|---|---|---|---|---|
| **T1** | **15, 20, 12** | **39.5, 48.0, 40.5** | **P1** | **P3** |
| T5 | 20, 57, 45 | 30.0, 74.0, 63.0 | P1 | P3 |
| T4 | 38, 52, 55 | 54.5, 67.5, 77.5 | P1 | P3 |
| T3 | 58, 36, 32 | 75.0, 54.0, 52.5 | P3 | P2 |
| T6 | 52, 42, 43 | 66.0, 58.0, 64.5 | P2 | P2 |
| T2 | 49, 56, 40 | 69.0, 71.0, 58.5 | P3 | P1 |
| T9 | 63, 60, 57 | 70.5, 66.5, 70.5 | P2 | P3 |
| T8 | 73, 70, 70 | 87.0, 78.0, 86.5 | P2 | P2 |
| T7 | 60, 88, 55 | 75.5, 100.0, 73.5 | P3 | P1 |
| T10 | 97, 77, 113 | exit task (EFT) | P2 | P1 |

(EFT and score values are for SLAS's own partial schedule.)

Story for the text:

1. **State-aware look-ahead (SLAS 77 vs HEFT 90).** Both use the same task order. On the first decision,
   HEFT puts T1 on its fastest processor, P3 (EFT 12). SLAS accepts a later finish on P1 (15), because
   its look-ahead sees that T1's most critical children can then run on P1 with no messages: T5 costs
   only 5 on P1, and T5 is the next task in rank order. HEFT's schedule ends with T10 on P1 waiting for
   T9's 27-unit message (start 85, makespan 90). SLAS places T9, T8 and T10 together on P2 and finishes
   at 77, ahead of every baseline (the best is LSTD with 82).
2. **The second schedule (DSLA 72 vs SLAS 77).** The rank order takes the high-rank T5 and T4 first, so
   the three short tasks T6, T3 and T2 are scheduled only afterwards and spread over P2 and P3. The
   earliest-finish order instead runs them straight after T1 on P3 (finishing at 19, 27 and 35), with no
   messages. The join tasks T8 and T9 can then start earlier (T8 at 49 instead of 60), and T10 finishes
   at 72.
3. **DSLA keeps the shorter schedule, 72.** No baseline reaches either of DSLA's two schedules on this
   graph.
