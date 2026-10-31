#!/usr/bin/env python3
"""Construction time of DSLA configurations (Section 8.5; Fig. 4).
  python3 paper/time.py <dsla_var_binary> <instance_dir> <out.json>
<instance_dir> holds gen1/ (v = 250..3000, p = 16, seed 7) and gen2/ (v = 1000, p = 4,8,32,64, seed 8)
as stripped .d3 files; see reproduce.sh. DSLA_TIME=1 makes dsla_var report its internal time
(from the end of input parsing to the start of output) on stderr. Median of five runs.
Timings are hardware dependent and are not part of the exact-match verification."""
import glob, json, os, re, statistics, subprocess, sys

exe, root, out_path = sys.argv[1:4]
cfg = {'HEFT-equivalent (lambda=0, rank)': ['lr=0', 'mode=0'], 'SLAS (rank build)': ['mode=0'],
       'EFT build': ['mode=1'], 'DSLA': [], 'DSLA, all successors': ['R=0']}
env = dict(os.environ, DSLA_TIME='1')
rows = []
for f in sorted(glob.glob(os.path.join(root, 'gen*', '**', '*.d3'), recursive=True)):
    rel = os.path.relpath(f, root)
    v, p, i = map(int, re.search(r'/(\d+)Nodes/(\d+)PROCESSORS/(\d+)_', '/' + rel).groups())
    for name, args in cfg.items():
        ts = []
        for _ in range(5):
            r = subprocess.run([exe, *args], stdin=open(f), capture_output=True, env=env, check=True)
            ts.append(float(r.stderr.decode().strip()))
        rows.append(dict(set='v' if rel.startswith('gen1') else 'p', v=v, p=p, inst=i, config=name, sec=statistics.median(ts)))
json.dump(rows, open(out_path, 'w'), indent=0)
print(f'timed {len(rows)} configuration-instance pairs')
