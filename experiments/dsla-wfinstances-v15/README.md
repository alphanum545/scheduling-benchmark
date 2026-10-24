# DSLA WfInstances v1.5 external validation — PUBLIC experiment setup

**Status: setup migrated; workflow NOT DISPATCHED.** This directory belongs to the **public** alphanum545/scheduling-benchmark repository. The private research-paper repository is NOT checked out, required, or used for benchmark execution.

## Frozen research contract

- Original, precommitted, outcome-blind protocol: [PROTOCOL.md](PROTOCOL.md), [PLAN.json](PLAN.json). These bytes are preserved exactly from before the five-family experiment; the external experiment has NOT been executed.
- Five Pegasus WfInstances v1.5 families: 1000genome, cycles, seismology, soykb, srasearch. Include every objectively eligible trace. The DAG and task runtime are real application observations, but the modeled heterogeneous processor and transfer costs are NOT observations of cloud scheduling.
- Original DSLA source pinned to this public repository at commit 2f078d86b9d39835756f6ed9613279418a52371b, algorithms/makespan/dsla.cpp, SHA-256 5be7070a3a0428e37babab2c7a932b455eb504c753ce4549c9bc47cccb448799.
- The source-pinned independent comparison engine has been copied locally into [code/engine.cpp](code/engine.cpp); Git blob SHA-1 89e0badbd0577bbeabfb45fe1a923bcabb224718. There are **no private-repository runtime dependencies**.
- The two precommitted Python scripts retain exact SHA-256 identities listed in [SOURCE_SHA256SUMS](SOURCE_SHA256SUMS). The new sharding wrapper changes ONLY deterministic work distribution and complete-family reconstruction, not eligibility, algorithm scoring, dataset selection, or the precommitted statistics.
- HEFT/PEFT comparisons are explicitly identified insertion-based reimplementations; no claim of author-certified comparator fidelity.

## Frozen twenty-way execution layout

The manual-only workflow template is the compressed file [workflow/dsla-wfinstances.yml.gz](workflow/dsla-wfinstances.yml.gz). To review: gzip -dc workflow/dsla-wfinstances.yml.gz. It has **only workflow_dispatch** (no push/pull request/schedule trigger), ubuntu-latest, 5 families x 4 fixed shards, and max-parallel 20. Each standard public ubuntu-latest runner has four vCPUs, as documented by GitHub. Concurrency group bench-store is shared with the current public benchmark workflow.

The complete Python evaluation, statistics, tests and baseline-fidelity audit are archived in four exact-byte pieces (source_bundle.part00 through source_bundle.part03). Reconstruct and test with:

```sh
cd experiments/dsla-wfinstances-v15
sha256sum -c SOURCE_PARTS_SHA256SUMS
cat source_bundle.part0{0,1,2,3} > source_bundle.tar.gz
sha256sum -c SOURCE_BUNDLE_SHA256SUMS
tar -xzf source_bundle.tar.gz
sha256sum -c SOURCE_SHA256SUMS
python3 tests/test_pipeline.py
python3 tests/test_sharding.py
```

Unit tests use artificial one-node DAGs and mocked schedules: **no real WfInstances performance measurements or superiority conclusions**. Full worker source and complete schedules will later be saved as artifacts of the public benchmark Actions workflow, with aggregate outputs only on its dedicated study branch. Neither the old benchmark workflows, production algorithms nor historical result store are modified.

**IMPORTANT: Do NOT run yet.** The user's existing public runners are occupied. Setup registration and any future explicit dispatch are separate actions. See [docs/RUNBOOK.md](docs/RUNBOOK.md).
