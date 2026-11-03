#!/bin/sh
# reproduce.sh: regenerate every result of the CGS evaluation from this commit.
#
#   ./reproduce.sh                 everything on this machine (WORKERS=<n> to set parallelism; default: all cores)
#   ./reproduce.sh shard <i> <n>   CI: the per-instance work of shard i of n -> .work/shards/*-<i>.*
#   ./reproduce.sh finish <n>      CI: merge the n shards' files from .work/shards/, then statistics, tables,
#                                  figures, worked examples, timing and verification
#
# Every per-instance step (schedules, fidelity check, instance statistics, the two analysis probes) is split
# by the same deterministic assignment (tools/bench.py plan for the schedules, tools/shards.py for the rest),
# and merging restores the original order, so the sharded outputs equal the single-machine ones.
# Writes outputs/ and finally runs paper/verify.py (exit 1 if anything differs from paper/expected).
set -eu
cd "$(dirname "$0")"
MODE=${1:-all}
OUT=outputs
WORKERS=${WORKERS:-$(nproc)}
BIN=.work/bin
S=.work/shards
case "$MODE" in
  all)    ;;
  shard)  I=${2:?shard index}; N=${3:?number of shards} ;;
  finish) N=${2:?number of shards} ;;
  *)      echo "usage: $0 [all | shard <i> <n> | finish <n>]" >&2; exit 2 ;;
esac

# ---- common setup: data, index, binaries
case "$MODE" in
  all)    rm -rf "$OUT" .work data_index.csv ;;
  shard)  rm -rf .work data_index.csv ;;                                     # writes no outputs/
  finish) rm -rf "$OUT" .work/bin .work/timing .work/plan.json data_index.csv ;;   # keep .work/shards
esac
mkdir -p "$BIN" "$S"
[ "$MODE" = shard ] || mkdir -p "$OUT/tables" "$OUT/figures" "$OUT/timing"

echo "== 1/11 data: verify archives, unpack workflows, regenerate random DAGs"
python3 tools/prepare_data.py

echo "== 2/11 index (2814 instances; six empty Montage files are excluded by the harness)"
python3 tools/bench.py index --data data

g++ -O2 -std=c++17 -w -o "$BIN/evaluate" tools/evaluate.cpp
g++ -O2 -std=c++17 -w -o "$BIN/read_only" tools/read_only.cpp
g++ -O2 -std=c++17 -w -o "$BIN/cgs_probe" tools/cgs_probe.cpp
for a in cgs heft_ins peft_ins ippts_ins aeft_ins pmms_ins bppts_ins la_heft; do
  g++ -O2 -std=c++17 -w -o "$BIN/$a" "algorithms/insertion/$a.cpp"; done
for a in heft peft ippts aeft pmms bppts; do g++ -O2 -std=c++17 -w -o "$BIN/$a" "algorithms/makespan/$a.cpp"; done

# ---- per-instance work: all of it, one shard of it, or merge the shards
shard_args() {  # $1 = file stem; prints the trailing arguments for the current mode
  case "$MODE" in
    all)    ;;
    shard)  echo "--shard $I $N $S/$1-$I.json" ;;
    finish) echo "--merge $S/$1-*.json" ;;
  esac
}

echo "== 3/11 schedules: 28 algorithm configurations x 2814 instances, every schedule validated"
case "$MODE" in
  all)
    python3 tools/bench.py plan --algos all --force --out .work/plan.json
    python3 tools/bench.py run --data data --plan .work/plan.json --workers "$WORKERS" --out "$OUT/runs.csv" ;;
  shard)
    python3 tools/bench.py plan --algos all --force --shards "$N" --out .work/plan.json
    python3 tools/bench.py run --data data --plan .work/plan.json --shard "$I" --workers "$WORKERS" --out "$S/runs-$I.csv" ;;
  finish)
    python3 - "$N" "$S" "$OUT/runs.csv" <<'EOF'
import csv, os, sys
n, d, out = int(sys.argv[1]), sys.argv[2], sys.argv[3]
files = [os.path.join(d, f'runs-{i}.csv') for i in range(n)]
missing = [f for f in files if not os.path.exists(f)]
if missing:
    sys.exit(f'missing shard result files: {missing}')
header, rows = None, []
for f in files:
    with open(f, newline='') as fh:
        r = csv.reader(fh); h = next(r)
        if header is None: header = h
        elif h != header: sys.exit(f'{f}: header differs')
        rows += list(r)
with open(out, 'w', newline='') as fh:
    w = csv.writer(fh); w.writerow(header); w.writerows(rows)
print(f'merged {len(rows)} runs from {n} shards')
EOF
    ;;
esac

echo "== 4/11 fidelity: each insertion baseline in append mode equals the repository original"
python3 tools/check_append.py data "$BIN" "$BIN/evaluate" "$OUT/append_check.json" "$WORKERS" $(shard_args append)

echo "== 5/11 instance statistics"
python3 paper/describe.py data_index.csv data "$BIN/evaluate" "$OUT/describe.json" $(shard_args describe)

if [ "$MODE" = shard ]; then
  echo "== 7/11 random-DAG probe (this shard)"
  python3 paper/why_random.py data_index.csv data "$BIN" - - $(shard_args why_random)
  echo "== 8/11 contention re-runs (this shard)"
  python3 paper/why_contention.py data_index.csv data "$BIN" - - $(shard_args why_contention)
  echo "shard $I of $N done"
  exit 0
fi

echo "== 6/11 statistics, tables and result figures"
(cd "$OUT" && python3 ../paper/analysis.py runs.csv && python3 ../paper/make_tables.py tables \
  && python3 ../paper/make_figs_results.py runs.csv figures)

echo "== 7/11 why CGS gains less on random DAGs (structure, closure probe, headroom; test split)"
python3 paper/why_random.py data_index.csv data "$BIN" "$OUT/runs.csv" "$OUT" $(shard_args why_random)

echo "== 8/11 what the contention replay measures (port loads of re-run schedules; test split)"
python3 paper/why_contention.py data_index.csv data "$BIN" "$OUT/runs.csv" "$OUT" $(shard_args why_contention)

echo "== 9/11 worked examples"
python3 paper/worked_example.py "$BIN" "$BIN/evaluate" "$OUT/example" paper/examples
cp "$OUT"/example/tab_ex_*.tex "$OUT/tables/"; cp "$OUT"/example/fig_ex_gantt.pdf "$OUT/figures/"

echo "== 10/11 scheduling time (hardware dependent)"
T=.work/timing
python3 tools/generate.py --out "$T/gen1" --nodes 250,500,1000,2000,3000 --procs 16 --ccr 1 --beta 0.5 --count 2 --seed 7 --prefix S- --format D2
python3 tools/generate.py --out "$T/gen2" --nodes 1000 --procs 4,8,32,64 --ccr 1 --beta 0.5 --count 2 --seed 8 --prefix S- --format D2
find "$T" -name '*.txt' | while read -r f; do "$BIN/evaluate" strip "$f" "${f%.txt}.d3" >/dev/null; done
python3 paper/time.py "$BIN" "$T" "$OUT/timing/timing.json"
(cd "$OUT" && python3 ../paper/make_figs.py figures)

echo "== 11/11 verification against paper/expected"
status=0
python3 paper/verify.py "$OUT" > "$OUT/VERIFY.txt" || status=$?
cat "$OUT/VERIFY.txt"
exit "$status"
