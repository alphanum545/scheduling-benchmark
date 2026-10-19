# Research Repository — Structural & Algorithmic Analysis

> **Purpose of this document.** This is a handover document intended to be given to an AI
> (or a new human) that must *analyse, compare, fix, or extend* the scheduling algorithms
> in `Algorithms/` and run them against the workloads in `D1/` and `D2/`.
>
> Every claim in this document marked **[verified]** was confirmed by compiling the actual
> source and running it against the actual data (see §11 for the harness and §12 for the
> bugs it surfaced). Claims marked **[static]** come from reading the source only.

---

## 0. Quick start (for an AI agent picking this up)

**Do these five things first:**

1. `bash tools/build_all.sh` → 19 binaries in `build/`, one expected failure.
2. `python3 tools/verify_formats.py` → confirms the §8 compatibility matrix.
3. **Read the routing table (§5.4 / Appendix A.2) before running anything.** The two
   dataset trees are *not* interchangeable with the two algorithm families.
4. `python3 tools/check_schedules.py --lower-bound <instance>` → the reference bound
   for any instance.
5. If you are touching the `Makespan/` algorithms, remember they need a **stripped**
   file: `python3 tools/make_stripped.py <instance> <out>`.

**The three questions this repo is actually asking** (see §14 for how to attack them):

| # | Question | Where to look |
|---|---|---|
| Q1 | How should a global energy budget be split across tasks so the makespan is minimised? | §10 — the 8 `Energy/` algorithms are 8 different answers to this one question |
| Q2 | Which HEFT-family priority + EFT rule gives the best makespan on a heterogeneous DAG? | §7 — the 11 `Makespan/` algorithms |
| Q3 | Do the two dataset families (D1: LIGO/Montage/Genome, D2: GE/FFT/LA) behave the same? | §4 — the **format difference** (§5) must be normalised before any comparison |

**Ground rules when analysing:**

* Never compare two algorithms' numbers unless both **parsed the same file correctly**
  (§8). Misparses are silent and produce plausible-looking garbage.
* Always report makespan **and** energy (§12.8 — the energy output is currently
  commented out in every Energy file).
* Always compare against the `cpmin` lower bound (§7.2) and check dependency
  feasibility (§13).

---

## Table of contents

0. [Quick start (for an AI agent picking this up)](#0-quick-start-for-an-ai-agent-picking-this-up)
1. [TL;DR — the 10 things you must know](#1-tldr--the-10-things-you-must-know)
2. [Top-level layout](#2-top-level-layout)3. [The research problem](#3-the-research-problem)
4. [Dataset catalogue](#4-dataset-catalogue)
5. [The three input file formats](#5-the-three-input-file-formats)
6. [Algorithm catalogue — `Algorithms/Energy/`](#6-algorithm-catalogue--algorithmsenergy)
7. [Algorithm catalogue — `Algorithms/Makespan/`](#7-algorithm-catalogue--algorithmsmakespan)
8. [Algorithm ↔ format compatibility matrix](#8-algorithm--format-compatibility-matrix)
9. [Shared building blocks](#9-shared-building-blocks)
10. [The real differentiator: per-task energy allocation](#10-the-real-differentiator-per-task-energy-allocation)
11. [Build & run instructions](#11-build--run-instructions)
12. [Known bugs and hazards (empirically confirmed)](#12-known-bugs-and-hazards-empirically-confirmed)
13. [Validation harness](#13-validation-harness)
14. [Suggested research directions](#14-suggested-research-directions)
15. [Appendix A — quick reference](#appendix-a--quick-reference)
16. [Appendix B — glossary](#appendix-b--glossary)

---

## 1. TL;DR — the 10 things you must know

1. **The repo is three folders:** `Algorithms/` (20 C++ source files, no data),
   `D1/` (1.9 GB, 3 workload families) and `D2/` (380 MB, 3 workload families).

2. **There are THREE different input file layouts, not two.** The two dataset trees use
   two of them; the `Makespan/` algorithms expect a **third** ("stripped") layout that
   **does not exist on disk** and must be generated. **[verified]**

3. **Format D1** = `n p` + *one discarded vector of p* + 5 parameter vectors + `compc` + `commc`.
   **Format D2** = `n p` + 5 parameter vectors + `compc` + `commc`.
   **Format D3 (stripped)** = `n p` + `compc` + `commc`. **[verified by exact token arithmetic]**

4. **The `Energy/` algorithms split into two incompatible groups.**
   `ERWS.cpp` and `algo3.cpp` parse **D1**; `EHEFT`, `ESECC`, `ISAECC`, `MSLECC`, `REP`,
   `RSMECC` parse **D2**. Running any of them on the other format produces garbage or a
   segfault. **[verified]**

5. **The `Makespan/` algorithms (except `cpmin`) all expect Format D3 (stripped).**
   Run on a raw D1/D2 file they **segfault** (`heft`, `peft`, `cpop`, `ipeft`, `ippts`,
   `lstd`, `mpeft`, `ppts`) or **silently emit a meaningless schedule** (`aeft`, `oeft`).
   **[verified]**

6. **`cpmin.cpp` is the odd one out** in `Makespan/`: it *does* skip the 5 parameter
   vectors, so it runs on **raw D2** files. It outputs a single integer (a lower bound),
   not a schedule. **[verified]**

7. **All algorithms read from `stdin`, not from a command-line file argument.**
   `./bin < instance.txt`. **[verified]**

8. **`ESECC.cpp` has a real budget-formula bug** (`PARAM * E_min_g` instead of
   `(1 + PARAM) * E_min_g`). At `PARAM < 1` it emits makespans *below the critical-path
   lower bound* with 5–8 of 9 tasks silently unassigned. **[verified]**

9. **`Makespan/new.cpp` does not compile** (`use of undeclared identifier 'f'`, line 21).
   It is dead scratch code. **[verified]**

10. **`D1/*/run.sh` is a destructive `clean_except.sh`**, byte-identical in all three D1
    families. It `rm -rf`s every top-level subfolder except the six `*PROCESSORS` ones.
    **Do not run it from the repo root.** **[verified by md5 + content read]**

---

## 2. Top-level layout

```
Research/                                  # workspace root (not a git repo)
│
├── Algorithms/                            # 196 KB — SOURCE ONLY, no data, no build system
│   ├── Energy/                            #   8 .cpp  — energy-constrained DVFS schedulers
│   │   ├── EHEFT.cpp      (16.7 KB)      #     newest; weighted-slack energy sharing
│   │   ├── ERWS.cpp       ( 9.1 KB)
│   │   ├── ESECC.cpp      ( 9.1 KB)      #     ⚠ budget-formula bug
│   │   ├── ISAECC.cpp     ( 9.3 KB)
│   │   ├── MSLECC.cpp     ( 8.8 KB)
│   │   ├── REP.cpp        ( 9.9 KB)
│   │   ├── RSMECC.cpp     ( 9.2 KB)
│   │   └── algo3.cpp      (10.1 KB)      #     the "reference" Energy implementation
│   │
│   └── Makespan/                          #  12 .cpp — makespan-oriented DAG schedulers
│       ├── aeft.cpp  ( 5.1 KB)            #     AEFT  (initial-OCT priority)
│       ├── cpmin.cpp ( 3.0 KB)            #     critical-path lower bound (D2 native)
│       ├── cpop.cpp  ( 8.0 KB)
│       ├── heft.cpp  ( 5.3 KB)            #     classic HEFT
│       ├── ipeft.cpp ( 9.3 KB)
│       ├── ippts.cpp ( 4.5 KB)            #     ⚠ emits dependency-violating schedules
│       ├── lstd.cpp  ( 5.9 KB)
│       ├── mpeft.cpp ( 6.7 KB)
│       ├── new.cpp   ( 0.8 KB)            #     ⚠ DOES NOT COMPILE — dead code
│       ├── oeft.cpp  ( 5.5 KB)            #     ⚠ emits dependency-violating schedules
│       ├── peft.cpp  ( 5.2 KB)            #     PEFT  (optimal-computation-time priority)
│       └── ppts.cpp  ( 4.9 KB)
│
├── D1/                                    # 1.9 GB — "Format D1" workloads
│   ├── LIGO/                              #   660 instance files
│   │   ├── run.sh                         #   ⚠ destructive cleanup script
│   │   ├── 50Nodes/ … 1000Nodes/         #   11 scales
│   │   │   └── 2PROCESSORS/ … 64PROCESSORS/   # 6 processor counts
│   │   │       └── 1_2.txt … 10_2.txt     #   10 instances per (scale, proc) cell
│   ├── Montage/                           #   660 instance files
│   │   └── … (same layout)
│   └── Genome/                            #   600 instance files
│       └── … (same layout, but NO 900Nodes)
│
├── D2/                                    # 380 MB — "Format D2" workloads
│   ├── GE/    9 · 35 · 135 · 527 · 2079 nodes      #   60 instance files
│   ├── FFT/  40 · 96 · 224 · 512 · 1152 nodes      #   60 instance files
│   └── LA/   11 · 37 · 137 · 529 · 2081 nodes      #   60 instance files
│       (each: 5 scales × 6 processor counts × 2 instances)
│
├── tools/                                 # ← added by this analysis
│   ├── build_all.sh                       #   compiles all 20 (19 succeed, 1 expected fail)
│   ├── verify_formats.py                  #   proves which algorithm parses which format
│   ├── check_schedules.py                 #   dependency-feasibility + lower-bound checker
│   ├── make_stripped.py                   #   generates Format D3 from D1/D2
│   └── README.md
│
├── build/                                 # ← created by tools/build_all.sh (binaries + shim)
│
└── REPO_ANALYSIS.md                       # ← this file
```

**Counts** **[verified]**

| Tree | Families | Instance files | Disk |
|---|---|---|---|
| `Algorithms/` | 2 (Energy, Makespan) | 20 `.cpp` | 196 KB |
| `D1/` | 3 (LIGO, Montage, Genome) | 1 920 `.txt` + 3 `run.sh` | 1.9 GB |
| `D2/` | 3 (GE, FFT, LA) | 180 `.txt` | 380 MB |

---

## 3. The research problem

All 20 algorithms solve the same family of problems:

> **Heterogeneous, DVFS-enabled DAG scheduling.**
> A workflow of `n` tasks is a directed acyclic graph. Each task has a *computation cost*
> that depends on which (heterogeneous) processor runs it. Processors are shared
> (non-preemptive, one task at a time) and support **dynamic voltage and frequency
> scaling (DVFS)** — a processor can run a task at any frequency between `f_ee` (the
> energy-efficient frequency) and `f_max`, trading speed for power.

* **`Energy/`** optimises under an **energy budget** (`E_given`) and reports the
  resulting **makespan**. Energy efficiency is the research contribution: the algorithms
  differ almost entirely in *how they slice a global energy budget across tasks*.
* **`Makespan/`** optimises the **makespan** only (classic HEFT-family list scheduling,
  with communication cost and heterogeneous processors) and reports a full schedule.

**Why two dataset trees?** `D1` and `D2` are the *same generator* with a different
**header layout** (D1 carries one extra, unused per-processor vector). They are also
**different workload families** — `D1` = astronomy/biology pipelines at 10× the scale,
`D2` = classic HPC traces (GNNP/GE, FFT, LASEP/LA) at 5 scales. The intended experimental
design is to run the *D2-native* algorithms on `D2` and the *D1-native* algorithms on `D1`.

---

## 4. Dataset catalogue

### 4.1 `D1/` — Format D1 workloads

| Family | Node-scale labels | Actual `n` per scale | Processor counts | Instances/cell | Files |
|---|---|---|---|---|---|
| `LIGO/` | 50,100,200,300,400,500,600,700,800,900,1000 | = label (exact) | 2,4,8,16,32,64 | 10 | 660 |
| `Montage/` | 50,100,200,300,400,500,600,700,800,900,1000 | = label (exact) | 2,4,8,16,32,64 | 10 | 660 |
| `Genome/` | 50,100,200,300,400,500,600,700,800,1000 | 47,100,199,297,399,497,597,699,799,997 | 2,4,8,16,32,64 | 10 | 600 |

> `Genome/` has **no `900Nodes`** directory, and its `n` is slightly below the label
> (the generator drops a few nodes). Always read `n` from the file header, never from the
> directory name. **[verified]**

### 4.2 `D2/` — Format D2 workloads

| Family | Node-scale labels | Actual `n` | Processor counts | Instances/cell | Files |
|---|---|---|---|---|---|
| `GE/` | 9, 35, 135, 527, 2079 | = label (exact) | 2,4,8,16,32,64 | 2 | 60 |
| `FFT/` | 40, 96, 224, 512, 1152 | = label (exact) | 2,4,8,16,32,64 | 2 | 60 |
| `LA/` | 11, 37, 137, 529, 2081 | = label (exact) | 2,4,8,16,32,64 | 2 | 60 |

### 4.3 Naming convention

```
D1/<Family>/<N>Nodes/<P>PROCESSORS/<instance>_<P>.txt
D2/<Family>/<N>Nodes/<P>PROCESSORS/<instance>_<P>.txt
```

* `N` = nominal node count, `P` = processor count, `<instance>` = 1-based seed/index.
* D1 instances are `1..10`; D2 instances are `1..2`.
* **The `P` in the filename always matches the `p` in the file header.** **[verified]**

### 4.4 Size profile (drives runtime)

| Instance | Size |
|---|---|
| `D2/LA/2081Nodes/{32,64}PROCESSORS/*` | 12–13 MB each (2081² ints) |
| `D2/GE/2079Nodes/{32,64}PROCESSORS/*` | 12–13 MB each |
| `D2/FFT/1152Nodes/64PROCESSORS/*` | ~4 MB |
| `D1/LIGO/1000Nodes/64PROCESSORS/*` | ~3.1 MB |

---

## 5. The three input file formats

All formats are **whitespace-separated plain text** (newlines are cosmetic; token order
is what matters). Read them with `cin >>` on a flat token stream.

### 5.1 Format D1 — used by `D1/**` files and by `ERWS.cpp` / `algo3.cpp`

```
┌──────────────────────────────────────────────────────────────┐
│  n                p                                            │  2 tokens  (ints)
│  <DISCARDED[p]>   ← unused per-processor vector (doubles)     │  p tokens
│  p_k_ind[p]       ← constant/induction power  (doubles)       │  p tokens
│  C_k_ef[p]        ← effective switching capacitance (doubles) │  p tokens
│  m_k[p]           ← DVFS power-law exponent       (doubles)     │  p tokens
│  f_k_ee[p]        ← energy-efficient frequency    (doubles)     │  p tokens
│  f_k_max[p]       ← maximum frequency             (doubles)     │  p tokens
│  compc[n][p]      ← computation cost matrix       (ints)       │  n·p tokens
│  commc[n][n]      ← communication cost matrix     (ints)       │  n² tokens
└──────────────────────────────────────────────────────────────┘
Total tokens = 2 + 6p + n·p + n²
```

**Proof of the layout** **[verified]** — `D1/LIGO/50Nodes/2PROCESSORS/1_2.txt` has
`n=50, p=2` and exactly `2614` whitespace tokens.
`2 + 6·2 + 50·2 + 50² = 2 + 12 + 100 + 2500 = 2614`. ✔ Exact match.

> **The discarded vector.** It is *not* one of the five used parameter vectors and is
> **read and thrown away** by `ERWS.cpp`/`algo3.cpp` (`double temp; … cin >> temp;`).
> It varies per file (values ≈ 0.10–0.40), is length `p`, and is unrelated to
> `f_k_ee` (which spans 0.02–0.99). Its origin is not recoverable from the repo.
> **Treat it as a format artefact, not data.** If you need it, capture it before
> the `cin >> temp;` loop. **[static + verified-by-value-range]**

### 5.2 Format D2 — used by `D2/**` files and by 6 of the 8 `Energy/` algorithms

```
┌──────────────────────────────────────────────────────────────┐
│  n                p                                            │  2 tokens  (ints)
│  p_k_ind[p]       ← constant/induction power  (doubles)       │  p tokens
│  C_k_ef[p]        ← effective switching capacitance (doubles) │  p tokens
│  m_k[p]           ← DVFS power-law exponent       (doubles)     │  p tokens
│  f_k_ee[p]        ← energy-efficient frequency    (doubles)     │  p tokens
│  f_k_max[p]       ← maximum frequency             (doubles)     │  p tokens
│  compc[n][p]      ← computation cost matrix       (ints)       │  n·p tokens
│  commc[n][n]      ← communication cost matrix     (ints)       │  n² tokens
└──────────────────────────────────────────────────────────────┘
Total tokens = 2 + 5p + n·p + n²
```

**Proof of the layout** **[verified]** — `D2/GE/9Nodes/2PROCESSORS/1_2.txt` has
`n=9, p=2` and exactly `111` tokens.
`2 + 5·2 + 9·2 + 9² = 2 + 10 + 18 + 81 = 111`. ✔ Exact match.

> **Format D2 is exactly Format D1 minus the discarded vector.** The two trees are
> otherwise structurally identical. **[verified]**

### 5.3 Format D3 ("stripped") — what the `Makespan/` algorithms actually parse

```
┌──────────────────────────────────────────────────────────────┐
│  n                p                                            │  2 tokens
│  compc[n][p]      ← computation cost matrix       (ints)       │  n·p tokens
│  commc[n][n]      ← communication cost matrix     (ints)       │  n² tokens
└──────────────────────────────────────────────────────────────┘
Total tokens = 2 + n·p + n²
```

**This format does not exist anywhere on disk.** Every `Makespan/*.cpp` except `cpmin.cpp`
has its header-skip block **commented out**:

```cpp
// for (int i = 0; i < 5; i++) { double x; for (int j = 0; j < p; j++) cin >> x; }
// for(int i = 0;i < p;i++){ int x; cin >> x; }
for (int i = 0; i < n; i++) for (int j = 0; j < p; j++) cin >> compc[i][j];
for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) cin >> commc[i][j];
```

**Implication:** the intended pipeline was *"strip the header off the dataset, then run
the Makespan algorithms on the stripped file."* That stripping step is missing from the
repo. `tools/make_stripped.py` in this repo performs it.

### 5.4 Parsing cheat-sheet (Python)

```python
def parse(path):
    t = open(path).read().split()
    n, p = int(t[0]), int(t[1]); i = 2
    fmt = None
    for cand, hdr in (("D1", p + 5*p), ("D2", 5*p), ("D3", 0)):
        j = i + hdr + n*p + n*n
        if j == len(t):
            fmt, i = cand, i + hdr
            break
    if fmt is None:
        raise ValueError(f"{path}: no format matches ({len(t)} tokens)")
    p_k_ind = [float(x) for x in t[i    : i+p]]; i += p
    C_k_ef  = [float(x) for x in t[i    : i+p]]; i += p
    m_k     = [float(x) for x in t[i    : i+p]]; i += p
    f_k_ee  = [float(x) for x in t[i    : i+p]]; i += p
    f_k_max = [float(x) for x in t[i    : i+p]]; i += p
    compc = [[int(t[i+r*p+c]) for c in range(p)] for r in range(n)]; i += n*p
    commc = [[int(t[i+r*n+c]) for c in range(n)] for r in range(n)]
    return dict(n=n, p=p, fmt=fmt, p_k_ind=p_k_ind, C_k_ef=C_k_ef, m_k=m,
                f_k_ee=f_k_ee, f_k_max=f_k_max, compc=compc, commc=commc)
```

The format can be **auto-detected by exact token count** — this is the most reliable
way to route a file to the right algorithm. **[verified]**

### 5.5 Value ranges in the data **[verified]**

| Quantity | Range | Notes |
|---|---|---|
| `p_k_ind` | 0.03 – 0.07 | leakage/induction power |
| `C_k_ef` | 0.80 – 1.19 | effective switching capacitance |
| `m_k` | 2.50 – 3.00 | DVFS power-law exponent (≈ 2.6 typical) |
| `f_k_ee` | 0.02 – 0.99 | energy-efficient frequency, **uniformly random per processor** |
| `f_k_max` | **always 1.0** | identical across all processors and all files |
| `compc` | ints, 0 – ~130 | task computation cost; **0 appears** (e.g. `D2/LA/11Nodes` task 0) |
| `commc` | ints ≥ 0, or **−1 = no edge** | diagonal is always −1 |

> `f_k_max = 1.0` everywhere means the DVFS sweep is always `f ∈ [f_ee[k], 1.0]`.
> `f_k_ee` being uniform-random (rather than a fixed 0.5) is a deliberate
> generalisation: it forces the algorithms to genuinely search for the energy-optimal
> frequency rather than assuming a single known point.

---

## 6. Algorithm catalogue — `Algorithms/Energy/`

All 8 files share the same skeleton:

```
read input → E_min[i], E_max[i] → E_min_g, E_max_g
            → E_given_g  (global budget)
            → E_pre[i]  (per-task cap)      ← ★ THE ALGORITHM'S ACTUAL CONTRIBUTION ★
            → upward rank rup[i] → priority order seq
            → for each task in seq: EST/EFT + DVFS frequency selection under E_pre[i]
            → print makespan
```

**Common output contract:** a single line on `stdout` containing the **makespan**
(a `double`). `ERWS`/`algo3` also compute `t_energy` but only print it in commented-out
lines — **you will need to re-enable those to collect energy numbers.**

**Common command-line contract:** `argv[1]` = `PARAM`, a multiplier on the energy budget
(default `1.0`). All algorithms except `ESECC` use `E_given = (1 + PARAM) · E_min_g`, so
`PARAM=0` ⇒ budget = exactly the minimum feasible energy; larger `PARAM` ⇒ more energy
⇒ shorter makespan. **[verified]**

### 6.1 Per-file summary

| File | Format | Priority | Energy-cap strategy | Notes |
|---|---|---|---|---|
| `ERWS.cpp` | **D1** | upward rank `rup` | proportional to `E_min[i]/E_min_g` | the "ERWS" baseline |
| `algo3.cpp` | **D1** | upward rank `rup` | blend of energy-span ratio and rank ratio (`alp = 0.6`) | the D1 "reference" |
| `EHEFT.cpp` | **D2** | `rankp = ranku − rankd + rankd(exit)` | **weighted slack sharing** with `W(t)` weights | newest; has a safe fallback; only algo with a *fallback* when no frequency fits |
| `ESECC.cpp` | **D2** | upward rank `rup` | `E_min[i] + E_ie·(E_avg[i]/E_avg_g)` | ⚠ **budget-formula bug** (§12.1) |
| `ISAECC.cpp` | **D2** | upward rank `rup` | `E_min[i] + E_ie·(E_avg[i]/E_avg_g)`, `E_ie = E_given − E_min_g` | same family as ESECC, correct budget |
| `MSLECC.cpp` | **D2** | upward rank `rup` | **no static allocation** — cap recomputed dynamically per task from the running energy balance | "minimum-slack" style |
| `REP.cpp` | **D2** | upward rank `rup` | proportional to the task's **energy span** `espan[i] = min_k (E_max(i,k) − E_min(i,k))` | "range/span-energy partition" |
| `RSMECC.cpp` | **D2** | upward rank `rup` | uniform interpolation: `E_min + (E_max−E_min)·ecl_g` where `ecl_g = (E_given−E_min_g)/(E_max_g−E_min_g)` | "resource-energy level" style |

### 6.2 The `task[][]` table (shared by all 8) **[static]**

```cpp
vector<vector<double>> task(n, vector<double>(6));
// [0] = AST  (earliest start time)
// [1] = AFT  (earliest finish time)
// [2] = processor index
// [3] = chosen frequency f
// [4] = energy consumed by this task
// [5] = E_pre, the per-task energy cap that was applied
```

### 6.3 The selection loop (shared by 7 of 8) **[static]**

```cpp
for each task t in priority order seq:
    cap = min( e_giv(E_given, task, e_min, seq, i),   // running budget minus E_min of the rest
               e_max[t] );
    for each processor j:
        est = max( avail[j],
                   max over preds u of ( task[u].AFT + (task[u].proc == j ? 0 : commc[u][t]) ) );
        for (f = f_ee[j]; f <= f_max[j]; f += 0.01):      // 0.01 frequency grid
            energy = calen(p_ind[j], C_ef[j], f, m[j], compc[t][j], f_max[j]);
            if (energy > cap) continue;                   // frequency violates the cap
            eft = est + compc[t][j] * (f_max[j] / f);
            if (eft < min_eft) { record(est, eft, j, f, energy); }
    avail[ chosen_processor ] = chosen_AFT;
```

**Two important behavioural notes:**

* Because `f` is swept **upwards** and the `EFT` decreases with `f`, the loop effectively
  selects **the highest frequency that still satisfies the energy cap**. That is the
  makespan-optimal choice under a per-task energy cap — correct, but it means the
  *only* freedom left is *which* task gets *how much* of the slack. That is precisely
  what distinguishes these 8 algorithms from one another.
* **If no frequency on any processor satisfies the cap, the task is silently left
  UNASSIGNED** — `task[t][0..4]` are never written, so `AFT = 0` and the processor
  defaults to `0`. The task silently occupies **zero time**. This is the failure mode
  behind the `ESECC` bug and the reason `EHEFT` added an explicit fallback.

### 6.4 `EHEFT.cpp` — the newest algorithm (different in three ways) **[static]**

1. **Priority**: `rankp[i] = ranku[i] − rankd[i] + rankd(exit_sink)` (ESMM-style
   "path rank"), not plain upward rank. `rankd` is computed in topological order via Kahn's
   algorithm; `ranku` via memoised DFS.
2. **Energy allocation**: *weighted slack sharing*.
   * `FED(t)` = average energy of task `t` on its **top-2 favourite processors** (the two
     with the smallest `compc[t][k]`), evaluated at `f_max`.
   * `W(t) = FED(t) · (1 + 1/(√LN(t) + √LN(entry)))`, where `LN` is the DAG level number
     and `entry` is the first predecessor-free task. This *up-weights* tasks that are
     shallow in the DAG.
   * `Eslack = E_given − Σ_{remaining} E_min` (the slack still to be distributed).
   * `E_pre(t) = E_min(t) + (W(t) / Σ_{remaining} W) · Eslack`, clamped to `[E_min, E_max]`.
3. **Safety fallback**: if no frequency fits the cap, it falls back to `f = f_max` for that
   processor so the schedule always remains **feasible** (at the cost of overshooting the
   per-task cap). This is the only Energy algorithm that cannot emit a zero-duration task.

`EHEFT` is also the only Energy algorithm that is written in a *modern* style
(`static inline` helpers, `const` references, iterative topological sort, `rnd4` rounding
helper) rather than the copy-pasted 2010-era style of the other seven.

---

## 7. Algorithm catalogue — `Algorithms/Makespan/`

These are **classic heterogeneous-DAG list-scheduling heuristics**. They do **not**
model energy at all; the 5 parameter vectors are skipped (or, for `cpmin`, read and
discarded) because they are irrelevant to them.

**Common output contract** — a schedule table on `stdout`:

```
<priority order as comma- or space-separated task ids>
TASKS  AST  AFT  PRO          ← header line varies slightly per file
1      0    13   2            ← one row per task, tab-separated
...
```

The exact header string differs per file: `heft.cpp` prints `"Tasks\tAST\tAFT\tProcessor"`,
`peft.cpp` prints `"Tasks\tST\tFT\tProc"`, `aeft.cpp`/`ippts.cpp` print
`"TASKS\tAST\tAFT\tPRO"`, `oeft.cpp` prints `"TASKS\tST\tFT\tPRO"`. **A parser must
tolerate both the 4-column table and the preceding priority-order line.**

### 7.1 Per-file summary

| File | Priority metric | Processor-selection rule | Notes |
|---|---|---|---|
| `heft.cpp` | upward rank `rup` (scaled ×10000) | classic **EFT** | the canonical HEFT baseline |
| `peft.cpp` | `oct[i]` = optimal computation time per processor, averaged | minimise `EFT + oct` | **PEFT** (Li et al.) |
| `aeft.cpp` | `ioct[i]` = initial optimal computation time, averaged | minimise `EFT + ioct` | **AEFT** |
| `cpop.cpp` | — | — | "critical-path-oriented" variant |
| `ipeft.cpp` | — | — | improved PEFT variant |
| `ippts.cpp` | `pcm[i]` × out-degree | minimise `EFT + lhead` | ⚠ **violates dependencies** (§12.4) |
| `lstd.cpp` | — | — | "list scheduling with task duplication" variant |
| `mpeft.cpp` | — | — | modified PEFT variant |
| `oeft.cpp` | `pcg[i]` × out-degree | minimise `EFT + pcg` | ⚠ **violates dependencies** (§12.4) |
| `ppts.cpp` | — | — | PEFT-priority-to-schedule variant |
| `cpmin.cpp` | n/a | n/a | **lower bound**: longest path of per-task `min_k compc[i][k]`. Prints one integer. Runs on **raw D2**. |
| `new.cpp` | — | — | ⚠ **does not compile**; dead scratch code |

### 7.2 `cpmin.cpp` is the reference lower bound

```cpp
w[i] = min_k compc[i][k];
// longest path in the DAG using w as node weights (Kahn topological order + DP)
// → bestCost = max over sinks of dp[sink]
```

This is a **valid makespan lower bound** for any list scheduler (it ignores
communication, which is safe because communication may overlap with computation on a
shared processor). Use it to sanity-check every other algorithm's output. **[verified]**

For `D2/GE/9Nodes/2PROCESSORS/1_2.txt` the bound is **229**, and a well-behaved
HEFT variant reaches exactly 229 (i.e. the bound is tight for that instance).

---

## 8. Algorithm ↔ format compatibility matrix

**This is the single most important table in the document.** It was determined by
compiling every algorithm, instrumenting its parser to dump the values it actually
read, and comparing against the ground-truth parse of the dataset. **[verified]**

### 8.1 `Algorithms/Energy/` — all read **stdin**, all print **one makespan**

| Algorithm | Header tokens consumed | ✅ Works on | ❌ On the other format |
|---|---|---|---|
| `ERWS` | 5p params **+ p discard** | **D1** | D2 → segfault |
| `algo3` | 5p params **+ p discard** | **D1** | D2 → segfault |
| `EHEFT` | 5p params | **D2** | D1 → misparse, 18 dependency violations |
| `ESECC` | 5p params | **D2** | D1 → misparse |
| `ISAECC` | 5p params | **D2** | D1 → misparse |
| `MSLECC` | 5p params | **D2** | D1 → misparse |
| `REP` | 5p params | **D2** | D1 → misparse |
| `RSMECC` | 5p params | **D2** | D1 → misparse |

### 8.2 `Algorithms/Makespan/` — all read **stdin**, all print a **schedule table**

| Algorithm | Header tokens consumed | ✅ Works on | ❌ On raw D1/D2 |
|---|---|---|---|
| `cpmin` | 5p skip | **D2 (raw)** | D1 → misparse (silent wrong number) |
| `heft` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `peft` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `aeft` | none | **D3 (stripped)** | D1/D2 → **silently wrong** (no deps) |
| `cpop` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `ipeft` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `ippts` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `lstd` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `mpeft` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `oeft` | none | **D3 (stripped)** | D1/D2 → **silently wrong** (no deps) |
| `ppts` | none | **D3 (stripped)** | D1/D2 → **segfault** |
| `new` | — | — | **does not compile** |

### 8.3 Why the segfaults happen

`heft.cpp`'s `calrp()` is a **non-memoised recursion**:

```cpp
if (commc[tind][i] != -1) { if (fin[i]==0) { rup[i] = calrp(i, ...); fin[i]=1; } ... }
```

If `commc` is misparsed so that *every* entry is `0` (i.e. no `-1` sentinel), every node
looks like a successor of every other node → **infinite recursion → stack overflow →
SIGSEGV (rc = −11)**. `aeft.cpp` and `oeft.cpp` initialise `commc` to `-1` instead, so
they don't crash — but they end up with a **dependency-free graph** and emit a
meaningless schedule. **A "feasible" schedule from these two is not evidence of
correctness.** **[verified — this bit me during this analysis; a naive feasibility check
on a misparsed run reports a vacuous PASS]**

---

## 9. Shared building blocks

### 9.1 The energy model **[static, identical in all 8 Energy files]**

```cpp
double calen(double p_k_ind, double c_k_ef, double f_k_h, double m_k,
             double compc, double f_k_max) {
    return (p_k_ind + c_k_ef * pow(f_k_h, m_k)) * compc * (f_k_max / f_k_h);
}
```

* **Power** `P(f) = p_k_ind + c_k_ef · f^m` — constant term + frequency-dependent term.
* **Time** `T(f) = compc · (f_max / f)` — inversely proportional to frequency.
* **Energy** `E = P(f) · T(f)`.

The DVFS energy curve is therefore U-shaped: minimum at
`f* = (m·c_ef·compc / p_k_ind)^(1/(m−1))` after accounting for the `f_max/f` factor.
The algorithms search the `[f_ee, f_max]` grid at 0.01 resolution to exploit this.

### 9.2 Per-task energy bounds **[static]**

```cpp
E_min[i] = min_k calen(p_ind[k], C_ef[k], f_ee[k], m_k[k], compc[i][k], f_max[k]);
E_max[i] = max_k calen(p_ind[k], C_ef[k], f_max[k], m_k[k], compc[i][k], f_max[k]);
```

### 9.3 Upward rank (HEFT priority) **[static]**

```cpp
mw[i]   = (1/p) · Σ_k compc[i][k];              // average computation cost
rup[i]  = mw[i] + max over successors s of ( commc[i][s] + rup[s] );
```

Computed by memoised DFS (`calrp`/`cal`) in the 7 classic Energy files, and by an
explicit topological sort in `EHEFT`. Sorted **descending by rank, ties broken by
ascending task index** (`mycomp`).

Because `rup[u] ≥ mw[u] + rup[s] > rup[s]`, the resulting order is **topologically
valid** — a successor never precedes its predecessor. (This is the property that
`ippts` and `oeft` break — §12.4.)

### 9.4 EST / EFT **[static]**

```cpp
est = max( avail[j],
            max over predecessors u of ( task[u].AFT + (same_proc ? 0 : commc[u][t]) ) );
eft = est + compc[t][j] * (f_max[j] / f);
```

**Communication is skipped when the predecessor is on the same processor** — i.e.
communication time is assumed to overlap with the predecessor's computation. This is the
standard HEFT assumption and it is why the correct makespan lower bound **excludes**
communication costs (see `cpmin`).

### 9.5 Rounding convention **[static]**

All Energy files round to 4 decimal places with `round(x * 10000.0) / 10000.0`
(`EHEFT` factors this into a helper named `rnd4`). Frequencies are swept on a
0.01 grid and rounded with `round(f * 100.0) / 100.0`. This rounding is applied
inconsistently (sometimes before, sometimes after comparisons), so **expect small
run-to-run discrepancies between implementations** and do not over-interpret
differences below ~1e-4.

---

## 10. The real differentiator: per-task energy allocation

Strip away the shared scaffolding and the 8 Energy algorithms differ in exactly **one
decision**: how to distribute the global slack `E_given − E_min_g` across the `n` tasks.

Let `Eslack = E_given − E_min_g` be the distributable slack.

| Algorithm | Allocation rule (per task `i`) | Intuition |
|---|---|---|
| `ERWS` | `E_min[i] + (E_min[i]/E_min_g)·Eslack` | share proportional to **minimum energy** — heavy tasks get more |
| `algo3` | `E_min[i] + w_i/Σw · Eslack`, `w_i = 0.6·r_i + 0.4·s_i`, `r_i = (E_max−E_min)/E_max`, `s_i = rup_i/max rup` | blend of **energy-span** and **structural rank** |
| `ISAECC` | `E_min[i] + (E_avg[i]/E_avg_g)·Eslack` | share proportional to **average energy** |
| `MSLECC` | *no static split* — the cap is recomputed **at scheduling time** from the live energy balance: `cap = E_given − (energy already spent) − (E_min of all remaining tasks)` | **greedy / dynamic**; the last task gets whatever is left |
| `REP` | `E_min[i] + (espan[i]/Σespan)·Eslack`, `espan[i] = min_k (E_max(i,k) − E_min(i,k))` | share proportional to **energy span** (how much room the task has) |
| `RSMECC` | `E_min[i] + (E_max[i]−E_min[i])·ecl_g`, `ecl_g = Eslack/(E_max_g−E_min_g)` | **uniform interpolation** to a global energy level |
| `ESECC` | `E_min[i] + (E_avg[i]/E_avg_g)·E_ie` — *same shape as ISAECC*, but with the buggy budget | see §12.1 |
| `EHEFT` | `E_min[i] + (W(i)/Σ_remaining W)·Eslack`, `W(i) = FED(i)·(1 + 1/(√LN(i)+√LN(entry)))` | **weighted slack sharing**, biased toward shallow DAG levels and toward tasks whose favourite processors are energy-hungry |

`MSLECC` is the odd one out structurally: it does **no static allocation at all**.
Its `prosele()` recomputes the cap on the fly via `e_giv()`, which sums the energy
*already consumed* by scheduled tasks plus the `E_min` of all *not-yet-scheduled* tasks.
The other 7 allocate statically in `main()` and then (for 6 of them) additionally clamp
against `e_giv()` at scheduling time.

> **Note:** `EHEFT` *also* clamps against a live slack term, but its allocation is
> genuinely *weighted* — the `W(i)` factor means the slack is not distributed
> proportionally to task size, but toward tasks that are structurally "important"
> (low level number) yet still energy-hungry on their favourite processors. This is the
> most interesting research contribution in the repo.

### 10.1 Observed behaviour (D2/GE/9Nodes/2PROCESSORS, `PARAM` sweep) **[verified]**

Critical-path lower bound = **229**. Unconstrained HEFT reference = **352**.

| `PARAM` | EHEFT | ESECC | ISAECC | MSLECC | REP | RSMECC |
|---|---|---|---|---|---|---|
| 0.0 | 509.87 | **22.09 ⚠** | 955.54 | 955.54 | 955.54 | 955.54 |
| 0.5 | 491.09 | **112.53 ⚠** | 561.51 | 770.81 | 540.93 | 576.12 |
| 1.0 | 392.16 | 955.54 | 477.87 | 603.55 | 439.80 | 473.77 |
| 2.0 | 342.19 | 459.60 | 385.41 | 353.49 | 347.64 | 395.56 |
| 4.0 | 301.00 | 356.44 | 352.00 | 352.00 | 352.00 | 352.00 |

Observations:

* **All algorithms are monotone in `PARAM`** (more energy ⇒ shorter makespan) *except*
  `ESECC` (buggy budget) and `REP` (tiny non-monotonicity at `PARAM=2`: 347.64 → 352.00).
* All algorithms **converge to 352.0** (the unconstrained HEFT makespan) as the budget
  grows — a good sign of correctness.
* **`EHEFT` reaches 301.0 at `PARAM=4`, better than every other algorithm's 352.0**
  and with a dependency-feasible schedule (0 violations). This is because `EHEFT` selects
  the frequency that *minimises EFT among feasible frequencies* (highest feasible `f`)
  and uses a smarter priority (`rankp`). **This is the headline result to reproduce.**
* `ESECC`'s values below 229 are **invalid** (§12.1).

---

## 11. Build & run instructions

### 11.1 Compiler requirements **[verified]**

All 20 files start with `#include <bits/stdc++.h>` → **GCC required**.
Apple's `clang` does not ship that header, and `/usr/bin/g++` on macOS is *clang in
disguise*, so a naive `g++ file.cpp` fails on a stock Mac.

**The easiest path — use the provided script, which handles both gotchas:**

```bash
bash tools/build_all.sh
# -> 19 binaries in ./build/, 1 expected failure (Makespan/new.cpp)
```

It automatically (1) generates a `bits/stdc++.h` shim if the compiler lacks one and
(2) picks the newest linkable macOS SDK.

**Manual route (Linux with real GCC):**

```bash
g++ -O2 -std=c++17 -o build/Energy_HEFT Algorithms/Energy/EHEFT.cpp
```

**Manual route (macOS with Apple clang) — the two gotchas:**

1. `bits/stdc++.h` is missing → either `brew install gcc` (then use `g++-14`/`g++-13`)
   **or** provide a shim header that includes the standard headers and compile with
   `-I <shim_dir>`.
2. On recent macOS the default SDK (`MacOSX27.0.sdk`) has a `libSystem.tbd` the linker
   rejects (`malformed file` / `unknown architecture`). Use an older SDK:

   ```bash
   SDK=/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk   # 15.4 also works
   g++ -O2 -std=c++17 -isysroot "$SDK" -I build/shim -o build/Energy_HEFT \
       Algorithms/Energy/EHEFT.cpp
   ```

### 11.2 Build everything

```bash
cd /Users/silaparasettilohith/Desktop/Research
bash tools/build_all.sh
```

<details>
<summary>Manual equivalent</summary>

```bash
SDK=/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk   # macOS only; omit on Linux
mkdir -p build
for f in Algorithms/Energy/*.cpp Algorithms/Makespan/*.cpp; do
  name=$(basename "$f" .cpp)
  fam=$(basename "$(dirname "$f")")
  g++ -O2 -std=c++17 -isysroot "$SDK" -o "build/${fam}_${name}" "$f" \
    || echo "BUILD FAILED: $f"
done
# Expected: 19/20 succeed. Algorithms/Makespan/new.cpp is expected to fail.
```
</details>

### 11.3 Run

**All programs read the instance from `stdin`.** There is no file argument.

```bash
# Energy (prints one makespan)
./build/Energy_ERWS   1.0 < D1/LIGO/50Nodes/2PROCESSORS/1_2.txt
./build/Energy_EHEFT  1.0 < D2/GE/9Nodes/2PROCESSORS/1_2.txt

# Makespan (prints a schedule table) — needs a STRIPPED file first
python3 tools/make_stripped.py D2/GE/9Nodes/2PROCESSORS/1_2.txt /tmp/ge9_2.stripped
./build/Makespan_heft < /tmp/ge9_2.stripped

# Validate any result
python3 tools/check_schedules.py D2/GE/9Nodes/2PROCESSORS/1_2.txt /tmp/result.out
```

### 11.4 Runtime characteristics **[static, estimated]**

| Algorithm | Complexity | Dominant cost |
|---|---|---|
| Energy (all) | `O(n²·p + n·p·100)` | the EST loop scans **all `n` predecessors for each of `p` processors**, then a 100-step frequency sweep per (task, processor) |
| `heft`, `aeft`, `oeft`, `ippts` | `O(n²)` *without* memoisation | `calrp`/`calioct`/`calpcg`/`calpcm` recurse per successor — **exponential in the worst case** on dense DAGs |
| `cpmin` | `O(n²)` | one topological pass |

Practical consequence: **`D2/LA/2081Nodes/64PROCESSORS` (13 MB, 2081 tasks) and
`D1/*/1000Nodes/64PROCESSORS` are the expensive cases.** The `n²` predecessor scan in the
Energy family is `2081² × 64 ≈ 277 M` iterations per instance. Budget accordingly
(or parallelise across the 6 processor-count directories).

---

## 12. Known bugs and hazards (empirically confirmed)

### 12.1 🔴 `ESECC.cpp` — wrong global-budget formula (critical)

```cpp
// ESECC.cpp, line ~211
double E_given_g = (PARAM)*E_min_g;          // ← BUG
E_avail = E_given_g - E_min_g;               // ← negative for PARAM < 1
```

Every other Energy algorithm uses `E_given_g = (1 + PARAM) * E_min_g`.
Consequences for `PARAM < 1`:

1. `E_given < E_min_g` ⇒ the budget is **below the minimum energy any schedule can use**.
2. `E_avail < 0` ⇒ `E_aa[i] = E_min[i] + E_avail/n < E_min[i]` ⇒ the per-task cap is
   below the task's minimum possible energy.
3. **No frequency satisfies the cap** ⇒ `prosele()` never writes `task[t][0..4]` ⇒ the
   task is **silently unassigned with AFT = 0** (zero duration!).
4. The reported makespan is therefore **below the critical-path lower bound** — i.e.
   mathematically impossible.

**Reproduction** **[verified]**

```bash
./build/Energy_ESECC 0.0 < D2/GE/9Nodes/2PROCESSORS/1_2.txt
# → 22.093      ← IMPOSSIBLE: lower bound is 229
# 8 of 9 tasks are unassigned (AFT = 0)
```

| `PARAM` | makespan | unassigned tasks | valid? |
|---|---|---|---|
| 0.0 | 22.093 | 8 / 9 | ❌ below lower bound |
| 0.25 | 166.277 | 8 / 9 | ❌ below lower bound |
| 0.5 | 112.525 | 7 / 9 | ❌ below lower bound |
| 0.75 | 219.038 | 5 / 9 | ❌ below lower bound |
| ≥ 1.0 | ≥ 229 | 0 | ✅ |

**Suggested fix:** change to `double E_given_g = (1 + PARAM) * E_min_g;`
and, defensively, add a fallback (as `EHEFT` already has) so a task can never be
left unassigned.

### 12.2 🟠 Silent zero-duration tasks in the shared Energy selection loop

The "no feasible frequency ⇒ task silently unassigned" behaviour (§6.3) lives in
`ERWS`, `algo3`, `ESECC`, `ISAECC`, `MSLECC`, `REP`, `RSMECC`.
`ESECC` is the only one that triggers it in practice at reasonable `PARAM` values,
but the hazard is shared. **Any Energy algorithm run at a tight budget can silently
emit an invalid schedule.** A validator must check for `AFT == 0` tasks (except
legitimately-zero-cost tasks — `D2/LA/11Nodes` task 0 has `compc = [0,0]`).

### 12.3 🟠 `Makespan/new.cpp` — does not compile

```cpp
21:            f >> compc[i][j];      // error: use of undeclared identifier 'f'
```

It also reads `commc` with an `n×p` loop over an `n×n` matrix (a second bug), and
calls `f.close()` on an undeclared file. It is **dead scratch code** — it looks like an
attempt to write a format converter. Delete it or rewrite it as a proper converter
(`tools/make_stripped.py` already fills that role).

### 12.4 🟠 `Makespan/ippts.cpp` and `oeft.cpp` — dependency-violating schedules

Both compute a priority that **does not preserve topological order**:

```cpp
// ippts.cpp:115 and oeft.cpp:166
prank[i] = (k / p) * outdeg[i];      // k = Σ_procs pcg[i][j]
```

Multiplying by out-degree can push a task ahead of its own predecessors. The
scheduling loop then reads `task[k][1] = 0` for a not-yet-scheduled predecessor,
producing an `EST` that is too small and a schedule that **violates the DAG
dependencies**.

**Reproduction** **[verified]** — `D2/GE/9Nodes/2PROCESSORS/1_2.txt`, stripped:

```
$ ./build/Makespan_oeft < /tmp/ge9_2.stripped
1,5,2,3,6,4,7,8,9,            ← task 5 scheduled before its predecessor task 2
TASKS ST FT PRO
1     0  13  2
2    13  108 2
3    58  120 1
4   139  159 1
5     0  40  1                ← AFT(2)=108, comm(2,5)=81 ⇒ must start ≥ 189
...
```
Edge `2→5` (cost 81) requires `AFT(2) + 81 ≤ ST(5)`, i.e. `189 ≤ 0`. **Violated.**

| Algorithm | D1/LIGO/50N violations | D2/GE/9N violations |
|---|---|---|
| `aeft` | 0 | 0 |
| `cpop` | 0 | 0 |
| `heft` | 0 | 0 |
| `ipeft` | 0 | 0 |
| `lstd` | 0 | 0 |
| `mpeft` | 0 | 0 |
| `peft` | 0 | 0 |
| `ppts` | 0 | 0 |
| **`ippts`** | **12** | **1** |
| **`oeft`** | **12** | **1** |

**Suggested fix:** replace the out-degree-multiplied priority with a topological-order
respected one (e.g. rank-based, as in `heft`), or explicitly sort the priority list so
that no task precedes any of its predecessors.

### 12.5 🟡 `Makespan/heft.cpp` — silent `long`→`int` truncation

```cpp
vector<pair<int, long>> pseq;
pseq.push_back(pair<int, long>(i + 1, rup[i]));
sort(pseq.begin(), pseq.end(), mycomp);   // mycomp takes pair<long,int> !
```

`mycomp` is declared as `bool mycomp(pair<long, int>, pair<long,int>)`, so each
`pair<int,long>` is **converted** to `pair<long,int>` — the rank is truncated to `int`
and the task id is widened. For large `n` with `rup` scaled by 10000 the rank can
exceed `INT_MAX` and the ordering becomes wrong. Same pattern in `ERWS.cpp`/`algo3.cpp`
(where `rup` is a `double` truncated to `long`). **[static]**

### 12.6 🟡 Non-memoised recursion in the Makespan family

`heft.cpp:calrp`, `aeft.cpp:calioct`, `oeft.cpp:calpcg`, `ippts.cpp:calpcm` all use the
same `if (fin[j]==0) { recurse(j); fin[j]=1; }` pattern, which recomputes shared
sub-DAGs repeatedly. On a dense DAG this is exponential. It also means that a *cyclic*
input would hang/crash — the datasets are acyclic, so this is a performance issue, not
a correctness one. **[verified: all datasets are acyclic, diagonals are −1]**

### 12.7 🔴 `D1/*/run.sh` — destructive cleanup script

`D1/LIGO/run.sh`, `D1/Montage/run.sh` and `D1/Genome/run.sh` are **byte-identical**
(md5 `d47a64c404ae1ba06e014a5b1e62db03`) and are a copy of `clean_except.sh`:

```bash
for top in */ ; do
  for item in "$top"* ; do
    name=$(basename "$item")
    skip=false
    for keep in "2PROCESSORS" "4PROCESSORS" "8PROCESSORS" "16PROCESSORS" "32PROCESSORS" "64PROCESSORS"; do
      [[ $name == "$keep" ]] && { skip=true; break; }
    done
    $skip || rm -rf -- "$item"          # ← DELETES EVERYTHING ELSE
  done
done
```

It is meant to be run **inside a `*Nodes/` directory** to strip that scale down to the
six `*PROCESSORS` subfolders. Run it from `D1/LIGO/` itself and it will `rm -rf` the
`50Nodes/ … 1000Nodes/` directories — **destroying the dataset**. `D2/` has no such
script. **Do not run these without reading them first.** **[verified]**

### 12.8 🟡 `ERWS.cpp` / `algo3.cpp` compute but never print the energy

Both compute `t_energy` but the print is commented out. To collect the energy-vs-makespan
trade-off curve (the whole point of an energy-constrained scheduler) you must uncomment:

```cpp
// cout << makespan << " " << t_energy << endl;
```

`EHEFT.cpp` does not even compute `t_energy`.

---

## 13. Validation harness

`tools/` (added by this analysis) contains the scripts used to produce every
`[verified]` claim in this document.

| Script | What it does |
|---|---|
| `tools/build_all.sh` | Compiles every algorithm into `build/`. Auto-generates a `bits/stdc++.h` shim and picks a linkable macOS SDK. Expect **19 OK / 1 FAIL** (`Makespan/new.cpp`). |
| `tools/verify_formats.py` | Compiles every algorithm, instruments its parser to dump what it *actually* read, and compares against the ground-truth parse → produces the §8 compatibility matrix. |
| `tools/check_schedules.py` | Parses a schedule table, checks **dependency feasibility** (the correct rule: add communication cost only when predecessor and successor are on *different* processors), checks for **unassigned (AFT=0)** tasks, and reports the `cpmin` lower bound. |
| `tools/make_stripped.py` | Generates a **Format D3** file from a D1 or D2 file (the missing preprocessing step). |

**Reproducing §8 (the compatibility matrix):**

```bash
python3 tools/verify_formats.py
```

Expected output:

```
Energy/EHEFT       D1=mismatch  D2=MATCH
Energy/ERWS        D1=MATCH     D2=mismatch
Energy/ESECC       D1=mismatch  D2=MATCH
Energy/ISAECC      D1=mismatch  D2=MATCH
Energy/MSLECC      D1=mismatch  D2=MATCH
Energy/REP         D1=mismatch  D2=MATCH
Energy/RSMECC      D1=mismatch  D2=MATCH
Energy/algo3       D1=MATCH     D2=mismatch
Makespan/cpmin     D1=mismatch  D2=MATCH
Makespan/*         D1=mismatch  D2=mismatch   (all except cpmin -> need Format D3)
Makespan/new       BUILD FAILED
```

**Minimum validation an algorithm must pass before its numbers are trustworthy:**

1. ✅ **Parse check** — the values the algorithm read match the file (token-count based).
2. ✅ **Lower bound** — `makespan ≥ cpmin(instance)`.
3. ✅ **Dependency feasibility** — for every edge `u→v`: if `proc(u) ≠ proc(v)` then
   `AFT(u) + commc[u][v] ≤ AST(v)`; if `proc(u) = proc(v)` then `AFT(u) ≤ AST(v)`.
4. ✅ **No unassigned tasks** — every task has a non-zero AFT (except tasks whose
   `compc` is genuinely 0, e.g. `D2/LA/11Nodes` task 0).
5. ✅ **Energy budget** — `Σ task[i].energy ≤ E_given`.
6. ✅ **Monotonicity** — makespan must be non-increasing in `PARAM`.

> ⚠ **Warning — two traps.**
>
> **(a) The dependency-feasibility check is *vacuous* on a misparsed run.** If the
> algorithm parsed garbage, `commc` may be all `−1` (no edges) and the check passes
> trivially. **Always pair the feasibility check with a parse check.**
> This exact trap occurred during this analysis: `aeft` and `oeft` initially looked
> "perfectly feasible" on raw D1/D2 files, and only the parse check revealed that they
> had read nothing.
>
> **(b) The lower bound must EXCLUDE communication cost.** HEFT-family schedulers assume
> communication to a task on the *same* processor overlaps with the predecessor's
> computation. A bound that charges communication therefore rejects valid schedules
> and produces false "impossible result" alarms. `check_schedules.py` uses the
> correct (communication-free) bound.

---

## 14. Suggested research directions

1. **Fix and re-baseline `ESECC`** (§12.1), then re-run the `PARAM` sweep to see
   whether ISAECC's allocation is genuinely better than ESECC's, or whether the
   difference was entirely the bug.
2. **Unify the Energy algorithms on one code path.** The 7 classic files are ~95 %
   identical copy-paste; only `main()`'s allocation block differs. Refactor to one
   `common.h` with a pluggable allocator interface — this removes the entire class of
   "the budget formula drifted between files" bugs. `EHEFT.cpp` is already the
   right shape.
3. **Make the silent-failure mode impossible.** Adopt `EHEFT`'s fallback (use `f_max`
   when nothing fits) everywhere, *or* fail loudly. The current behaviour produces
   mathematically impossible makespans that look like good results.
4. **Fix `ippts` / `oeft` dependency violations** (§12.4) and re-measure them — their
   current numbers are not comparable to the other Makespan algorithms.
5. **Build the missing Format D3 pipeline** and automate the full benchmark:
   1920 D1 instances × 2 applicable Energy algorithms + 180 D2 instances × 6 Energy
   algorithms + all Makespan algorithms, across a `PARAM` grid.
6. **Collect energy, not just makespan.** Every published claim about these algorithms
   needs the energy axis; that requires re-enabling the `t_energy` output (§12.8).
7. **Reconcile `EHEFT`'s 301.0 vs. the family's 352.0** (D2/GE, `PARAM=4`).
   `EHEFT` wins by using a *different priority* (`rankp`) and *EFT-minimising*
   frequency selection. Is that a genuine improvement or an artefact of comparing
   against algorithms that saturate? Worth isolating with an ablation.
8. **Scale.** The `n²` predecessor scan and the `n²` `commc` matrix make the
   2000-node instances expensive. Consider a sparse adjacency representation.

---

## Appendix A — quick reference

### A.1 Format tokens

| Layout | Token sequence | Total |
|---|---|---|
| **D1** | `n p` · `p` discarded · `p_k_ind[p]` · `C_k_ef[p]` · `m_k[p]` · `f_k_ee[p]` · `f_k_max[p]` · `compc[n][p]` · `commc[n][n]` | `2 + 6p + np + n²` |
| **D2** | `n p` · `p_k_ind[p]` · `C_k_ef[p]` · `m_k[p]` · `f_k_ee[p]` · `f_k_max[p]` · `compc[n][p]` · `commc[n][n]` | `2 + 5p + np + n²` |
| **D3** | `n p` · `compc[n][p]` · `commc[n][n]` | `2 + np + n²` |

### A.2 Routing table

```
file format ──┬── D1 ──▶ ERWS, algo3                      (Energy)
               ├── D2 ──▶ EHEFT, ESECC, ISAECC, MSLECC, REP, RSMECC   (Energy)
               │         cpmin                            (Makespan, lower bound only)
               └── D3 ──▶ heft, peft, aeft, cpop, ipeft, ippts,
                          lstd, mpeft, oeft, ppts          (Makespan)
```

### A.3 Command-line contract

| Program | argv | stdin | stdout |
|---|---|---|---|
| `Energy/*` | `argv[1]` = `PARAM` (default `1.0`) | instance file | one `double` (makespan) |
| `Makespan/*` (except `cpmin`) | none | **stripped** instance | priority list + 4-column table |
| `Makespan/cpmin` | none | D2 instance | one `int` (lower bound) |

### A.4 Key constants

| Symbol | Meaning | Value in data |
|---|---|---|
| `p_k_ind[k]` | constant/induction power | 0.03 – 0.07 |
| `C_k_ef[k]` | effective switching capacitance | 0.80 – 1.19 |
| `m_k[k]` | DVFS power-law exponent | 2.50 – 3.00 |
| `f_k_ee[k]` | energy-efficient frequency | 0.02 – 0.99 (random) |
| `f_k_max[k]` | max frequency | **always 1.0** |
| `compc[i][k]` | computation cost of task `i` on processor `k` | int ≥ 0 |
| `commc[i][j]` | communication cost `i→j` | int ≥ 0, **`−1` = no edge** |

---

## Appendix B — glossary

| Term | Meaning in this repo |
|---|---|
| **AST / AFT** | Earliest Start Time / Earliest Finish Time of a task in the produced schedule |
| **EFT** | Earliest Finish Time on a *specific* processor: `EST + compc[t][k]·(f_max/f)` |
| **EST** | Earliest Start Time on a specific processor: max over predecessors' ready times and the processor's availability |
| **upward rank (`rup`)** | `mw[i] + max_s(commc[i][s] + rup[s])` — the standard HEFT priority; guarantees a topological order |
| **`rankp`** | `EHEFT`'s ESMM-style priority: `ranku − rankd + rankd(exit sink)` |
| **`mw`** | mean computation cost of a task across processors: `(1/p)·Σ_k compc[i][k]` |
| **`E_min[i]`** | minimum energy for task `i` over all processors, at each processor's `f_ee` |
| **`E_max[i]`** | maximum energy for task `i` over all processors, at `f_max` |
| **`E_min_g` / `E_max_g`** | the sums `Σ_i E_min[i]` / `Σ_i E_max[i]` |
| **`E_given`** | the global energy budget, `(1 + PARAM)·E_min_g` (⚠ `ESECC` uses `PARAM·E_min_g`) |
| **`E_pre[i]`** | the per-task energy cap derived from the global budget by the algorithm's allocation rule |
| **`Eslack`** | `E_given − E_min_g`, the energy available to distribute above the minimum |
| **`FED`** | `EHEFT`'s "favourite-energy-duration": average energy on the task's top-2 favourite processors |
| **`W(t)`** | `EHEFT`'s weight: `FED(t)·(1 + 1/(√LN(t)+√LN(entry)))` |
| **`LN`** | DAG level number (longest path depth from a source) |
| **`oct` / `ioct` / `pcg` / `pcm`** | "optimal computation time" variants used as Makespan priorities |
| **Format D1 / D2 / D3** | the three file layouts defined in §5 |
| **`PROC` / `PRO`** | the processor column of the schedule table (1-based in the output) |
