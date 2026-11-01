#!/usr/bin/env python3
"""Scheduling time of CGS and the baselines (Fig. fig_time).
  python3 paper/time.py <bin_dir> <instance_dir> <out.json>
<bin_dir> holds compiled cgs, heft_ins, aeft_ins, peft_ins, la_heft and read_only (tools/read_only.cpp, which
only reads the instance). <instance_dir> holds gen1/ (v = 250..3000, p = 16, seed 7) and gen2/ (v = 1000,
p = 4,8,32,64, seed 8) as stripped .d3 files; see reproduce.sh.
Wall time of one process run (input reading and output included; 'read_only' shows the reading share),
median of five runs. The sources are not modified for timing. Hardware dependent; not verified."""
import glob, json, os, re, statistics, subprocess, sys, time

bindir, root, out_path = sys.argv[1:4]
cfg = {'cgs': ['cgs'], 'cgs_single': ['cgs', 'single'], 'heft_ins': ['heft_ins'], 'aeft_ins': ['aeft_ins'],
       'peft_ins': ['peft_ins'], 'la_heft': ['la_heft'], 'read_only': ['read_only']}
rows = []
for f in sorted(glob.glob(os.path.join(root, 'gen*', '**', '*.d3'), recursive=True)):
    rel = os.path.relpath(f, root)
    v, p, i = map(int, re.search(r'/(\d+)Nodes/(\d+)PROCESSORS/(\d+)_', '/' + rel).groups())
    for name, cmd in cfg.items():
        ts = []
        for _ in range(5):
            t0 = time.perf_counter()
            subprocess.run([os.path.join(bindir, cmd[0]), *cmd[1:]], stdin=open(f), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
            ts.append(time.perf_counter() - t0)
        rows.append(dict(set='v' if rel.startswith('gen1') else 'p', v=v, p=p, inst=i, config=name, sec=statistics.median(ts)))
json.dump(rows, open(out_path, 'w'), indent=0)
print(f'timed {len(rows)} configuration-instance pairs')
