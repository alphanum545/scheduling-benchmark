#!/usr/bin/env python3
"""cmp.py REF csv... : paired comparison of every algo vs REF (mean paired gain %, W/T/L, mean makespan, validity),
plus Wilcoxon p if scipy is present. Optional --vs NAME adds a second reference column."""
import sys, csv
from collections import defaultdict
args = sys.argv[1:]
ref = args[0]; files = args[1:]
mk = defaultdict(dict); valid = defaultdict(lambda: [0, 0]); rt = defaultdict(list)
for f in files:
    for r in csv.DictReader(open(f)):
        valid[r["algo"]][1] += 1
        if r["valid"] in ("1", "True"):
            valid[r["algo"]][0] += 1
            mk[r["algo"]][r["path"]] = float(r["makespan"]); rt[r["algo"]].append(float(r["runtime"]))
try:
    from scipy.stats import wilcoxon
except Exception:
    wilcoxon = None
common = set.intersection(*[set(d) for d in mk.values()])
print(f"common instances: {len(common)}; reference = {ref} (positive gain = algo shorter than {ref})")
print(f"{'algo':16}{'valid':>10}{'mean mk':>11}{'gain%':>9}{'W/T/L':>14}{'p':>11}{'rt s':>8}")
for a in sorted(mk, key=lambda a: sum(mk[a][p] for p in common)):
    g = [100 * (mk[ref][p] - mk[a][p]) / mk[ref][p] for p in common]
    w = sum(x > 1e-12 for x in g); l = sum(x < -1e-12 for x in g); t = len(g) - w - l
    pv = ""
    if wilcoxon and a != ref and (w + l) > 0:
        pv = f"{wilcoxon([mk[ref][p] for p in common], [mk[a][p] for p in common]).pvalue:.2e}"
    print(f"{a:16}{valid[a][0]:>5}/{valid[a][1]:<4}{sum(mk[a][p] for p in common)/len(common):>11.2f}"
          f"{sum(g)/len(g):>9.3f}{f'{w}/{t}/{l}':>14}{pv:>11}{sum(rt[a])/len(rt[a]):>8.4f}")
