# Public DSLA WfInstances execution through the existing Benchmark workflow

Date: 2026-10-24. **Repository:** alphanum545/scheduling-benchmark (public). **Branch:** research/dsla-wfv15-existing-benchmark-20261024.

## Why this adapter exists

The GitHub connector can dispatch the existing .github/workflows/benchmark.yml workflow but lacks effective authorization to create/update .github/workflows/dsla-wfinstances.yml. Reuse the *existing registered public 20-job Benchmark workflow* on this separate research branch. **No workflow YAML was edited.** GitHub Actions remains on standard public ubuntu-latest runners, max-parallel 20; 4 vCPUs per Linux runner. The production main, its tools, original algorithm code, historical result store, and original workflows are unchanged.

The three original tool implementations were moved byte-identically to tools/bench_original.py (original Git blob 95596f4e0dc56f05573199b4e0b93829da7146a6), tools/fetch_data_original.py (b7200cd86c43c7b671adabcf7c0b35fff3b6d738) and tools/report_original.py (0b5876ca4c34dd3ac43fe578034844950b66595d). Branch-local tools/bench.py, tools/fetch_data.py and tools/report.py delegate all modes other than explicit dsla_wfv15 to their original versions. Adapter source is code/bridge.py.gz (SHA256 6111a14a8582e65971e36736347c734c5da850326be5dff9d10054da6b03fe80, decompressed Python SHA256 1a63d0d1f7fe47e709888366e42eb6cdf76b26866fe62032e58ba8615f467883).

## Exact dispatch (manual only)

The registered workflow is **Benchmark**, identified by .github/workflows/benchmark.yml. Run it only on this study branch, with explicit input fields:

```
algos:       dsla_wfv15
highlight:   dsla
families:    all
datasets:    all
nodes:       all
procs:       all
split:       all
shards:      20
report_name: dsla_wfv15_external_20261024
force:       false
```

The adapter writes an exact 20-entry matrix (five predeclared WfInstances Pegasus families x four lexicographic deterministic shards), verifies the frozen original DSLA source SHA256 5be7070a3a0428e37babab2c7a932b455eb504c753ce4549c9bc47cccb448799 and comparison engine Git blob 89e0badbd0577bbeabfb45fe1a923bcabb224718, and reconstructs immutable precommitted code from the existing SHA-checked four-part public archive. No private-repository data or token is needed.

Each worker downloads only public WfInstances v1.5 Pegasus DAGs, applies the unchanged modeling and comparator methods, checks original scheduler/engine equality and feasibility, and preserves schedules, eligibility records and failures in a SHA256-verified base64 tar.gz envelope within its shard-N.csv artifact. Although the artifact filename ends in .csv to fit the existing workflow, the payload is compressed structured evidence, **not a row-level scheduling CSV**. The report stage refuses missing, duplicated or conflicting shards; reconstructs all five family folders and runs the unchanged source-frozen grouped analysis. Outputs are written under results/reports/dsla_wfv15_external_20261024 on this branch only.

**Scientific caveats:** observed application DAGs and task runtimes are from WfInstances, but all heterogeneous processor/network costs and makespans are modeled; baseline author-code fidelity is not certified. Existing data and benchmark results were not reprocessed. Zero-spanning confidence intervals and negative gains are retained. This is not a real cloud-performance experiment.

## Engineering-only verification before empirical execution

The branch adapter was tested locally using artificial one-task family traces, 20 independently wrapped shards, 1,400 mocked schedule rows, a six-comparison family-stratified analysis whose expected effect was zero, and a missing-shard rejection. The test did not use real WfInstances data and is not empirical performance evidence.

The existing Benchmark workflow's own plan, run and report commands are unchanged. Do NOT merge these branch-specific wrapper scripts into benchmark main; this study should run on the dedicated branch only. If the run fails, preserve all run logs/artifacts and do not silently omit cases or publish an average gain.

For method and source origin, see PROTOCOL.md, PLAN.json and SOURCE_SHA256SUMS.
