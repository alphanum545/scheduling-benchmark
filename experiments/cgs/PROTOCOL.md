# CGS frozen protocol (written before any test-split run)

Frozen source: `experiments/cgs/cgs.cpp`, SHA-256 `2f6db80db6cad2177ee1eca09ce47e03f18f673e47feca8d4d80f71fc95ad665`.
Frozen configuration: defaults (`lam=0.5`, `credit=1`, gate on, parents-only commit, contention-table tail, parents=2, shortlist=2).

All design choices (HEFT rank, insertion, lam = 0.5, gate, credit = 1, parents-only commit, tail) were selected on the
712-instance development split only (`split_of` in `tools/bench.py`). The test split (2102 instances) is run **once**
with the frozen source; nothing is changed after seeing it.

Test-split runs:
- candidate: `cgs`
- ablations: `cgs single` (no closures), `cgs credit=0` (plain rank gate), `cgs nogate`
- insertion comparators (fair to CGS): `heft_ins` (repository HEFT with insertion; append mode reproduces repository
  HEFT on 711/712 dev instances), `scct_x ins` (SCCT with insertion)
- repository baselines (non-insertion, unchanged): heft, peft, aeft, oppts, left, scct, dsla

Primary outcome: mean paired relative makespan gain of `cgs` vs `heft_ins`, and vs `dsla`, with Wilcoxon signed-rank
tests (two-sided). Secondary: per-family and per-CCR gains; ablation gains; runtime.
Every schedule is checked with `tools/evaluate.cpp check`; invalid schedules are reported, not averaged.
