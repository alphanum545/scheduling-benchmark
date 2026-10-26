#!/usr/bin/env python3
"""Tables for the SCCT report, read from results/store.csv at the current algorithm hashes.
gain(A vs B) = (makespan_B - makespan_A) / makespan_B * 100; positive = A shorter.
Usage: python3 results/reports/scct/scct_tables.py"""
import csv, os, sys, statistics as st
from collections import defaultdict
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
sys.path.insert(0, os.path.join(ROOT, "tools")); import bench
H = {a: v["hash"] for a, v in bench.load_algos().items()}
BASE = ["heft", "peft", "ipeft", "mpeft", "aeft", "ppts", "ippts", "bppts", "oppts", "pmms", "left", "lstd"]
R = defaultdict(dict)
for r in csv.DictReader(open(os.path.join(ROOT, "results", "store.csv"))):
    a = r["algo"]
    if a in H and r["algo_hash"] == H[a] and r["status"] in ("ok", "unchecked"):
        R[r["path"]][a] = float(r["makespan"])
fam = lambda p: p.split("/")[0]
WF = {"FFT", "GE", "Genome", "LA", "LIGO", "Montage"}
def common(algos, split="all"):
    return [p for p in R if (split == "all" or bench.split_of(p) == split) and all(a in R[p] for a in algos)]
def stats(a, b, P):
    d = [(R[p][b] - R[p][a]) / R[p][b] * 100 for p in P]
    n = len(d); w = sum(x > 1e-9 for x in d); l = sum(x < -1e-9 for x in d)
    return n, st.mean(d), st.median(d), w / n * 100, (n - w - l) / n * 100, l / n * 100
def table(title, rows, cols):
    print(f"\n### {title}\n\n| " + " | ".join(cols) + " |\n|" + "|".join([":--"] + ["--:"] * (len(cols) - 1)) + "|")
    for r in rows: print("| " + " | ".join(r) + " |")

# 1. OPPTS repetition ablation
P = common(["oppts", "oppts_ft", "oppts_fs", "oppts_fb"])
table(f"OPPTS with its repetitions removed vs original OPPTS (all, n={len(P)})",
      [[v, f"{s[1]:+.2f}%", f"{s[3]:.1f}%", f"{s[5]:.1f}%"] for v, s in ((v, stats(v, "oppts", P)) for v in ("oppts_ft", "oppts_fs", "oppts_fb"))],
      ["variant", "mean gain vs OPPTS", "shorter", "longer"])

# 2. SCCT vs baselines, test and all
for split in ("test", "all"):
    P = common(["scct"] + BASE, split)
    rows = [[b, f"{s[1]:+.2f}%", f"{s[2]:+.2f}%", f"{s[3]:.1f}%", f"{s[5]:.1f}%"] for b, s in ((b, stats("scct", b, P)) for b in BASE)]
    best = sum(R[p]["scct"] < min(R[p][b] for b in BASE) - 1e-9 for p in P) / len(P) * 100
    table(f"SCCT vs baselines, {split} split (n={len(P)}; SCCT strictly best of all 13 on {best:.1f}%)",
          rows, ["baseline", "mean gain", "median gain", "SCCT shorter", "SCCT longer"])

# 3. per family (all instances)
P = common(["scct"] + BASE)
rows = []
for F in sorted({fam(p) for p in P}, key=lambda f: (f.startswith("R-"), f)):
    Q = [p for p in P if fam(p) == F]
    g = {b: stats("scct", b, Q)[1] for b in BASE}
    hard = min(g, key=g.get)
    rows.append([F, str(len(Q)), f"{g['oppts']:+.2f}%", f"{g['heft']:+.2f}%", f"{sum(v > 0 for v in g.values())}/12", f"{hard} {g[hard]:+.2f}%"])
table("SCCT by family (all instances)", rows, ["family", "n", "vs OPPTS", "vs HEFT", "baselines beaten", "strongest baseline"])

# 4. component ablations (all instances)
abl = [("scct_nocont", "no sibling contention in the selection table"),
       ("scct_noself", "no extra weight on own cost in selection"),
       ("scct_rankcont", "rank by the contention table (first version)"),
       ("scct_mu1", "OPPTS priority, mu = 1")]
P = common(["scct", "oppts"] + [a for a, _ in abl])
rows = [[a, d, f"{stats('scct', a, P)[1]:+.2f}%", f"{stats(a, 'oppts', P)[1]:+.2f}%"] for a, d in abl]
table(f"SCCT component ablations (all, n={len(P)})", rows, ["variant", "change", "SCCT gain over variant", "variant vs OPPTS"])

# 5. dev-split sweeps
sw = [("scct_mu0", "mu = 0"), ("scct_mu05", "mu = 0.5"), ("scct_mu1", "mu = 1 (OPPTS priority)"), ("scct_mu15", "mu = 1.5"),
      ("scct", "mu = 2 (chosen)"), ("scct_mu3", "mu = 3"), ("scct_mu5", "mu = 5"), ("scct_mu10", "mu = 10"),
      ("scct_g025", "mu = 0, gamma = 0.25"), ("scct_g05", "mu = 0, gamma = 0.5"), ("scct_g075", "mu = 0, gamma = 0.75"),
      ("scct_rankcont", "rank by contention table"), ("scct_work", "contention table + computation-only path"),
      ("scct_cap1", "contention capped at p children (first-version ranking)")]
P = common(["oppts", "heft"] + [a for a, _ in sw], "dev")
rows = []
for a, d in sw:
    Pw = [p for p in P if fam(p) in WF]; Pr = [p for p in P if fam(p).startswith("R-")]
    rows.append([a, d, f"{stats(a, 'oppts', P)[1]:+.2f}%", f"{stats(a, 'oppts', Pw)[1]:+.2f}%", f"{stats(a, 'oppts', Pr)[1]:+.2f}%", f"{stats(a, 'heft', P)[1]:+.2f}%"])
table(f"Priority sweeps on the dev split (n={len(P)}), gain vs OPPTS / HEFT", rows,
      ["variant", "setting", "vs OPPTS", "workflows", "random", "vs HEFT"])
