# Scaling of BLA-HEFT

Figure: `bla_heft_scaling.pdf` (rebuild with `python3 paper/make_figures.py`).

Random layered DAGs from `tools/generate.py` (CCR 1, beta 0.5, 2 instances per point, seeds 7 and 8),
timed on a 2-core cloud container, median of 5 runs.

Instances: `python3 tools/generate.py --out <dir> --nodes 250,500,1000,2000,3000 --procs 16 --ccr 1 --beta 0.5
--count 2 --seed 7 --prefix S- --format D2`, and the same with `--nodes 1000 --procs 4,8,32,64 --seed 8`.

`bla_heft_internal_time.csv` is the scheduling time of BLA-HEFT alone, measured with a copy of
`bla_heft.cpp` that prints the time from the end of input parsing to the start of output
(set 1: p = 16; set 2: v = 1000). Log-log slopes: 1.23 against v at p = 16 (250 to 3000 tasks) and 1.12 against p at v = 1000
(4 to 64 processors; 1.5 between 32 and 64), consistent with O(e p + v p^2) on these sparse graphs and
within the O(v^2 p) bound. 3000 tasks on 16 processors take 18 ms; 1000 tasks on 64 processors, 31 ms.

`wall_time_all_algorithms.csv` (seconds, median of 3 runs, mean of the 2 instances; `reader` only parses
the input) is the end-to-end wall time of every algorithm as implemented in the repository (process start, reading the n x n input matrix, scheduling, output). It is dominated by input
parsing, which is O(v^2) for every algorithm, and the implementations read their input differently
(BLA-HEFT and lab2 use unsynchronised streams, most baselines do not). Do not use it to claim BLA-HEFT is
faster than the baselines; a fair runtime comparison needs the same I/O code in every implementation.
