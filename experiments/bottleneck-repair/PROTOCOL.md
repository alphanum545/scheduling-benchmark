# Bounded bottleneck-closure repair: development protocol

Date: 2026-10-26. Public scheduling-benchmark only. No workflow YAML changes; retain the existing twenty-shard public Benchmark workflow. No changes to main or private paper repositories.

## Question
Can exact repair of actual processor assignments outperform static sibling forecasts and the current DSLA seed, without accepting a worse makespan? This is an experimental schedule-improvement method, not yet an established novel algorithm.

## Frozen candidate
Source: algorithms/makespan/bcr.cpp. Local SHA-256 before commit: e1fb275b3131143d9c37eaac0bb61f081f7e36731263ac7363063134dc8f8281.
Keep the seed topological order. First perform single-task repairs; then consider critical-parent prefixes and backward-chain groups of at most four tasks, assigned jointly to a target processor. Select up to twenty-four actual zero-slack tasks per round. Consider four surrogate-ranked target processors plus the current processors of group members. Budgets: at most 4096 evaluated moves, eight rounds per phase. These values are computational limits fixed before inspecting this candidate's development outcomes, not tuned performance weights.

Each candidate rebuilds the changed processor precedence arcs, propagates all affected data/resource finish times in topological order, and accepts only lexicographic improvement in makespan then total finish time. An early rejection is permitted only when an already-finalized task finish exceeds the incumbent best makespan. Incremental and complete replay are checked by a verification mode.

## Configurations
bcr_scct_seed and bcr_dsla_seed are reimplementation conformance controls. bcr_scct applies full repair to SCCT. bcr_dsla_single uses only single-task moves under the same maximum trial/round budget. bcr_dsla adds bounded joint groups. bcr_dsla_strict disallows equal-makespan moves. Original scct, dsla, heft, left, oppts, aeft, peft remain unchanged. Do not claim seed equivalence until the controls are checked on the compared instances.

## Execution
Development split only, all families, processor counts, sizes and formats. Run all named candidates and unchanged comparators with force=true so runtime measurements come from the same public run rather than mixed historical environments. The test split has already been examined for earlier SCCT designs, so it is not a certified pristine holdout; it will not be evaluated here or used for tuning. Fresh grouped external validation is required before a publication claim.

## Analysis
Check full validity and complete coverage before any superiority claim. Compare paired relative makespan gains on identical paths, report win/tie/loss, mean and median, per-family regressions, SLR, scheduler runtime including seed construction, and tail losses. Report ratio of average makespans separately from average paired percentages. Use one aggregate per named DAG as a dependence sensitivity analysis, noting that named grouping needs structural verification. Holm-correct pairwise tests. Compare full vs single and strict variants; improvement from additional computation is not in itself novelty. Both variants have the same maximum evaluation budget, not an automatically equal realized wall-clock time.

## Correctness evidence before benchmarking
Local engineering tests (not performance evidence) in test_bcr.cpp: a synthetic three-task fan-in barrier has makespan 12 in its specified feasible incumbent; no single relocation improves the lexicographic objective, while joint relocation gives 5. Randomized tiny-DAG tests compare incremental with complete replay and check non-worsening repair, arbitrary task labels, zero costs and disconnected graphs. Address/undefined behavior sanitizer run passed: 100 generated graphs, 6000 trial calls, 200 seed replay checks. See UNIT_TESTS.txt.

## Prior art and claim boundary
Critical-path local search and graph-informed neighborhoods already appear in Wong et al., A Tabu Search for the Heterogeneous DAG Scheduling Problem (ICPADS 2009, DOI 10.1109/ICPADS.2009.127); variable-neighborhood DAG scheduling appears in Selvi and Manimegalai (2017, DOI 10.1080/08839514.2017.1300010). Coleman et al. (arXiv:2403.07112) caution through component comparisons that recombining list-scheduling components is not enough to establish contribution. An exhaustive novelty/fidelity audit and matched modern improvement baselines are outstanding. No claim of being the first local-search or critical-path repair algorithm.

## Decision rule
Continue only if a candidate gives a reproducible practically meaningful gain, passes validity/seed checks, and has a defensible runtime tradeoff. Report all losses. A negative component ablation is evidence against that proposed contribution. Do not discard instances or silently overwrite failed experiments.
