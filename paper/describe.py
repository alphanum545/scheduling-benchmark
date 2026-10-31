#!/usr/bin/env python3
"""Per-family descriptive statistics of the instances (Table 3).
  python3 paper/describe.py <data_index.csv> <data_dir> <evaluate_binary> <out.json>"""
import csv, json, os, subprocess, sys, tempfile, collections
import numpy as np

index, data, tool, out_path = sys.argv[1:5]
out = collections.defaultdict(list)
with tempfile.TemporaryDirectory() as tmp:
    d3 = os.path.join(tmp, 'd.d3')
    for r in csv.DictReader(open(index)):
        subprocess.run([tool, 'strip', os.path.join(data, r['path']), d3], capture_output=True, check=True)
        t = np.fromfile(d3, sep=' ', dtype=np.int64); n, p = int(t[0]), int(t[1])
        W = t[2:2 + n * p].reshape(n, p).astype(float); C = t[2 + n * p:2 + n * p + n * n].reshape(n, n)
        e = C[C != -1]
        ccr = (e.mean() if e.size else 0) / W.mean()
        het = np.mean(W.std(1) / np.maximum(W.mean(1), 1e-9))
        out[r['family']].append(dict(n=n, p=p, e=int(e.size), ccr=float(ccr), het=float(het)))
res = {}
for f, v in out.items():
    c = [x['ccr'] for x in v]
    res[f] = dict(instances=len(v), n=[min(x['n'] for x in v), max(x['n'] for x in v)],
                  p=sorted({x['p'] for x in v}), edges_per_task=float(np.mean([x['e'] / x['n'] for x in v])),
                  ccr_median=float(np.median(c)), ccr_iqr=[float(np.percentile(c, 25)), float(np.percentile(c, 75))],
                  het_mean=float(np.mean([x['het'] for x in v])))
json.dump(res, open(out_path, 'w'), indent=1)
print(f'described {sum(len(v) for v in out.values())} instances in {len(res)} families')
