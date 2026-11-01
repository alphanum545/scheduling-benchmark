#!/usr/bin/env python3
"""Statistical analysis for the CGS manuscript (same statistics as the DSLA manuscript; DSLA is not included).

Input : one or more run CSVs written by tools/bench.py run (all algorithms in algorithms.toml).
Output: results.json with every number quoted in the manuscript.

Conventions
  gain_B(i) = 100 (C_B(i) - C_CGS(i)) / C_B(i)    (positive favours CGS)
  cluster   = (family, nodes, instance): workflow instances share one DAG across processor
              counts, so they are not independent observations.
  CI        = 95% percentile bootstrap over clusters (10,000 resamples, seed 20261031).
  test      = cluster-level two-sided Wilcoxon signed-rank on per-cluster mean gains,
              Holm-adjusted over the comparator family.
"""
import json, sys, hashlib, numpy as np, pandas as pd
from scipy import stats

STORE = sys.argv[1]
EXTRA = sys.argv[2:]                       # optional extra CSVs with local ablation rows
PROP = 'cgs'
BASE = ['heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']   # all insertion-based
APPEND = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']                                   # repository originals
ABL = ['cgs_single', 'cgs_credit0', 'cgs_nogate', 'cgs_lam0', 'cgs_tailnone', 'cgs_commitall', 'cgs_parents3']
EXTRA_M = ['cpop', 'ppts', 'ipeft', 'mpeft', 'lstd', 'left', 'left_d']
B_REPS, SEED = 10000, 20261031


def split_of(path):
    return 'dev' if int(hashlib.sha256(path.encode()).hexdigest()[:8], 16) % 4 == 0 else 'test'


def load():
    d = pd.read_csv(STORE)
    for f in EXTRA:
        d = pd.concat([d, pd.read_csv(f)], ignore_index=True)
    d = d[d.status == 'ok']
    mk = d.pivot_table(index='path', columns='algo', values='makespan', aggfunc='first')
    meta = d.drop_duplicates('path').set_index('path')[['dataset', 'family', 'nodes', 'n', 'p', 'instance', 'lb']]
    meta = meta.loc[mk.index]
    meta['split'] = [split_of(p) for p in meta.index]
    meta['cluster'] = meta.family + '/' + meta.nodes.astype(str) + '/' + meta.instance.astype(str)
    meta['kind'] = np.where(meta.family.str.startswith('R-'), 'random', 'workflow')
    fam = meta.family.str.extract(r'R-ccr([\d.]+)-b([\d.]+)')
    meta['ccr'], meta['beta'] = fam[0], fam[1]
    return mk, meta


def boot_ci(g, cl, rng):
    """Cluster bootstrap CI of the instance-level mean of g."""
    df = pd.DataFrame({'g': g, 'c': cl})
    agg = df.groupby('c').g.agg(['sum', 'count'])
    s, n = agg['sum'].to_numpy(), agg['count'].to_numpy()
    k = len(s)
    idx = rng.integers(0, k, size=(B_REPS, k))
    means = s[idx].sum(1) / n[idx].sum(1)
    return float(np.percentile(means, 2.5)), float(np.percentile(means, 97.5))


def compare(mk, meta, a, b, rng):
    """Paired comparison of algorithm a against comparator b on the rows of meta."""
    x, y = mk.loc[meta.index, a].to_numpy(float), mk.loc[meta.index, b].to_numpy(float)
    g = 100 * (y - x) / y
    lo, hi = boot_ci(g, meta.cluster.to_numpy(), rng)
    cm = pd.Series(g, index=meta.index).groupby(meta.cluster).mean()
    nz = cm[cm != 0]
    p = float(stats.wilcoxon(nz).pvalue) if len(nz) > 0 else 1.0
    return dict(mean=float(g.mean()), median=float(np.median(g)), ci=[lo, hi],
                wins=int((x < y).sum()), ties=int((x == y).sum()), losses=int((x > y).sum()),
                n=int(len(g)), clusters=int(cm.size), p_cluster=p,
                worst=float(g.min()), best=float(g.max()),
                ratio_of_totals=float(100 * (y.sum() - x.sum()) / y.sum()))


def holm(res, key='p_cluster'):
    items = sorted(res.items(), key=lambda kv: kv[1][key])
    m, run = len(items), 0.0
    for i, (k, v) in enumerate(items):
        run = max(run, min(1.0, (m - i) * v[key]))
        v['p_holm'] = run


def friedman(mk, meta, algos):
    X = mk.loc[meta.index, algos].to_numpy(float)
    ranks = stats.rankdata(X, axis=1).mean(0)
    chi, p = stats.friedmanchisquare(*X.T)
    k, N = len(algos), len(X)
    q = {2: 1.960, 3: 2.343, 4: 2.569, 5: 2.728, 6: 2.850, 7: 2.949, 8: 3.031}[k]
    cd = q * np.sqrt(k * (k + 1) / (6 * N))
    return dict(ranks={a: float(r) for a, r in zip(algos, ranks)}, chi2=float(chi), p=float(p), cd=float(cd), N=int(N))


def main():
    mk, meta = load()
    rng = np.random.default_rng(SEED)
    out = {'counts': {}, 'overall': {}, 'strata': {}, 'friedman': {}, 'slr': {}, 'speedup': {}, 'efficiency': {},
           'secondary': {}}
    out['counts'] = dict(instances=int(len(meta)), clusters=int(meta.cluster.nunique()),
                         test=int((meta.split == 'test').sum()), dev=int((meta.split == 'dev').sum()),
                         test_clusters=int(meta[meta.split == 'test'].cluster.nunique()),
                         workflow=int((meta.kind == 'workflow').sum()), random=int((meta.kind == 'random').sum()))
    vb = mk[BASE].min(axis=1)
    mk = mk.assign(virtual_best=vb)
    d = pd.read_csv(STORE)
    d = d[d.status == 'ok']
    sp_eff = {c: d.pivot_table(index='path', columns='algo', values=c, aggfunc='first') for c in ('speedup', 'efficiency')}
    for sp in ['test', 'all', 'dev']:
        m = meta if sp == 'all' else meta[meta.split == sp]
        res = {b: compare(mk, m, PROP, b, rng) for b in BASE}
        holm(res)
        res['virtual_best'] = compare(mk, m, PROP, 'virtual_best', rng)
        out['overall'][sp] = res
        out['friedman'][sp] = friedman(mk, m, [PROP] + BASE)
        out['slr'][sp] = {a: float((mk.loc[m.index, a] / m.lb).mean()) for a in [PROP] + BASE}
        for c in ('speedup', 'efficiency'):
            out[c][sp] = {a: float(sp_eff[c].loc[m.index, a].mean()) for a in [PROP] + BASE}
    # strata on the held-out split
    t = meta[meta.split == 'test']
    def strata(tt, col, values):
        r = {}
        for v in values:
            m = tt[tt[col] == v]
            if len(m):
                r[str(v)] = {b: {k: compare(mk, m, PROP, b, rng)[k] for k in ('mean', 'ci', 'wins', 'ties', 'losses', 'n')}
                             for b in BASE + ['virtual_best']}
        return r
    out['strata']['family'] = strata(t, 'family', sorted(t.family.unique()))
    out['strata']['p'] = strata(t, 'p', sorted(t.p.unique()))
    out['strata']['ccr'] = strata(t, 'ccr', ['0.1', '1', '10'])
    out['strata']['beta'] = strata(t, 'beta', ['0.1', '0.5', '1'])
    out['strata']['kind'] = strata(t, 'kind', ['workflow', 'random'])
    t2 = t.assign(size=pd.cut(t.n, [0, 100, 500, 3000], labels=['<=100', '101-500', '>500']).astype(str))
    out['strata']['size'] = strata(t2, 'size', ['<=100', '101-500', '>500'])
    # secondary metrics (means over the held-out split) straight from the store
    ds = d[d.path.isin(t.index) & d.algo.isin([PROP] + BASE)]
    out['secondary'] = ds.groupby('algo')[['utilization', 'comm_ratio', 'imbalance', 'rob_ratio', 'rob_cv', 'cont_ratio']].mean().to_dict('index')
    # robustness / contention paired: CGS vs each baseline under perturbed and contention replays
    rob = ds.assign(rob_mk=ds.makespan * ds.rob_ratio).pivot_table(index='path', columns='algo', values='rob_mk')
    cont = ds.pivot_table(index='path', columns='algo', values='cont_mk')
    out['replay'] = {}
    for name, tab in (('noise', rob), ('contention', cont)):
        tab = tab.loc[t.index]
        r = {}
        for b in BASE:
            x, y = tab[PROP].to_numpy(float), tab[b].to_numpy(float)
            g = 100 * (y - x) / y
            lo, hi = boot_ci(g, t.cluster.to_numpy(), rng)
            r[b] = dict(mean=float(g.mean()), ci=[lo, hi], wins=int((x < y).sum()), ties=int((x == y).sum()), losses=int((x > y).sum()))
        out['replay'][name] = r
    # ablations: full CGS versus each variant (positive = full CGS shorter)
    abl = [c for c in ABL if c in mk.columns]
    out['ablation'] = {}
    for sp in ['test', 'all']:
        m = meta if sp == 'all' else meta[meta.split == sp]
        res = {c: compare(mk, m, PROP, c, rng) for c in abl}
        holm(res)
        out['ablation'][sp] = res
    # component effects by communication group on the held-out split
    grp = np.where(t.family.str.startswith('R-ccr10'), 'ccr10', np.where(t.family.str.startswith('R-ccr1-'), 'ccr1',
          np.where(t.family.str.startswith('R-ccr0.1'), 'ccr0.1', 'workflow')))
    gm = lambda a, b, ix: float((100 * (mk.loc[ix, b] - mk.loc[ix, a]) / mk.loc[ix, b]).mean())
    out['mechanism_by_group'] = {}
    for gname in ['ccr0.1', 'ccr1', 'ccr10', 'workflow']:
        ix = t.index[grp == gname]
        out['mechanism_by_group'][gname] = dict(
            n=int(len(ix)), joint_placement=gm(PROP, 'cgs_single', ix), comm_credit=gm(PROP, 'cgs_credit0', ix),
            gate=gm(PROP, 'cgs_nogate', ix), table_in_selection=gm(PROP, 'cgs_lam0', ix),
            changed_by_closures=float((mk.loc[ix, PROP] != mk.loc[ix, 'cgs_single']).mean()))
    # context: CGS against the repository's append-only originals, and what insertion alone gives each baseline
    out['append'] = {}
    res = {b: compare(mk, t, PROP, b, rng) for b in APPEND}
    holm(res)
    out['append']['cgs_vs_original'] = res
    out['append']['insertion_effect'] = {b: {k: v for k, v in compare(mk, t, b + '_ins', b, rng).items()
                                             if k in ('mean', 'ci', 'wins', 'ties', 'losses', 'n')} for b in APPEND}
    # methods discussed but not used as baselines (append-only, as in the repository)
    out['extra'] = {}
    for b in EXTRA_M:
        if b in mk.columns:
            r = compare(mk, t, PROP, b, rng)
            out['extra'][b] = {k: r[k] for k in ('mean', 'ci', 'wins', 'ties', 'losses', 'n')}
    json.dump(out, open('results.json', 'w'), indent=1)
    print(json.dumps({k: out[k] for k in ('counts',)}, indent=1))
    for sp in ['test', 'all']:
        print(sp)
        for b, v in out['overall'][sp].items():
            print(f"  {b:12s} mean {v['mean']:6.2f} CI [{v['ci'][0]:.2f},{v['ci'][1]:.2f}] median {v['median']:6.2f} "
                  f"W/T/L {v['wins']}/{v['ties']}/{v['losses']} p {v['p_cluster']:.2e} holm {v.get('p_holm', float('nan')):.2e} worst {v['worst']:.1f}")
        print('  ranks', {k: round(x, 3) for k, x in out['friedman'][sp]['ranks'].items()}, 'CD', round(out['friedman'][sp]['cd'], 3))


if __name__ == '__main__':
    main()
