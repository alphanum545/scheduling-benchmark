#!/usr/bin/env python3
"""Write LaTeX table bodies for the manuscript from results.json (no hand-copied numbers)."""
import json, math, os, sys
OUT = sys.argv[1]
R = json.load(open('results.json'))
N = dict(heft='HEFT', peft='PEFT', ippts='IPPTS', aeft='AEFT', pmms='PMMS', bppts='BPPTS',
         virtual_best='Virtual best', dsla='DSLA')
BASE = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']


def f2(x): return f'{x:.2f}'.replace('-', '$-$')


def ci(c): return f'[{f2(c[0])}, {f2(c[1])}]'


def pfmt(p):
    if p == 0: return '$<10^{-300}$'
    e = math.floor(math.log10(p)); m = p / 10 ** e
    return f'${m:.1f}\\times10^{{{e}}}$' if e < -2 else f'{p:.3f}'


def w(name, s):
    open(os.path.join(OUT, name), 'w').write(s)


# main comparison
T, A = R['overall']['test'], R['overall']['all']
rows = []
for b in BASE + ['virtual_best']:
    t, a = T[b], A[b]
    p = t.get('p_holm', t['p_cluster'])
    rows.append(f"{N[b]} & {f2(t['mean'])} & {ci(t['ci'])} & {f2(t['median'])} & "
                f"{t['wins']}/{t['ties']}/{t['losses']} & {pfmt(p)} & {f2(a['mean'])} \\\\")
    if b == 'bppts': rows.append('\\midrule')
w('tab_main_rows.tex', '\n'.join(rows) + '\n')

# ranks and SLR
F, S = R['friedman']['test'], R['slr']['test']
order = sorted(F['ranks'], key=lambda a: F['ranks'][a])
w('tab_rank_rows.tex', '\n'.join(f"{N[a]} & {F['ranks'][a]:.2f} & {S[a]:.2f} \\\\" for a in order) + '\n')
w('friedman.tex', f"\\newcommand{{\\FriedChi}}{{{F['chi2']:.0f}}}\\newcommand{{\\FriedCD}}{{{F['cd']:.2f}}}\\newcommand{{\\FriedN}}{{{F['N']}}}\n")

# replays
rows = []
for b in BASE:
    n, c = R['replay']['noise'][b], R['replay']['contention'][b]
    rows.append(f"{N[b]} & {f2(T[b]['mean'])} & {f2(n['mean'])} {ci(n['ci'])} & {f2(c['mean'])} {ci(c['ci'])} \\\\")
w('tab_replay_rows.tex', '\n'.join(rows) + '\n')
sec = R['secondary']
w('tab_secondary_rows.tex', '\n'.join(
    f"{N[a]} & {sec[a]['utilization']:.3f} & {sec[a]['comm_ratio']:.3f} & {sec[a]['rob_ratio']:.3f} & {sec[a]['cont_ratio']:.2f} \\\\"
    for a in ['dsla'] + BASE) + '\n')

# ablation
if R.get('ablation', {}).get('test'):
    L = dict(slas='Rank build only (SLAS)', v_eft='Minimum-EFT build only', v_static='Static state, both builds',
             v_static_slas='Static state, rank build only', dsla_fix='Without own-cost term $\\lambda w_{ik}$',
             v_allkids='All successors ($R=\\infty$)', v_nolook='No look-ahead ($\\lambda=0$), both builds',
             v_rank01='Rank build with $\\lambda=0.1$', v_eft05='Minimum-EFT build with $\\lambda=0.5$')
    AT, AA = R['ablation']['test'], R['ablation']['all']
    keys = [k for k in ['v_nolook', 'v_static', 'dsla_fix', 'v_allkids', 'slas', 'v_eft', 'v_rank01', 'v_eft05'] if k in AT]
    rows = [f"{L[k]} & {f2(AT[k]['mean'])} & {ci(AT[k]['ci'])} & {AT[k]['wins']}/{AT[k]['ties']}/{AT[k]['losses']} & {pfmt(AT[k]['p_holm'])} & {f2(AA[k]['mean'])} \\\\" for k in keys]
    w('tab_ablation_rows.tex', '\n'.join(rows) + '\n')
print('tables written')
