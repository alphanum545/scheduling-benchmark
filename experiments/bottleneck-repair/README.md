# Bounded bottleneck-closure repair (BCR)

Experimental development study, not a submission-ready novelty claim.

- `PROTOCOL.md`: pre-execution questions, frozen budgets, controls and claim boundaries.
- `../../algorithms/makespan/bcr.cpp`: standalone candidate plus SCCT/DSLA seed controls.
- `test_bcr.cpp`: reproducible minimal fan-in barrier and randomized replay-conformance checks.
- `UNIT_TESTS.txt`: local sanitizer validation record; not benchmark performance evidence.
- Expected benchmark output: `../../results/reports/bcr-dev-v1/` on this branch only.

Compile correctness tests from repository root:

```sh
g++ -O1 -g -std=c++17 -fsanitize=address,undefined -fno-omit-frame-pointer experiments/bottleneck-repair/test_bcr.cpp -o /tmp/test-bcr
/tmp/test-bcr
```

The locally tested C++ file is byte-identical to the committed Git blob `b53cb158b69baacada2eafca44fb06c34240d46a` (16026 bytes; SHA-256 in the protocol). Baseline source files and manual Benchmark workflow are unchanged. No private repository is used, and main is not updated. Performance results and per-family analyses will be recorded here only after execution; no gain is assumed.
