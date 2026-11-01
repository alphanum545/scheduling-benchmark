#!/usr/bin/env python3
"""Statistical analysis for the DSLA manuscript.

Input : one or more run CSVs written by tools/bench.py run (all algorithms in algorithms.toml).
Output: results.json with every number quoted in the manuscript.

Conventions
  gain_B(i) = 100 (C_B(i) - C_DSLA(i)) / C_B(i)   (positive favours DSLA)
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
BASE = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']
NAMES = dict(heft='HEFT', peft='PEFT', ippts='IPPTS', aeft='AEFT', pmms='PMMS', bppts='BPPTS',
             dsla='DSLA', slas='SLAS')
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
    out = {'counts': {}, 'overall': {}, 'strata': {}, 'friedman': {}, 'slr': {}, 'secondary': {}}
    out['counts'] = dict(instances=int(len(meta)), clusters=int(meta.cluster.nunique()),
                         test=int((meta.split == 'test').sum()), dev=int((meta.split == 'dev').sum()),
                         test_clusters=int(meta[meta.split == 'test'].cluster.nunique()),
                         workflow=int((meta.kind == 'workflow').sum()), random=int((meta.kind == 'random').sum()))
    vb = mk[BASE].min(axis=1)
    mk = mk.assign(virtual_best=vb)
    for sp in ['test', 'all', 'dev']:
        m = meta if sp == 'all' else meta[meta.split == sp]
        res = {b: compare(mk, m, 'dsla', b, rng) for b in BASE}
        holm(res)
        res['virtual_best'] = compare(mk, m, 'dsla', 'virtual_best', rng)
        out['overall'][sp] = res
        out['friedman'][sp] = friedman(mk, m, ['dsla'] + BASE)
        out['slr'][sp] = {a: float((mk.loc[m.index, a] / m.lb).mean()) for a in ['dsla'] + BASE}
    # strata on the held-out split
    t = meta[meta.split == 'test']
    def strata(col, values):
        r = {}
        for v in values:
            m = t[t[col] == v]
            if len(m):
                r[str(v)] = {b: {k: compare(mk, m, 'dsla', b, rng)[k] for k in ('mean', 'ci', 'wins', 'ties', 'losses', 'n')}
                             for b in BASE + ['virtual_best']}
        return r
    out['strata']['family'] = strata('family', sorted(t.family.unique()))
    out['strata']['p'] = strata('p', sorted(t.p.unique()))
    out['strata']['ccr'] = strata('ccr', ['0.1', '1', '10'])
    out['strata']['beta'] = strata('beta', ['0.1', '0.5', '1'])
    out['strata']['kind'] = strata('kind', ['workflow', 'random'])
    # size bands on workflow families
    t2 = t.assign(size=pd.cut(t.n, [0, 100, 500, 3000], labels=['<=100', '101-500', '>500']))
    t_backup = t; t = t2
    out['strata']['size'] = strata('size', ['<=100', '101-500', '>500'])
    t = t_backup
    # secondary metrics (means over the held-out split) straight from the store
    d = pd.read_csv(STORE)
    d = d[(d.status == 'ok') & d.path.isin(t.index) & d.algo.isin(['dsla'] + BASE)]
    out['secondary'] = d.groupby('algo')[['utilization', 'comm_ratio', 'imbalance', 'rob_ratio', 'rob_cv', 'cont_ratio']].mean().to_dict('index')
    # robustness / contention paired: DSLA vs each baseline under perturbed and contention replays
    rob = d.assign(rob_mk=d.makespan * d.rob_ratio).pivot_table(index='path', columns='algo', values='rob_mk')
    cont = d.pivot_table(index='path', columns='algo', values='cont_mk')
    out['replay'] = {}
    for name, tab in (('noise', rob), ('contention', cont)):
        tab = tab.loc[t.index]
        r = {}
        for b in BASE:
            x, y = tab['dsla'].to_numpy(float), tab[b].to_numpy(float)
            g = 100 * (y - x) / y
            lo, hi = boot_ci(g, t.cluster.to_numpy(), rng)
            r[b] = dict(mean=float(g.mean()), ci=[lo, hi], wins=int((x < y).sum()), ties=int((x == y).sum()), losses=int((x > y).sum()))
        out['replay'][name] = r
    # dual-policy selection (needs the EFT-only build, if available)
    if 'v_eft' in mk.columns:
        sel = {}
        for sp in ['test', 'all']:
            m = meta if sp == 'all' else meta[meta.split == sp]
            r_, e_ = mk.loc[m.index, 'slas'], mk.loc[m.index, 'v_eft']
            sel[sp] = dict(rank_strict=int((r_ < e_).sum()), eft_strict=int((e_ < r_).sum()), tie=int((e_ == r_).sum()))
        out['selection'] = sel
    # ablations on the held-out split: DSLA versus each variant (positive = full DSLA shorter)
    abl = [c for c in ['slas', 'v_eft', 'v_static', 'dsla_fix', 'v_allkids', 'v_nolook', 'v_rank01', 'v_eft05', 'v_static_slas'] if c in mk.columns]
    out['ablation'] = {}
    for sp in ['test', 'all']:
        m = meta if sp == 'all' else meta[meta.split == sp]
        res = {c: compare(mk, m, 'dsla', c, rng) for c in abl}
        holm(res)
        out['ablation'][sp] = res
    # mechanism-only comparison: no-look-ahead dual build versus single baselines (context)
    if 'v_nolook' in mk.columns:
        out['nolook_vs_base'] = {b: compare(mk, t, 'v_nolook', b, rng)['mean'] for b in BASE}
    # mechanism effects by communication group on the held-out split (Section 8.3)
    if 'v_static' in mk.columns:
        grp = np.where(t.family.str.startswith('R-ccr10'), 'ccr10', np.where(t.family.str.startswith('R-ccr1-'), 'ccr1',
              np.where(t.family.str.startswith('R-ccr0.1'), 'ccr0.1', 'workflow')))
        gm = lambda a, b, ix: float((100 * (mk.loc[ix, b] - mk.loc[ix, a]) / mk.loc[ix, b]).mean())
        out['mechanism_by_group'] = {}
        for gname in ['ccr0.1', 'ccr1', 'ccr10', 'workflow']:
            ix = t.index[grp == gname]
            out['mechanism_by_group'][gname] = dict(
                n=int(len(ix)), state_dual=gm('dsla', 'v_static', ix), state_rank=gm('slas', 'v_static_slas', ix),
                lookahead=gm('dsla', 'v_nolook', ix), own_cost=gm('dsla', 'dsla_fix', ix), second_build=gm('dsla', 'slas', ix),
                eft_selected_share=float((mk.loc[ix, 'v_eft'] < mk.loc[ix, 'slas']).mean()))
        out['state_within_rank_build'] = compare(mk, t, 'slas', 'v_static_slas', rng)
    # methods discussed but not used as baselines (Section 2, Table "extra")
    out['extra'] = {}
    for b in ['cpop', 'ppts', 'ipeft', 'mpeft', 'left', 'left_d']:
        if b in mk.columns:
            r = compare(mk, t, 'dsla', b, rng)
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
