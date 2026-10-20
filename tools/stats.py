#!/usr/bin/env python3
"""stats.py: statistical significance tests for paired makespan comparisons.

Every algorithm is run on the same instances, so comparisons are paired.

  * Wilcoxon signed-rank test (two-sided), highlight vs each other algorithm, on the per-instance
    relative difference (other - highlight) / other, so large instances do not dominate.
    p-values are Holm-Bonferroni corrected for the number of comparisons.
    Effect size: matched-pairs rank-biserial correlation r in [-1, 1] (positive = highlight better).
    Median relative gain with a 95% bootstrap confidence interval.
  * Friedman test over all algorithms (are they all equivalent?), average ranks per algorithm
    (1 = best) and the Nemenyi critical difference: two algorithms whose average ranks differ by
    more than CD are significantly different at alpha = 0.05 (Demsar, JMLR 2006).

Note: instances that share a DAG and differ only in the processor count are not fully independent;
the tests treat every instance as one observation, as is common in this literature.
"""
import math
import numpy as np
import pandas as pd

try:
    from scipy import stats as sst
except ImportError:  # the report workflow installs pandas/matplotlib; add scipy on demand
    import subprocess, sys
    subprocess.run([sys.executable, "-m", "pip", "install", "--quiet", "scipy"], check=True)
    from scipy import stats as sst

ALPHA = 0.05


def holm(pvals):
    """Holm-Bonferroni step-down correction; returns adjusted p-values in the input order."""
    p = np.asarray(pvals, float)
    order = np.argsort(p)
    adj = np.empty_like(p)
    running = 0.0
    m = len(p)
    for i, idx in enumerate(order):
        running = max(running, min(1.0, (m - i) * p[idx]))
        adj[idx] = running
    return adj


def rank_biserial(d):
    """Matched-pairs rank-biserial correlation of differences d (zeros dropped)."""
    d = d[d != 0]
    if len(d) == 0:
        return 0.0
    r = pd.Series(np.abs(d)).rank().to_numpy()
    pos, neg = r[d > 0].sum(), r[d < 0].sum()
    return float((pos - neg) / (pos + neg))


def boot_ci_median(x, n=2000, seed=0):
    rng = np.random.default_rng(seed)
    idx = rng.integers(0, len(x), size=(n, len(x)))
    meds = np.median(x[idx], axis=1)
    return float(np.percentile(meds, 2.5)), float(np.percentile(meds, 97.5))


def pairwise_wilcoxon(m, highlight, names, groups=None):
    """m: DataFrame index=instance, columns=algorithm, values=makespan.
    groups: optional Series (index=instance) naming the DAG; when given, an extra conservative
    p-value is computed on per-DAG mean gains (one observation per DAG, all processor counts pooled)."""
    rows = []
    for other in names:
        if other == highlight:
            continue
        d = ((m[other] - m[highlight]) / m[other]).to_numpy()  # >0: highlight shorter
        nz = d[d != 0]
        if len(nz) == 0:
            p = 1.0
        else:
            p = float(sst.wilcoxon(nz, zero_method="wilcox", alternative="two-sided").pvalue)
        lo, hi = boot_ci_median(d * 100)
        rows.append(dict(versus=other, n=len(d), wins=int((d > 0).sum()), ties=int((d == 0).sum()),
                         losses=int((d < 0).sum()), mean_gain_pct=round(100 * d.mean(), 3),
                         median_gain_pct=round(100 * float(np.median(d)), 3),
                         median_ci95=f"[{lo:.3f}, {hi:.3f}]", effect_r=round(rank_biserial(d), 3),
                         p_value=p))
        if groups is not None:
            gd = pd.Series(d, index=m.index).groupby(groups.reindex(m.index).to_numpy()).mean().to_numpy()
            gnz = gd[gd != 0]
            rows[-1]["n_dags"] = len(gd)
            rows[-1]["p_per_dag"] = float(sst.wilcoxon(gnz).pvalue) if len(gnz) else 1.0
    t = pd.DataFrame(rows)
    if len(t):
        t["p_holm"] = holm(t.p_value)
        t["significant"] = np.where(t.p_holm < ALPHA, np.where(t.effect_r > 0, "yes, better", "yes, worse"), "no")
        if "p_per_dag" in t:
            t["p_per_dag_holm"] = holm(t.p_per_dag)
            t["robust"] = np.where((t.p_holm < ALPHA) & (t.p_per_dag_holm < ALPHA), "yes", "no")
            t = t.drop(columns="p_per_dag")
            t["p_per_dag_holm"] = t.p_per_dag_holm.map(lambda v: f"{v:.2e}")
        t["p_value"] = t.p_value.map(lambda v: f"{v:.2e}")
        t["p_holm"] = t.p_holm.map(lambda v: f"{v:.2e}")
    return t


def friedman_nemenyi(m, names):
    """Friedman chi-square test, average ranks and Nemenyi critical difference."""
    m = m[names].dropna()
    n, k = m.shape
    ranks = m.rank(axis=1, method="average")  # 1 = shortest makespan
    avg = ranks.mean().sort_values()
    if k < 3:
        return dict(n=n, k=k, chi2=None, p=None, cd=None), avg
    chi2, p = sst.friedmanchisquare(*[m[c].to_numpy() for c in names])
    q = sst.studentized_range.ppf(1 - ALPHA, k, np.inf) / math.sqrt(2)
    cd = q * math.sqrt(k * (k + 1) / (6.0 * n))
    return dict(n=n, k=k, chi2=float(chi2), p=float(p), cd=float(cd)), avg


def significance_tables(c, names, highlight, by_family=True):
    """c: comparable runs (DataFrame with path, family, algo, makespan). Returns dict of tables + text."""
    m = c.pivot_table(index="path", columns="algo", values="makespan")[names]
    info = c.drop_duplicates("path").set_index("path")
    dag_cols = [k for k in ("dataset", "family", "nodes", "instance") if k in info]
    groups = info[dag_cols].astype(str).agg("/".join, axis=1) if "instance" in info else None
    fr, avg = friedman_nemenyi(m, names)
    ranks = pd.DataFrame({"average_rank": avg.round(3)})
    best = avg.index[0]
    ranks["differs_from_best"] = [("—" if a == best else ("yes" if fr["cd"] and avg[a] - avg[best] > fr["cd"] else "no"))
                                  for a in avg.index]
    out = {"Friedman_ranks": ranks}
    lines = ["## Statistical significance", "",
             f"Friedman test over {fr['k']} algorithms on {fr['n']} instances: "
             + (f"chi2 = {fr['chi2']:.1f}, p = {fr['p']:.2e}; Nemenyi critical difference (alpha 0.05) = {fr['cd']:.3f} ranks."
                if fr["chi2"] is not None else "needs at least 3 algorithms."),
             "", ranks.to_markdown(), ""]
    if highlight:
        w = pairwise_wilcoxon(m, highlight, names, groups)
        out["Wilcoxon_vs_highlight"] = w.set_index("versus")
        lines += [f"### Wilcoxon signed-rank: {highlight} vs each algorithm (Holm-corrected, two-sided)", "",
                  "gain = (other - " + highlight + ") / other; positive = " + highlight + " has the shorter makespan. "
                  "p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor "
                  "counts); 'robust' = significant under both.", "",
                  w.to_markdown(index=False), ""]
        if by_family and "family" in c:
            fam_rows = []
            fams = c.drop_duplicates("path").set_index("path").family
            for fam in sorted(fams.unique()):
                mf = m.loc[fams[fams == fam].index.intersection(m.index)]
                if len(mf) < 6:
                    continue
                wf = pairwise_wilcoxon(mf, highlight, names)
                for _, r in wf.iterrows():
                    fam_rows.append(dict(family=fam, versus=r.versus, n=r.n, mean_gain_pct=r.mean_gain_pct,
                                         effect_r=r.effect_r, p_holm=r.p_holm, significant=r.significant))
            if fam_rows:
                ft = pd.DataFrame(fam_rows)
                out["Wilcoxon_by_family"] = ft.set_index(["family", "versus"])
    out["_markdown"] = "\n".join(lines)
    return out
