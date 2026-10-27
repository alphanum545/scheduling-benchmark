# RCIS development experiment

Independent Ready-Closure Insertion Scheduling, built from an empty schedule without invoking DSLA, SCCT or BCR. Critical-path priority uses minimum computation costs. Eligible ready parent-join groups are jointly assigned with gap insertion and a deterministic local surrogate. The algorithm does not guarantee superiority over independent single-task placement.

Experimental parameters frozen before development-corpus evaluation: at most two uncommitted ready parents per closure, a shortlist of two processors per parent plus common-target inclusion. Comparator ablations: `--single`, `--append`, `--no-common-target`. Report code and all regressions; do not use test split for selection. Original full registry preserved as `algorithms_original.toml`.

This source has not been shown to be novel over published heterogeneous DAG lookahead and joint-task scheduling. Do not present small engineering examples or earlier BCR results as corpus performance evidence.
