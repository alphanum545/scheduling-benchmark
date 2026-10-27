#!/usr/bin/env python3
"""analyze_ext.py - the analysis fixed in PROTOCOL.md.  usage: analyze_ext.py RESULTS.csv [REF=cgs]"""
import csv, sys, collections
from scipy.stats import wilcoxon

rows = list(csv.DictReader(open(sys.argv[1])))
ref = sys.argv[2] if len(sys.argv) > 2 else "cgs"
bad = [r for r in rows if r["valid"] != "1"]
M = collections.defaultdict(dict); meta = {}
for r in rows:
    if r["valid"] != "1": continue
    k = (r["workflow"], r["p"], r["ccr"], r["beta"])
    M[r["algo"]][k] = float(r["makespan"]); meta[k] = r["family"]
algos = [a for a in M if a != ref]
inst = sorted(M[ref])
print(f"instances {len(inst)}, workflows {len({k[0] for k in inst})}, families {len(set(meta.values()))}, "
      f"invalid rows {len(bad)}")

def gain(b, k): return 100 * (M[b][k] - M[ref][k]) / M[b][k] if M[b][k] > 0 else 0.0

res = []
for b in algos:
    ks = [k for k in inst if k in M[b]]
    g = [gain(b, k) for k in ks]
    w = sum(M[ref][k] < M[b][k] for k in ks); l = sum(M[ref][k] > M[b][k] for k in ks)
    per = collections.defaultdict(list)
    for k, x in zip(ks, g): per[k[0]].append(x)
    pw = [sum(v) / len(v) for v in per.values()]
    fam = collections.defaultdict(list)
    for k, x in zip(ks, g): fam[meta[k]].append(x)
    macro = sum(sum(v) / len(v) for v in fam.values()) / len(fam)
    p = wilcoxon(pw).pvalue if any(abs(x) > 1e-12 for x in pw) else 1.0
    res.append([b, len(ks), sum(g) / len(g), macro, w, len(ks) - w - l, l, p])
# Holm across baselines
order = sorted(range(len(res)), key=lambda i: res[i][7]); m = len(res); run = 0
for rank, i in enumerate(order):
    run = max(run, min(1.0, (m - rank) * res[i][7])); res[i].append(run)
res.sort(key=lambda r: r[2])
print(f"\nMean paired gain of {ref} (positive = {ref} shorter), per-workflow Wilcoxon, Holm-adjusted")
print(f"{'baseline':14s} {'n':>5s} {'gain%':>7s} {'macro%':>7s} {'W/T/L':>16s} {'p':>9s} {'Holm p':>9s}")
for b, n, g, mac, w, t, l, p, hp in res:
    print(f"{b:14s} {n:5d} {g:7.2f} {mac:7.2f} {f'{w}/{t}/{l}':>16s} {p:9.1e} {hp:9.1e}")

def table(keyf, title, cols):
    print(f"\n{title}")
    G = collections.defaultdict(lambda: collections.defaultdict(list))
    for b in cols:
        for k in inst:
            if k in M[b]: G[keyf(k)][b].append(gain(b, k))
    print(f"{'':28s} {'n':>5s} " + " ".join(f"{b[:10]:>10s}" for b in cols))
    for key in sorted(G):
        n = len(G[key][cols[0]])
        print(f"{str(key)[:28]:28s} {n:5d} " + " ".join(f"{sum(G[key][b]) / len(G[key][b]):10.2f}" for b in cols))

main = [b for b in ["aeft_ins", "heft_ins", "peft_ins", "la_heft", "scct_ins", "bppts_ins", "ippts_ins", "cgs_single", "cgs_credit0"] if b in M]
table(lambda k: meta[k], f"Gain % of {ref} by family", main)
table(lambda k: f"CCR {k[2]}", f"Gain % of {ref} by CCR", main)
table(lambda k: f"P {k[1]}", f"Gain % of {ref} by processors", main)
table(lambda k: f"beta {k[3]}", f"Gain % of {ref} by heterogeneity", main)
rt = collections.defaultdict(list)
for r in rows: rt[r["algo"]].append(float(r["runtime"]))
print("\nMean runtime s (2-core sandbox, two workers; not citable): " +
      ", ".join(f"{a} {sum(v) / len(v):.3f}" for a, v in sorted(rt.items())))
