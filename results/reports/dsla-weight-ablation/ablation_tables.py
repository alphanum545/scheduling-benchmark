#!/usr/bin/env python3
"""Per-family and per-split tables for the DSLA score-weight ablation (dsla vs dsla_fix, slas vs slas_fix).
gain = (variant - original) / variant * 100; positive = the original (with the lambda*w(t,k) term) is shorter.
Reads results/store.csv at the current algorithm hashes.  Usage: python3 results/reports/dsla-weight-ablation/ablation_tables.py"""
import csv, os, sys, statistics as st
from collections import defaultdict
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
sys.path.insert(0, os.path.join(ROOT, "tools")); import bench
H = {a: v["hash"] for a, v in bench.load_algos().items()}
R = defaultdict(dict)
for r in csv.DictReader(open(os.path.join(ROOT, "results", "store.csv"))):
    if r["algo"] in ("dsla", "dsla_fix", "slas", "slas_fix") and r["algo_hash"] == H[r["algo"]] and r["status"] == "ok":
        R[r["path"]][r["algo"]] = float(r["makespan"])
def row(label, orig, var, keep):
    g = [(rs[var] - rs[orig]) / rs[var] * 100 for p, rs in R.items() if orig in rs and var in rs and keep(p)]
    n = len(g); w = sum(x > 1e-9 for x in g); l = sum(x < -1e-9 for x in g)
    return f"| {label} | {n} | {w/n*100:.1f} | {(n-w-l)/n*100:.1f} | {l/n*100:.1f} | {st.mean(g):+.3f} |"
fam = lambda p: p.split("/")[0]
hdr = "| group | n | original shorter % | equal % | variant shorter % | mean gain % |\n|:--|--:|--:|--:|--:|--:|"
for o, v in (("dsla", "dsla_fix"), ("slas", "slas_fix")):
    print(f"\n### {o} vs {v}\n\n{hdr}")
    print(row("all", o, v, lambda p: True))
    for sp in ("dev", "test"): print(row(f"split {sp}", o, v, lambda p, sp=sp: bench.split_of(p) == sp))
    for F in sorted({fam(p) for p in R}): print(row(F, o, v, lambda p, F=F: fam(p) == F))
