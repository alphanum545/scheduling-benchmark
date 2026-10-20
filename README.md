# Scheduling benchmark

Runs DAG-scheduling algorithms on the workflow datasets in the cloud (GitHub Actions),
checks every schedule, and produces results tables and paper-ready figures.

- **Any algorithm, one source file.** D1 files (6 header lines) and D2 files (5 header lines)
  are detected automatically; every algorithm receives the same clean input.
- **Fast.** Work is split across up to 20 machines, each using all its cores. Only runs not
  done before are executed; an algorithm reruns automatically when its source changes.
- **Trustworthy.** Every schedule is checked for dependency violations, processor overlaps,
  wrong task durations and makespans below the critical-path lower bound. Invalid runs are
  flagged, never averaged in.

---

## One-time setup

### 1. Put this folder on GitHub
Create a repository and push this folder to it (including the hidden `.github` folder).

### 2. Upload the datasets
1. Compress each family folder separately (`LIGO`, `Montage`, `Genome`, `GE`, `FFT`, `LA`).
   In Finder: right-click the folder → **Compress**. You get `LIGO.zip`, and so on.
   Each family folder must contain its `<N>Nodes` folders directly.
2. On GitHub, open the repository → **Releases** → **Draft a new release**.
   - Tag: `data` (exactly this; create it when asked)
   - Title: anything, e.g. "Datasets"
   - Drag the six `.zip` files into the attachment box (each must be under 2 GB)
   - **Publish release**

Archives can also be `.tar.gz` / `.tar.xz`, may contain several families, and may include
macOS junk or the `run.sh` scripts; all of that is handled.

### 3. Index the data
**Actions** tab → **Prepare data** → **Run workflow**. It downloads every archive, checks every
file, and commits `data_index.csv`. Rerun it whenever you change the archives on the release.

---

## Running a benchmark

**Actions** tab → **Benchmark** → **Run workflow**, then fill in:

| Field | Example | Meaning |
|---|---|---|
| algos | `all` or `oppts,aeft,peft,heft` | IDs from `algorithms.toml` |
| highlight | `oppts` | your proposed algorithm: drawn first, gains computed against it |
| families | `all` or `LIGO,GE` | dataset families |
| datasets | `all`, `D1`, `D2` | file format group |
| nodes | `all` or `50,100,1000` | task counts (from the `<N>Nodes` folder names) |
| procs | `all` or `2,4,8` | processor counts |
| report_name | `oppts-vs-baselines` | folder name for this report |
| split | `all`, `dev`, `test` | `dev` ≈ 1 instance in 4, for designing and tuning; `test` = the held-out rest, for the final comparison only |
| shards | `20` | parallel machines (20 is the free-plan maximum) |
| force | off | rerun everything, ignoring stored results |

When it finishes:
- The run page shows a summary (validity table, averages, gains).
- The report is committed to `results/reports/<report_name>/` and also attached to the run
  as a downloadable artifact.

### What a report contains
| File | Contents |
|---|---|
| `summary.xlsx` | Overall averages · validity per algorithm · makespan/SLR by tasks and by processors · speedup and efficiency by processors · pairwise better/equal/worse % and average gain · runtime |
| `summary.md` | the key tables as text |
| `results.csv` | one row per run (makespan, SLR, speedup, efficiency, runtime, status, first violation) |
| `figures/*.png, *.pdf` | makespan, SLR, speedup and efficiency charts (one panel per family) and a gain chart; PDF for the paper |

Averages use only instances where **every** selected algorithm produced a valid schedule, so
all algorithms are compared on exactly the same instances. The **Validity** sheet shows what
was excluded and why.

All results ever produced are kept in `results/store.csv`. Reports can be rebuilt at any time
without rerunning anything (run Benchmark again with the same selection; nothing new executes).

---

## Adding an algorithm

1. Put the `.cpp` file in `algorithms/` (any subfolder).
2. Add a block to `algorithms.toml`:
   ```toml
   [mynewalgo]
   source = "algorithms/makespan/mynewalgo.cpp"
   ```
3. Commit, then run **Benchmark** with `algos` = `mynewalgo` (or `all`). Only the new
   algorithm runs; the others come from the store.

### What the program must do
- Read the instance from **standard input**.
- Default input (`input = "d3"`): `n p`, then the `n × p` computation-cost matrix, then the
  `n × n` communication matrix (`-1` = no edge). No header vectors, whatever the dataset.
- Print the schedule as a table after a header line (any header text containing letters):
  ```
  1,4,2,3,...            <- optional lines before the header are ignored
  TASK START FINISH PROC
  1    0     14     2    <- task (1-based), start, finish, processor (1-based)
  ...
  ```
  Your existing algorithms already print exactly this.

### Other options (`algorithms.toml`)
| Option | Values | Use |
|---|---|---|
| `input` | `d3` (default), `d2`, `raw` | `d2` = header kept as 5 parameter vectors (D1's extra first vector removed), for algorithms that need the energy parameters. `raw` = the file untouched. |
| `output` | `schedule` (default), `makespan` | `makespan` = the program prints only a number; it is checked against the lower bound only |
| `args` | e.g. `["1.0"]` | command-line arguments |
| `duplication` | `true` / `false` | allow entry-task duplication (LSTD) |
| `enabled` | `true` / `false` | include in `all` |
| `timeout` | seconds (default 900) | per run |
| `flags` | default `-O2 -std=c++17` | compiler flags |

`#include <bits/stdc++.h>` works everywhere (a substitute header is supplied on macOS).

---

## Run statuses
| Status | Meaning |
|---|---|
| `ok` | valid schedule |
| `unchecked` | makespan-only output at or above the lower bound |
| `invalid` | schedule breaks a dependency, overlaps on a processor, has a wrong duration, or the makespan is below the lower bound; `first_violation` says which |
| `timeout`, `crash(code)`, `no_output` | the program did not finish properly |
| `bad_instance`, `missing_file`, `check_error` | data or infrastructure problem; the last two are retried automatically next run |

## Metrics
- **SLR** = makespan / (critical path using each task's minimum cost)
- **Speedup** = (minimum over processors of the total cost of all tasks on that processor) / makespan
- **Efficiency** = speedup / processors

---

## Random DAGs (CCR / heterogeneity experiments)
`tools/generate.py` writes random layered DAGs in the same format and folder layout.
`tools/generated.json` lists generated datasets; they are **not uploaded**: every machine rebuilds
identical files from the fixed seed (Prepare data indexes them, Benchmark shards rebuild only the
families they need). The current set, `gen:random-v1`, has 9 families `R-ccr<C>-b<B>`
(CCR 0.1, 1, 10 × heterogeneity β 0.1, 0.5, 1), each with 100/300/500/1000 tasks on 4/8/16/32
processors, 5 graphs per size (720 instances). To add more, add an entry to `generated.json`
(use a new name, e.g. `gen:random-v2`, so the old files stay reproducible) and rerun Prepare data.

## Development / test split
Every instance is permanently assigned to `dev` (about 1 in 4) or `test` by a hash of its path
(`split_of` in `tools/bench.py`). Design and tune algorithms with `split = dev`; run `test` only
for the final comparison, so the reported gains are not tuned to the reported instances.

## Running locally (optional)
Needs Python 3.11+ and g++ (plus `pandas matplotlib openpyxl tabulate` for reports).
```
python3 tools/bench.py index --data data
python3 tools/bench.py plan --algos oppts,aeft --families GE
python3 tools/bench.py run --data data
python3 tools/bench.py merge
python3 tools/report.py --algos oppts,aeft --highlight oppts --name local-test
```

## Layout
```
algorithms.toml            algorithm registry
algorithms/makespan/*.cpp  algorithm sources
tools/bench.py             index, plan, run, merge
tools/evaluate.cpp         format detection, conversion, schedule checking (C++, fast)
tools/report.py            tables and figures
tools/fetch_data.py        downloads dataset archives from the release
tools/generate.py          random DAG generator
data_index.csv             one row per instance (written by Prepare data)
results/store.csv          every run ever made
results/reports/<name>/    reports
.github/workflows/         Prepare data, Benchmark
```

## Notes on the included algorithms
- `peft`, `ppts`, `ippts`, `aeft`, `oppts`: a ready list was added (the task scheduled next is
  the highest-priority task whose predecessors are all scheduled). Without it these produced
  schedules that start tasks before their inputs arrive.
- `oppts` follows the paper (rank = mean OPCM). `oppts_outdeg` is the original code's ranking
  (mean OPCM × out-degree), disabled by default.
- `heft`, `cpop`, `lstd`, `peft`: large matrices are now passed by reference (identical results,
  7–16× faster on 2,000-task instances).
- All algorithms use the non-insertion policy (a task is appended after the last task on a
  processor).
