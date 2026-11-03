#!/usr/bin/env python3
"""check_append.py: fidelity of the insertion baselines.

  python3 tools/check_append.py <data_dir> <bin_dir> <evaluate> <out.json> [workers] [--shard i n f | --merge glob]

Each algorithms/insertion/<b>_ins.cpp is a repository baseline with only the start-time rule changed; its
`append` argument restores the original rule. This runs <b>_ins append and the original <b> on every indexed
instance and checks that the two schedules (task, start, finish, processor) are identical.
<bin_dir> holds compiled heft, peft, ippts, aeft, pmms, bppts and their *_ins versions.
--shard / --merge split the instances across CI jobs (tools/shards.py); the merged result is identical."""
import csv, json, os, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import shards
argv, MODE = shards.parse(sys.argv[1:])
data, bindir, tool, out_path = argv[:4]
workers = int(argv[4]) if len(argv) > 4 else os.cpu_count()
BASE = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']
paths = [r['path'] for r in csv.DictReader(open(os.path.join(ROOT, 'data_index.csv')))]


def sched(cmd, d3):
    o = subprocess.run(cmd, stdin=open(d3), capture_output=True, text=True, check=True).stdout
    return sorted(tuple(map(int, l.split())) for l in o.split('\n')
                  if l.strip() and l.split()[0].isdigit() and len(l.split()) == 4)


def one(path):
    with tempfile.TemporaryDirectory() as tmp:
        d3 = os.path.join(tmp, 'x.d3')
        subprocess.run([tool, 'strip', os.path.join(data, path), d3], capture_output=True, check=True)
        return {b: sched([os.path.join(bindir, b)], d3) == sched([os.path.join(bindir, b + '_ins'), 'append'], d3) for b in BASE}


todo = shards.mine(paths, MODE)
with ThreadPoolExecutor(workers) as ex:
    part = list(ex.map(one, todo))
if MODE and MODE[0] == 'shard':
    shards.save(dict(zip(todo, part)), MODE); sys.exit(0)
res = shards.load(paths, MODE) if MODE else part
summary = {b: dict(identical=sum(r[b] for r in res), instances=len(res),
                   differing=[p for p, r in zip(paths, res) if not r[b]][:20]) for b in BASE}
json.dump(summary, open(out_path, 'w'), indent=1)
for b in BASE:
    print(f"{b:6s} append mode identical to the original on {summary[b]['identical']}/{len(res)} instances")
