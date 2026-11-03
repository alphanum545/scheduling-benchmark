# CGS evaluation: reproduction package

This branch regenerates every result of the evaluation of **CGS** (Contention-table, Gated join-closure
Scheduling), using the same benchmark, validator and statistics as the DSLA manuscript package
(`paper/dsla-fgcs-reproduction`). DSLA itself is not part of this study.

## One command

    pip install -r requirements.txt
    ./reproduce.sh

On GitHub, a push to this branch runs `.github/workflows/reproduce.yml`, which executes the same script on a
GitHub-hosted runner, uploads `outputs/` as an artifact, commits it to the branch, and fails if any result
differs from `paper/expected`.

## What it does

| Step | Produces |
|---|---|
| Verify and unpack `data/archives/*.zip` (SHA-256 pinned); regenerate the nine random families from seed 1 | `data/` |
| Index all instances | `data_index.csv` (2814 instances, 712 dev / 2102 test) |
| Run 28 algorithm configurations on every instance and validate every schedule | `outputs/runs.csv` |
| Check that each insertion baseline in `append` mode equals the repository original on every instance | `outputs/append_check.json` |
| Instance statistics | `outputs/describe.json` |
| Statistics, tables, readable summary | `outputs/results.json`, `outputs/tables/`, `outputs/SUMMARY.md` |
| Why CGS gains less on random DAGs: graph structure, closure probe, headroom (test split) | `outputs/why_random/`, `outputs/tables/tab_why_random_rows.tex`, `outputs/tables/why_random_macros.tex` |
| Worked examples (classic Topcuoglu DAG; Lookahead HEFT paper's Fig. 1) | `outputs/example/` |
| Scheduling time vs tasks and processors | `outputs/timing/timing.json` |
| Figures | `outputs/figures/` |
| Verification | `outputs/VERIFY.txt` |

## Metrics (as in the DSLA manuscript)

- **Makespan**, compared in pairs: mean gain `100 (C_B − C_CGS) / C_B` with a 95 % cluster-bootstrap CI
  (cluster = one DAG across its processor counts; 10,000 resamples, seed 20261031), median gain,
  wins/ties/losses, cluster-level two-sided Wilcoxon signed-rank test, Holm-adjusted across baselines;
  test split (headline), all instances and dev; against each baseline and the per-instance virtual best.
- **Friedman** mean ranks with the **Nemenyi** critical difference; mean **SLR**, **speedup** and **efficiency**.
- **Strata** on the test split: family, processors, CCR, heterogeneity β, workflow vs random, task count.
- **Secondary schedule metrics** (`tools/evaluate.cpp`): utilization, paid-communication ratio, load imbalance,
  robustness (30 replays with costs scaled by U[0.8, 1.2]), network contention (one-port replay), and the
  paired gains under both replays.
- **Ablations** of CGS's components, also by communication group.
- **Context**: CGS against the append-only repository originals, the gain insertion alone gives each baseline,
  and methods discussed but not used as baselines.
- **Scheduling time** vs number of tasks and processors.
- **Why the gains are smaller on random DAGs** (`paper/why_random.py`, text in `paper/sections/random_graphs.tex`):
  parents per task and share of tasks with 1–2 parents; from `tools/cgs_probe.cpp` (CGS with counters, same
  schedules), how often a closure's join ready time is already set by parents placed earlier and how much of
  it the closure decides; component gains and how many schedules closures shorten or lengthen; headroom against
  max(critical path, total work / p), meaningful at CCR 0.1 only. Workflows pooled vs random by CCR.

## Algorithms (`algorithms.toml`)

Every compared method uses **insertion-based placement** (earliest idle gap that fits), as CGS does.

- **Proposed:** `cgs`, the configuration frozen in `research/cgs-20261027` (`experiments/cgs/PROTOCOL.md`).
- **Baselines:** HEFT, PEFT, IPPTS, AEFT, PMMS, BPPTS with insertion (`algorithms/insertion/*_ins.cpp`: the
  repository sources with only the start-time rule changed; `append` reproduces the original exactly, checked on
  every run), and Lookahead HEFT (`la_heft.cpp`, depth 1, max variant, insertion).
  `heft_ins.cpp` is the exception: it is a rewrite of canonical HEFT, not a patched copy. It computes ranks in
  floating point, while the repository HEFT uses fixed point (×10⁴, truncated), so near-equal ranks can break
  differently. In `append` mode it equals the repository HEFT on 2811 of 2814 instances; of the other three, two
  have the same makespan and one differs by 0.2 % (R-ccr10-b0.1/1000Nodes/32PROCESSORS/5_32: 35910 vs 35831).
- **Ablations:** `cgs_single` (no joint placement), `cgs_credit0` (plain rank gate), `cgs_nogate`,
  `cgs_lam0` (EFT-only selection), `cgs_tailnone`, `cgs_commitall`, `cgs_parents3`.
- **Context:** the six baselines as in the repository (append-only), and CPOP, PPTS, IPEFT, MPEFT, LSTD, LEFT,
  LEFT-D (append-only, sources identical to commit 2f078d8).

## Verification

`paper/verify.py` fails unless:

- the index has 2814 instances (712 dev / 2102 test);
- all 28 × 2814 schedules are valid;
- every insertion baseline in `append` mode gives the original's schedule on all 2814 instances, except the
  three known HEFT cases below;
- every table body equals `paper/expected/tables` byte for byte;
- every number in `results.json`, `describe.json` and `why_random/why_random.json` equals `paper/expected`;
- `cgs_probe` gives CGS's makespan on every test instance (`paper/why_random.py` exits otherwise);
- the worked examples give the expected makespans, including the published ones
  (classic DAG: HEFT 80, AEFT 76; Lookahead HEFT paper: HEFT 260, Lookahead HEFT 184).

Scheduling times depend on hardware and are reported, not compared. Exact equality of the statistics requires
the pinned package versions in `requirements.txt` (Python 3.13).

## Layout

```
algorithms/insertion/  CGS, the insertion baselines and Lookahead HEFT
algorithms/makespan/   append-only repository sources (context and fidelity check)
algorithms.toml        the 28 algorithm configurations
data/archives/         workflow instance archives + SHA256SUMS
tools/                 data preparation, random generator, harness, validator, fidelity check
paper/                 analysis, tables, figures, examples, timing, verification
paper/sections/        manuscript text that takes every number from generated macros
paper/expected/        the expected tables and numbers, for verification only
reproduce.sh           the full pipeline
```

Six Montage files in the archives are empty and are excluded by the harness.
