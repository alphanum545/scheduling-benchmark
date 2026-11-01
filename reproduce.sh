#!/bin/sh
# reproduce.sh: regenerate every result of the DSLA manuscript from this commit.
#   ./reproduce.sh            (WORKERS=<n> to set parallelism; default: all cores)
# Writes outputs/ and finally runs paper/verify.py (exit 1 if anything differs from the paper).
set -eu
cd "$(dirname "$0")"
OUT=outputs
WORKERS=${WORKERS:-$(nproc)}
BIN=.work/bin
rm -rf "$OUT" .work data_index.csv
mkdir -p "$OUT/tables" "$OUT/figures" "$OUT/timing" "$BIN"

echo "== 1/8 data: verify archives, unpack workflows, regenerate random DAGs"
python3 tools/prepare_data.py

echo "== 2/8 index (2814 instances; six empty Montage files are excluded by the harness)"
python3 tools/bench.py index --data data

echo "== 3/8 schedules: 24 algorithms x 2814 instances, every schedule validated"
python3 tools/bench.py plan --algos all --force --out .work/plan.json
python3 tools/bench.py run --data data --plan .work/plan.json --workers "$WORKERS" --out "$OUT/runs.csv"

g++ -O2 -std=c++17 -w -o "$BIN/evaluate" tools/evaluate.cpp
g++ -O2 -std=c++17 -w -o "$BIN/dsla_var" algorithms/makespan/dsla_var.cpp

echo "== 4/8 instance statistics (Table 3)"
python3 paper/describe.py data_index.csv data "$BIN/evaluate" "$OUT/describe.json"

echo "== 5/8 statistics, tables and result figures"
(cd "$OUT" && python3 ../paper/analysis.py runs.csv && python3 ../paper/make_tables.py tables \
  && python3 ../paper/make_figs_results.py runs.csv figures)

echo "== 6/8 worked example (Section 6)"
EX=.work/exbin; mkdir -p "$EX"
for a in heft peft ippts aeft pmms bppts dsla; do g++ -O2 -std=c++17 -w -o "$EX/$a" "algorithms/makespan/$a.cpp"; done
python3 paper/worked_example.py paper/examples/example_classic_adapted.d3 "$EX" "$BIN/evaluate" "$OUT/example" paper/examples/classic_topcuoglu.d3
cp "$OUT"/example/tab_ex_*.tex "$OUT/tables/"; cp "$OUT"/example/fig_ex_gantt.pdf "$OUT/figures/"

echo "== 7/8 construction time (Section 8.8; hardware dependent)"
T=.work/timing
python3 tools/generate.py --out "$T/gen1" --nodes 250,500,1000,2000,3000 --procs 16 --ccr 1 --beta 0.5 --count 2 --seed 7 --prefix S- --format D2
python3 tools/generate.py --out "$T/gen2" --nodes 1000 --procs 4,8,32,64 --ccr 1 --beta 0.5 --count 2 --seed 8 --prefix S- --format D2
find "$T" -name '*.txt' | while read -r f; do "$BIN/evaluate" strip "$f" "${f%.txt}.d3" >/dev/null; done
python3 paper/time.py "$BIN/dsla_var" "$T" "$OUT/timing/timing.json"
(cd "$OUT" && python3 ../paper/make_figs.py figures)

echo "== 8/8 verification against the manuscript"
status=0
python3 paper/verify.py "$OUT" > "$OUT/VERIFY.txt" || status=$?
cat "$OUT/VERIFY.txt"
exit "$status"
