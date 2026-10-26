# SCCT joint-neighborhood pilot (public benchmark)
Status: Experimental candidate, **not** a verified performance improvement.

Source: algorithms/makespan/scct_joint.cpp (original unchanged).
Frozen baseline: original SCCT on main commit 3741b6d4a61a9599284f78c23a826658b81f7500.
Four experimental labels use one source, with command-line flags:

- scct_joint_base: no flags; exact greedy-SCCT control intended to reproduce original makespans.
- scct_joint: bounded eight-state sibling beam search, with original greedy plan always included.
- scct_diamond: original greedy sibling plan, but isolated-diamond join predicted once.
- scct_joint_diamond: both.

A diamond is eligible only if the parent has >=2 children, every child has exactly the
parent as its only predecessor and the same join as its only successor, and the join
has exactly those children as predecessors. All other nodes use the generic model.

All experiments preserve SCCT priority, mu=2, scheduling and tie breakers.
Scores are local predictions, not rigorous global lower bounds. The joint and
diamond estimators can be smaller without producing a smaller final makespan.
Reference source remains on main. Variant labels are dynamically appended to
algorithms.toml at runtime by register.py, from the branch-specific Benchmark
workflow. No activation or modification on main.

Protocol: run development split first (712 instances). Check validity, control
equivalence, paired family-wise gains/losses, runtime; freeze settings, then
run held-out test split only if sufficiently justified. Compare with SCCT,
DSLA, OPPTS, HEFT and LEFT, and record weaknesses at CCR=10.
Not claim publication novelty absent prior-art review.
