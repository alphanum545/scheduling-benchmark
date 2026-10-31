# DSLA manuscript: reproduction package

This branch contains exactly what is needed to regenerate every result of the
manuscript *DSLA: Dual-policy state-aware look-ahead list scheduling for
workflows on heterogeneous systems*, and nothing else.

## One command

    pip install -r requirements.txt
    ./reproduce.sh

On GitHub, pushing this branch to a new repository runs
`.github/workflows/reproduce.yml`, which executes the same script on a
GitHub-hosted runner, uploads `outputs/` as an artifact, commits it to the
repository, and fails if any result differs from the manuscript.

## What it does

| Step | Produces | Manuscript |
|---|---|---|
| Verify and unpack `data/archives/*.zip` (SHA-256 pinned); regenerate the nine random families from seed 1 | `data/` | Section 7.1 |
| Index all instances | `data_index.csv` (2814 instances, 712 dev / 2102 test) | Sections 7.1–7.2 |
| Run 17 algorithms on every instance and validate every schedule | `outputs/runs.csv` | Sections 8.1–8.4 |
| Instance statistics | `outputs/describe.json` | Table 3 |
| Statistics: paired gains, cluster bootstrap, Wilcoxon–Holm, Friedman–Nemenyi, strata, replays, ablations | `outputs/results.json`, `outputs/tables/` | Tables 4–8 |
| Decision-level examples | `outputs/examples.json` | Section 6, Table 2 |
| Construction time | `outputs/timing/timing.json` | Section 8.5 |
| Figures | `outputs/figures/` | Figs. 2–4 |
| Verification | `outputs/VERIFY.txt` | — |

## Algorithms (`algorithms.toml`)

- Baselines: HEFT, PEFT, IPPTS, AEFT, PMMS, BPPTS. DSLA and its rank build
  SLAS. Sources are identical to alphanum545/scheduling-benchmark commit
  2f078d8.
- Ablations: `dsla_fix` (without the own-cost term; from commit c5d7c89)
  and eight variants of `dsla_var.cpp`, a parametrized copy of `dsla.cpp`
  whose default arguments reproduce `dsla.cpp` exactly.

All algorithms use append-only placement and the harness's ready-list rule.

## Verification

`paper/verify.py` fails unless:

- the index has 2814 instances;
- all 17 × 2814 schedules are valid;
- every table body equals the manuscript's byte for byte (`paper/expected/tables`);
- every number in `results.json` and `describe.json` equals `paper/expected`;
- the Section 6 examples reproduce Table 2.

Construction times depend on hardware and are reported, not compared.
Exact equality of the statistics requires the pinned package versions in
`requirements.txt` (Python 3.13).

## Layout

```
algorithms/makespan/   the nine C++ sources
algorithms.toml        the 17 algorithm configurations
data/archives/         workflow instance archives + SHA256SUMS
tools/                 data preparation, random generator, harness, schedule validator
paper/                 analysis, tables, figures, examples, timing, verification
paper/expected/        the manuscript's tables and numbers, for verification only
reproduce.sh           the full pipeline
```

The workflow archives are those of the benchmark's `data` release. Six
Montage files in them are empty and are excluded by the harness, as in the
manuscript.
