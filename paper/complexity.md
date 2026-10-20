# Time complexity

Notation: v tasks, e edges (e ≤ v(v−1)/2), p processors, R = 3 critical children per task, and ω the
mean size of the ready list over the scheduling steps (1 ≤ ω ≤ v). Every algorithm below is
non-insertion, and its schedule is built in v steps.

Two implementation facts are used throughout. Both are standard and are the way the benchmark's DSLA
code computes them.

- **Look-ahead tables in O(e·p), not O(e·p²).** OCT, PCM, IOCT, OPCM and PM all have the form
  T(t,k) = w(t,k) + max over children c of min over q of [T(c,q) + (q ≠ k)·c(t,c)], or a close variant.
  For one edge, the inner minimum equals min(T(c,k), min_q T(c,q) + c(t,c)). The min over q is computed
  once per edge, in O(p), so each (edge, processor) pair costs O(1) and the whole table costs O(e·p).
  Written naively, the same table costs O(e·p²).
- **Data-ready times kept incrementally.** DRT_c(q) is the earliest time task c's input data can be
  on processor q. When a parent finishes, DRT is updated for each of its children on every processor:
  O(p) per edge, O(e·p) in total. A task's EST on any processor is then O(1), and its EFT on all
  processors is O(p).

## DSLA, phase by phase

| Phase | Work | Cost |
|---|---|---|
| 1. Topological order | Kahn's algorithm | O(v + e) |
| 2. IOCT table | backward pass, O(p) per edge with the min trick, plus O(p) per task | O(e·p + v·p) |
| 3. Upward rank and priority order | mean cost O(p) per task, one pass over the edges, then a sort | O(v·p + e + v log v) |
| 4. Critical children K(t) | mean IOCT O(p) per task; sort each task's children, keep R | O(v·p + e log e) |
| 5. BUILD: task choice, π_rank | scan the ready list for the highest rank | O(ω) per step, O(v·ω) total |
| 5. BUILD: task choice, π_eft | min_k EFT for every ready task: O(p) each | O(ω·p) per step, O(v·ω·p) total |
| 5. BUILD: EFT on every processor | O(1) per processor thanks to DRT | O(v·p) |
| 5. BUILD: blended score | for each processor k, each of the ≤ R children, each processor q: O(1) | O(R·p²) per task, O(v·R·p²) total |
| 5. BUILD: commit and update DRT | O(p) per outgoing edge | O(e·p) total |
| 6. Two BUILDs and the comparison | phase 5 twice, then compare two makespans | 2 × phase 5 + O(1) |

The total is T(DSLA) = O(e·p + e log e + v·R·p² + v·ω·p). With e ≤ v², ω ≤ v and R a constant:

- **Worst case**, O(v²·p + v·p²) = O(v·p·(v + p)). That is **O(v²·p) when p ≤ v**, which holds for
  every instance in the benchmark. It is reached by dense DAGs (e = Θ(v²), where the DRT updates
  dominate) and by wide DAGs (ω = Θ(v), for example many independent tasks, where π_eft's scan of the
  ready list dominates).
- **Best case**, Θ(v·p²). A chain has e = v − 1, ω = 1 and one child per task. The look-ahead then
  costs p² per task and everything else costs O(v·p). Plain HEFT costs Θ(v·p) here, so DSLA's best case
  is higher by a factor of p. This is the price of a state-aware look-ahead over all processors.
- **Average case**, Θ(e·p + v·p² + v·ω·p), with e and ω taken for the graph class. For the
  benchmark's workflows and random graphs, e is 1.1 to 13 times v and ω is far below v. In that regime
  the v·p² look-ahead term and the e·p term dominate, and the cost grows close to linearly in v. The
  measured exponents agree (see `scaling/`): SLAS's scheduling time grows with exponent 1.23 in v and
  1.12 in p. DSLA runs the same work twice plus the π_eft scan.

SLAS is phase 5 once with π_rank: O(e·p + e log e + v·R·p² + v·ω). Its worst case is O(v²·p) when
p ≤ v, and its best case Θ(v·p²).

## All algorithms

These costs are derived from each algorithm's definition, using the two implementation facts above,
and are not quoted from the original papers. Some of those papers state O(v²·p) for a table phase that
costs O(e·p²) when written naively; with the min trick, every table below costs O(e·p). Best case is a
chain (e = v − 1, ω = 1); worst case is a dense or wide DAG (e = Θ(v²) or ω = Θ(v)).

| Algorithm | Static phase | Construction phase | Best | Average | Worst |
|---|---|---|---|---|---|
| HEFT (2002) | upward rank O(v·p + e), sort | EFT on all processors, O(p) per task plus O(e·p) DRT updates | Θ(v·p) | Θ(e·p) | O(v²·p) |
| PEFT (2014) | OCT O(e·p) | EFT + OCT, O(p) per task plus O(e·p) | Θ(v·p) | Θ(e·p) | O(v²·p) |
| IPEFT (2017) | two tables, O(e·p) each | as PEFT | Θ(v·p) | Θ(e·p) | O(v²·p) |
| PPTS (2019) | PCM O(e·p) | EFT + PCM, as PEFT | Θ(v·p) | Θ(e·p) | O(v²·p) |
| LSTD (2020) | upward rank | EFT with entry-task duplication, O(p) extra | Θ(v·p) | Θ(e·p) | O(v²·p) |
| IPPTS (2021) | PCM plus heterogeneity rank, O(e·p) | EFT + PCM-based look-ahead | Θ(v·p) | Θ(e·p) | O(v²·p) |
| MPEFT (2021) | OCT plus a descendant table, O(e·p) | as PEFT | Θ(v·p) | Θ(e·p) | O(v²·p) |
| PMMS (2023) | PM O(e·p), scaling-down O(e·p) | EFT + PM, as PEFT | Θ(v·p) | Θ(e·p) | O(v²·p) |
| AEFT (2025) | IOCT O(e·p) | EFT + IOCT, as PEFT | Θ(v·p) | Θ(e·p) | O(v²·p) |
| BPPTS (2025) | PCM variant O(e·p) | as PPTS | Θ(v·p) | Θ(e·p) | O(v²·p) |
| LEFT (2025) | none | global min EFT over ready tasks × processors, O(ω·p) per step | Θ(v·p) | Θ(e·p + v·ω·p) | O(v²·p) |
| LEFT-D (2025) | none | as LEFT, plus O(p) for the entry copies | Θ(v·p) | Θ(e·p + v·ω·p) | O(v²·p) |
| OPPTS (previous submission) | OPCM O(e·p) | EFT + OPCM | Θ(v·p) | Θ(e·p) | O(v²·p) |
| **SLAS** | IOCT O(e·p), rank, K(t) | blended state-aware look-ahead, O(R·p²) per task | Θ(v·p²) | Θ(e·p + v·p²) | O(v²·p) (p ≤ v) |
| **DSLA** | as SLAS | two BUILDs; π_eft adds O(ω·p) per step | Θ(v·p²) | Θ(e·p + v·p² + v·ω·p) | O(v²·p) (p ≤ v) |

## Answering reviewers on complexity

DSLA has the same worst-case class as every baseline, O(v²·p). Why it schedules better at the same
cost:

- **What differs is how the O(v·p²) look-ahead budget is spent, not the class.** Table-based methods
  (PEFT, PPTS, AEFT, OPPTS, PMMS) spend O(e·p) once, before scheduling, on estimates that assume every
  processor is idle. DSLA spends O(R·p²) per task during construction and re-evaluates its children
  against the real processor availability and data-arrival times.
- **The second BUILD is a constant factor of two, not a change of class.** It is the only cost DSLA
  adds over SLAS. Its main term, O(v·ω·p), is bounded by the same O(v²·p).
- **The honest trade-off is the best case.** On chain-like graphs DSLA costs Θ(v·p²) against Θ(v·p)
  for HEFT. The paper should state this.
