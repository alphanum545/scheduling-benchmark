#!/usr/bin/env python3
"""Manuscript figures from results.json (and timing.json when present). Output: PDF vector figures."""
import json, os, sys
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap, TwoSlopeNorm

OUT = sys.argv[1]
R = json.load(open('results.json'))
INK, INK2, MUTED, GRID, AXIS = '#0b0b0b', '#52514e', '#898781', '#e1e0d9', '#c3c2b7'
CAT = ['#2a78d6', '#eb6834', '#1baf7a', '#eda100', '#e87ba4', '#008300', '#6250d6']
MARK = ['o', 's', '^', 'D', 'v', 'P', 'X']
plt.rcParams.update({
    'font.family': 'serif', 'font.serif': ['DejaVu Serif'], 'font.size': 7.5, 'axes.titlesize': 7.5,
    'axes.labelsize': 7.5, 'xtick.labelsize': 7, 'ytick.labelsize': 7, 'legend.fontsize': 7,
    'axes.edgecolor': AXIS, 'axes.labelcolor': INK2, 'xtick.color': INK2, 'ytick.color': INK2,
    'axes.spines.top': False, 'axes.spines.right': False, 'axes.grid': True, 'grid.color': GRID,
    'grid.linewidth': 0.5, 'axes.axisbelow': True, 'pdf.fonttype': 42, 'lines.linewidth': 1.4,
    'figure.dpi': 200})
NAMES = dict(heft='HEFT', peft='PEFT', ippts='IPPTS', aeft='AEFT', pmms='PMMS', bppts='BPPTS', virtual_best='Virtual best')
BASE = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']


def save(fig, name):
    fig.savefig(os.path.join(OUT, name + '.pdf'), bbox_inches='tight', pad_inches=0.02)
    fig.savefig(os.path.join(OUT, name + '.png'), bbox_inches='tight', pad_inches=0.02, dpi=220)
    plt.close(fig)


# ---------- Figure: family x comparator heatmap of mean gain (held-out split)
def family_heatmap():
    fam = R['strata']['family']
    order = ['LIGO', 'Montage', 'Genome', 'FFT', 'GE', 'LA'] + [f'R-ccr{c}-b{b}' for c in ('0.1', '1', '10') for b in ('0.1', '0.5', '1')]
    cols = BASE + ['virtual_best']
    M = np.array([[fam[f][c]['mean'] for c in cols] for f in order])
    lab = [f if not f.startswith('R-') else 'R: CCR {} / β {}'.format(*f[5:].split('-b')) for f in order]
    ns = [fam[f]['heft']['n'] for f in order]
    cmap = LinearSegmentedColormap.from_list('div', ['#d03b3b', '#f0efec', '#2a78d6'])
    fig, ax = plt.subplots(figsize=(3.45, 3.9))
    norm = TwoSlopeNorm(vmin=-10, vcenter=0, vmax=10)
    ax.imshow(np.clip(M, -10, 10), cmap=cmap, norm=norm, aspect='auto')
    for i in range(M.shape[0]):
        for j in range(M.shape[1]):
            v = M[i, j]
            ax.text(j, i, f'{v:.1f}', ha='center', va='center', fontsize=6.2,
                    color='#ffffff' if abs(v) > 7 else INK)
    ax.set_xticks(range(len(cols)), [NAMES[c] for c in cols], rotation=40, ha='right')
    ax.set_yticks(range(len(order)), [f'{l} ({n})' for l, n in zip(lab, ns)])
    ax.grid(False)
    for s in ax.spines.values(): s.set_visible(False)
    ax.axhline(5.5, color='#ffffff', lw=2)
    ax.axvline(5.5, color='#ffffff', lw=2)
    ax.tick_params(length=0)
    save(fig, 'fig_family_gain')


# ---------- Figure: gain by p, CCR and beta (held-out), strongest comparators
def strata_lines():
    comps = ['heft', 'aeft', 'pmms', 'virtual_best']
    panels = [('p', 'Processors $p$', None), ('ccr', 'CCR (random DAGs)', ['0.1', '1', '10']),
              ('beta', 'Heterogeneity $\\beta$ (random DAGs)', ['0.1', '0.5', '1'])]
    fig, axes = plt.subplots(1, 3, figsize=(7.1, 2.05), sharey=False)
    for ax, (key, xl, keys) in zip(axes, panels):
        S = R['strata'][key]
        ks = keys or sorted(S, key=lambda x: float(x))
        x = np.arange(len(ks))
        for c_i, c in enumerate(comps):
            m = np.array([S[k][c]['mean'] for k in ks])
            lo = np.array([S[k][c]['ci'][0] for k in ks]); hi = np.array([S[k][c]['ci'][1] for k in ks])
            off = (c_i - 1.5) * 0.06
            ax.errorbar(x + off, m, yerr=[m - lo, hi - m], color=CAT[c_i], marker=MARK[c_i], ms=4,
                        mec='#fcfcfb', mew=0.6, capsize=0, elinewidth=0.9, label=NAMES[c])
        ax.axhline(0, color=AXIS, lw=0.8, zorder=0)
        ax.set_xticks(x, [str(int(float(k))) if key == 'p' else k for k in ks])
        ax.set_xlabel(xl)
    axes[0].set_ylabel('Mean gain of DSLA (%)')
    h, l = axes[0].get_legend_handles_labels()
    fig.legend(h, l, loc='upper center', ncol=4, frameon=False, bbox_to_anchor=(0.5, 1.07))
    fig.tight_layout(w_pad=1.2)
    save(fig, 'fig_strata')


# ---------- Figure: ablation forest plot
def ablation():
    if 'ablation' not in R or not R['ablation'].get('test'):
        return
    A = R['ablation']['test']
    labels = dict(slas='Rank build only (SLAS)', v_eft='Min-EFT build only', v_static='Static state (both builds)',
                  v_static_slas='Static state (rank build)', dsla_fix='No own-cost term $\\lambda w_{ik}$',
                  v_allkids='All successors ($R=\\infty$)', v_nolook='No look-ahead ($\\lambda=0$, both builds)',
                  v_rank01='Rank build, $\\lambda=0.1$', v_eft05='Min-EFT build, $\\lambda=0.5$')
    keys = [k for k in ['v_nolook', 'v_static', 'dsla_fix', 'v_allkids', 'slas', 'v_eft', 'v_rank01', 'v_eft05'] if k in A]
    fig, ax = plt.subplots(figsize=(3.45, 0.28 * len(keys) + 0.5))
    y = np.arange(len(keys))[::-1]
    for yi, k in zip(y, keys):
        v = A[k]
        ax.plot([v['ci'][0], v['ci'][1]], [yi, yi], color=CAT[0], lw=1.6, solid_capstyle='round')
        ax.plot(v['mean'], yi, 'o', color=CAT[0], ms=4.5, mec='#fcfcfb', mew=0.8)
        ax.text(max(v['ci'][1], 0) + 0.15, yi, f"{v['mean']:.2f}", va='center', fontsize=6.5, color=INK2)
    ax.axvline(0, color=AXIS, lw=0.8, zorder=0)
    ax.set_yticks(y, [labels[k] for k in keys])
    ax.set_xlabel('Mean gain of DSLA over the variant (%)')
    ax.grid(axis='y', visible=False)
    ax.set_xlim(left=min(-0.3, min(A[k]['ci'][0] for k in keys) - 0.2), right=max(A[k]['ci'][1] for k in keys) + 1.1)
    save(fig, 'fig_ablation')


# ---------- Figure: construction time scaling
def timing():
    if not os.path.exists('timing/timing.json'):
        return
    T = json.load(open('timing/timing.json'))
    import collections
    cfgs = ['HEFT-equivalent (lambda=0, rank)', 'SLAS (rank build)', 'DSLA', 'DSLA, all successors']
    short = {'HEFT-equivalent (lambda=0, rank)': 'Rank build, no look-ahead', 'SLAS (rank build)': 'SLAS (rank build)',
             'DSLA': 'DSLA', 'DSLA, all successors': 'DSLA, all successors'}
    fig, axes = plt.subplots(1, 2, figsize=(7.1, 2.1))
    for ax, (st, var, fixed) in zip(axes, [('v', 'v', 16), ('p', 'p', 1000)]):
        for ci, c in enumerate(cfgs):
            pts = collections.defaultdict(list)
            for r in T:
                if r['config'] != c: continue
                if var == 'v' and r['p'] == 16 and r['set'] == 'v': pts[r['v']].append(r['sec'])
                if var == 'p' and r['v'] == 1000 and (r['set'] == 'p' or r['p'] == 16): pts[r['p']].append(r['sec'])
            xs = sorted(pts); ys = [1000 * np.mean(pts[x]) for x in xs]
            ax.plot(xs, ys, marker=MARK[ci], color=CAT[ci], ms=4, mec='#fcfcfb', mew=0.6, label=short[c])
        ax.set_xscale('log'); ax.set_yscale('log')
        ax.set_xlabel('Tasks $v$ ($p=16$)' if var == 'v' else 'Processors $p$ ($v=1000$)')
        xt = sorted({r[var] for r in T if (var == 'v' and r['set'] == 'v') or (var == 'p' and r['v'] == 1000)})
        ax.set_xticks(xt, [str(x) for x in xt]); ax.minorticks_off()
    axes[0].set_ylabel('Construction time (ms)')
    h, l = axes[0].get_legend_handles_labels()
    fig.legend(h, l, loc='upper center', ncol=4, frameon=False, bbox_to_anchor=(0.5, 1.07))
    fig.tight_layout(w_pad=1.5)
    save(fig, 'fig_time')


family_heatmap(); strata_lines(); ablation(); timing()
print('figures written')
