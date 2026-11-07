#!/usr/bin/env python3
"""Statistics for the CGS paper (paper/cgs-final).

Reads results/reports/<name>/results.csv and produces the evidence tables the
paper needs:

  1. Per-benchmark paired comparison of CGS against each baseline
     (mean/median paired gain, W/T/L, bootstrap CI on the mean).
  2. Wilcoxon signed-rank, instance-level and per-DAG, Holm-corrected.
  3. Friedman test + Nemenyi critical difference over all algorithms.
  4. Per-family and per-processor breakdowns.
  5. Ablations (cgs_single, cgs_credit0) against full CGS.

Benchmarks are reported separately (workflows, random graphs) and never pooled,
as the paper requires.

usage: stats_paper.py <report_dir> [--out DIR]
"""
import csv
import hashlib
import json
import math
import os
import sys
from collections import defaultdict

WORKFLOWS = ("FFT", "GE", "Genome", "LA", "LIGO", "Montage")
REF = "cgs"
BASELINES = ["heft_ins", "peft", "aeft", "bppts", "left", "la_heft"]
ABLATIONS = ["cgs_single", "cgs_credit0"]


# ---------------------------------------------------------------- statistics
def rankdata(xs):
    """Average ranks, ties shared."""
    order = sorted(range(len(xs)), key=lambda i: xs[i])
    ranks = [0.0] * len(xs)
    i = 0
    while i < len(order):
        j = i
        while j + 1 < len(order) and xs[order[j + 1]] == xs[order[i]]:
            j += 1
        avg = (i + j) / 2.0 + 1.0
        for k in range(i, j + 1):
            ranks[order[k]] = avg
        i = j + 1
    return ranks


def wilcoxon(diffs):
    """Two-sided Wilcoxon signed-rank. Returns (W, z, p) with normal approx and
    tie correction; zero differences dropped (Wilcoxon's standard handling)."""
    d = [x for x in diffs if x != 0]
    n = len(d)
    if n == 0:
        return 0.0, 0.0, 1.0
    ranks = rankdata([abs(x) for x in d])
    wpos = sum(r for r, x in zip(ranks, d) if x > 0)
    wneg = sum(r for r, x in zip(ranks, d) if x < 0)
    w = min(wpos, wneg)
    mean = n * (n + 1) / 4.0
    # tie correction on the rank variance
    _, counts = zip(*[(r, ranks.count(r)) for r in set(ranks)])
    tie_term = sum(c ** 3 - c for c in counts)
    var = n * (n + 1) * (2 * n + 1) / 24.0 - tie_term / 48.0
    if var <= 0:
        return w, 0.0, 1.0
    z = (w - mean) / math.sqrt(var)
    p = 2.0 * (1.0 - _norm_cdf(abs(z)))
    return w, z, min(1.0, p)


def _norm_cdf(x):
    return 0.5 * (1.0 + math.erf(x / math.sqrt(2.0)))


def holm(pvals):
    """Holm-Bonferroni step-down. Returns adjusted p in the input order."""
    m = len(pvals)
    order = sorted(range(m), key=lambda i: pvals[i])
    adj = [0.0] * m
    running = 0.0
    for rank, i in enumerate(order):
        val = (m - rank) * pvals[i]
        running = max(running, val)
        adj[i] = min(1.0, running)
    return adj


def friedman(blocks):
    """blocks: list of lists (one rank row per instance, k algorithms).
    Returns (chi2, df, p) using the standard Friedman statistic."""
    n = len(blocks)
    k = len(blocks[0]) if n else 0
    if n == 0 or k < 3:
        return 0.0, 0, 1.0
    colsum = [sum(row[j] for row in blocks) for j in range(k)]
    chi2 = (12.0 / (n * k * (k + 1))) * sum(s * s for s in colsum) - 3 * n * (k + 1)
    df = k - 1
    return chi2, df, 1.0 - _chi2_cdf(chi2, df)


def _chi2_cdf(x, df):
    """Regularized lower incomplete gamma via series/continued fraction."""
    if x <= 0:
        return 0.0
    a = df / 2.0
    z = x / 2.0
    if z < a + 1:
        # series
        term = 1.0 / a
        total = term
        n = 1
        while n < 500:
            term *= z / (a + n)
            total += term
            if abs(term) < 1e-14 * abs(total):
                break
            n += 1
        return total * math.exp(-z + a * math.log(z) - math.lgamma(a))
    # continued fraction
    tiny = 1e-300
    b = z + 1 - a
    c = 1 / tiny
    d = 1 / b
    h = d
    for i in range(1, 500):
        an = -i * (i - a)
        b += 2
        d = an * d + b
        if abs(d) < tiny:
            d = tiny
        c = b + an / c
        if abs(c) < tiny:
            c = tiny
        d = 1 / d
        delta = d * c
        h *= delta
        if abs(delta - 1) < 1e-14:
            break
    return 1.0 - math.exp(-z + a * math.log(z) - math.lgamma(a)) * h


def bootstrap_ci(vals, iters=2000, alpha=0.05, seed=7):
    """Percentile bootstrap CI for the mean."""
    if not vals:
        return (0.0, 0.0)
    rng = _Rng(seed)
    n = len(vals)
    means = []
    for _ in range(iters):
        s = 0.0
        for _ in range(n):
            s += vals[rng.next_below(n)]
        means.append(s / n)
    means.sort()
    lo = means[int((alpha / 2) * iters)]
    hi = means[int((1 - alpha / 2) * iters) - 1]
    return (lo, hi)


class _Rng:
    """Small deterministic LCG so results are reproducible without numpy."""
    def __init__(self, seed):
        self.s = seed & 0xFFFFFFFF

    def next_below(self, n):
        self.s = (1103515245 * self.s + 12345) & 0x7FFFFFFF
        return self.s % n


# ---------------------------------------------------------------- data
def split_of(p):
    return "dev" if int(hashlib.sha256(p.encode()).hexdigest()[:8], 16) % 4 == 0 else "test"


def load(report_dir):
    path = os.path.join(report_dir, "results.csv")
    mk, meta = defaultdict(dict), {}
    for r in csv.DictReader(open(path)):
        if r.get("status") != "ok" or not r.get("makespan"):
            continue
        mk[r["algo"]][r["path"]] = float(r["makespan"])
        meta[r["path"]] = {
            "family": r["family"],
            "dataset": r["dataset"],
            "n": int(r["n"]),
            "p": int(r["p"]),
        }
    return mk, meta


def benchmark_of(path, meta):
    return "workflows" if meta[path]["family"] in WORKFLOWS else "random"


def gain(ref, other):
    """Percent reduction of `other` relative to `ref`; positive = ref shorter."""
    return 100.0 * (other - ref) / other


def dag_key(path):
    """One observation per DAG: strip the processor-count segment."""
    parts = path.split("/")
    return "/".join(parts[:-2] + parts[-1:]) if len(parts) >= 3 else path


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: stats_paper.py <report_dir> [--out DIR]")
    report_dir = sys.argv[1]
    out_dir = report_dir
    if "--out" in sys.argv:
        out_dir = sys.argv[sys.argv.index("--out") + 1]
    os.makedirs(out_dir, exist_ok=True)

    mk, meta = load(report_dir)
    algos = [a for a in [REF] + BASELINES + ABLATIONS if a in mk]
    paths = sorted(set.intersection(*[set(mk[a]) for a in algos]))
    print(f"report: {report_dir}")
    print(f"algorithms: {len(algos)}  instances with full coverage: {len(paths)}")

    results = {}
    for bench in ("workflows", "random"):
        ps = [p for p in paths if benchmark_of(p, meta) == bench]
        if not ps:
            continue
        print(f"\n{'='*78}\nBENCHMARK: {bench}  (n={len(ps)})\n{'='*78}")
        block = {"n": len(ps), "comparisons": {}, "families": {}, "processors": {}}

        # ---- 1/2. paired comparisons + Wilcoxon
        print(f"{'baseline':12}{'mean%':>9}{'median%':>9}{'95% CI':>20}{'W/T/L':>14}{'p_inst':>10}{'p_dag_holm':>12}")
        raw_p = []
        rows = []
        for a in BASELINES:
            if a not in mk:
                continue
            gs = [gain(mk[REF][p], mk[a][p]) for p in ps]
            w = sum(1 for p in ps if mk[REF][p] < mk[a][p])
            l = sum(1 for p in ps if mk[REF][p] > mk[a][p])
            t = len(ps) - w - l
            _, _, p_inst = wilcoxon(gs)
            # per-DAG: average gain within each DAG, then test
            bydag = defaultdict(list)
            for p, g in zip(ps, gs):
                bydag[dag_key(p)].append(g)
            dag_gains = [sum(v) / len(v) for v in bydag.values()]
            _, _, p_dag = wilcoxon(dag_gains)
            lo, hi = bootstrap_ci(gs)
            med = sorted(gs)[len(gs) // 2]
            rows.append((a, sum(gs) / len(gs), med, lo, hi, w, t, l, p_inst, p_dag, len(dag_gains)))
            raw_p.append(p_dag)
        adj = holm(raw_p) if raw_p else []
        for (a, mean, med, lo, hi, w, t, l, p_inst, p_dag, nd), pa in zip(rows, adj):
            print(f"{a:12}{mean:>+8.2f}%{med:>+8.2f}%  [{lo:+6.2f},{hi:+6.2f}]{w:>7}/{t}/{l}{p_inst:>10.2e}{pa:>12.2e}")
            block["comparisons"][a] = {
                "mean_gain_pct": mean, "median_gain_pct": med,
                "ci95": [lo, hi], "wins": w, "ties": t, "losses": l,
                "p_instance": p_inst, "p_dag": p_dag, "p_dag_holm": pa,
                "n_dags": nd, "significant_holm": pa < 0.05,
            }

        # ---- 3. Friedman + Nemenyi
        ranked = [a for a in algos if all(p in mk[a] for p in ps)]
        if len(ranked) >= 3:
            blocks = []
            for p in ps:
                vals = [mk[a][p] for a in ranked]
                blocks.append(rankdata(vals))
            chi2, df, pval = friedman(blocks)
            n, k = len(blocks), len(ranked)
            cd = 2.569 * math.sqrt(k * (k + 1) / (6.0 * n))  # q(0.05, k) approx for k<=10
            avg = {a: sum(b[j] for b in blocks) / n for j, a in enumerate(ranked)}
            print(f"\nFriedman: chi2={chi2:.2f} df={df} p={pval:.3e}   Nemenyi CD(0.05)={cd:.3f} ranks")
            best = min(avg, key=avg.get)
            for a in sorted(avg, key=avg.get):
                diff = avg[a] - avg[best]
                print(f"  {a:12} rank={avg[a]:6.3f}  {'differs' if diff > cd else 'same as best'}")
            block["friedman"] = {"chi2": chi2, "df": df, "p": pval, "nemenyi_cd": cd,
                                 "avg_ranks": avg, "best": best}

        # ---- 4a. per family
        print(f"\nper-family mean gain vs CGS:")
        fams = sorted({meta[p]['family'] for p in ps})
        hdr = "".join(f"{a[:9]:>11}" for a in BASELINES if a in mk)
        print(f"{'family':14}{'n':>5}{hdr}")
        for f in fams:
            fp = [p for p in ps if meta[p]["family"] == f]
            line = f"{f:14}{len(fp):>5}"
            entry = {}
            for a in BASELINES:
                if a not in mk:
                    continue
                g = sum(gain(mk[REF][p], mk[a][p]) for p in fp) / len(fp)
                line += f"{g:>+10.2f}%"
                entry[a] = g
            print(line)
            block["families"][f] = entry

        # ---- 4b. per processor count
        print(f"\nper-processor mean gain vs CGS:")
        print(f"{'p':>5}{'n':>5}{hdr}")
        for pv in sorted({meta[p]["p"] for p in ps}):
            pp = [p for p in ps if meta[p]["p"] == pv]
            line = f"{pv:>5}{len(pp):>5}"
            entry = {}
            for a in BASELINES:
                if a not in mk:
                    continue
                g = sum(gain(mk[REF][p], mk[a][p]) for p in pp) / len(pp)
                line += f"{g:>+10.2f}%"
                entry[a] = g
            print(line)
            block["processors"][str(pv)] = entry

        # ---- 5. ablations
        print(f"\nablations (positive = full CGS better):")
        for a in ABLATIONS:
            if a not in mk:
                continue
            gs = [gain(mk[REF][p], mk[a][p]) for p in ps]
            w = sum(1 for p in ps if mk[REF][p] < mk[a][p])
            l = sum(1 for p in ps if mk[REF][p] > mk[a][p])
            _, _, pv = wilcoxon(gs)
            print(f"  {a:12} {sum(gs)/len(gs):+7.2f}%  W/L={w}/{l}  p={pv:.2e}")
            block.setdefault("ablations", {})[a] = {
                "mean_gain_pct": sum(gs) / len(gs), "wins": w, "losses": l, "p": pv}

        results[bench] = block

    out = os.path.join(out_dir, "stats.json")
    json.dump(results, open(out, "w"), indent=2)
    print(f"\nwrote {out}")


if __name__ == "__main__":
    main()
