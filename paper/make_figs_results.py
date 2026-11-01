#!/usr/bin/env python3
"""Result figures of the CGS evaluation (held-out test instances; all methods with insertion).

  python3 make_figs_results.py <runs.csv> <out_dir>

fig_slr_tasks      mean SLR vs number of tasks, one panel per workflow family
fig_slr_procs      mean SLR vs number of processors, one panel per workflow family
fig_random         mean SLR of the random DAGs vs CCR, heterogeneity beta and size
fig_speedup        mean speedup vs processors (workflow and random instances)
fig_pairwise       share of instances on which CGS is better / equal / worse
fig_gain_dist      distribution of per-instance gains of CGS over each baseline
"""
import hashlib, os, sys
import numpy as np, pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

RUNS, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)
ALG = ['cgs', 'heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']
NAME = dict(cgs='CGS', heft_ins='HEFT', peft_ins='PEFT', ippts_ins='IPPTS', aeft_ins='AEFT', pmms_ins='PMMS', bppts_ins='BPPTS',
            la_heft='Lookahead HEFT')
COL = dict(cgs='#2a78d6', heft_ins='#eb6834', peft_ins='#1baf7a', ippts_ins='#eda100', aeft_ins='#e87ba4', pmms_ins='#008300',
           bppts_ins='#6250d6', la_heft='#8a5a3c')
MRK = dict(cgs='o', heft_ins='s', peft_ins='^', ippts_ins='v', aeft_ins='D', pmms_ins='P', bppts_ins='X', la_heft='*')
INK2, AXIS, GRID = '#52514e', '#c3c2b7', '#e1e0d9'
plt.rcParams.update({
    'font.family': 'serif', 'font.serif': ['DejaVu Serif'], 'font.size': 7.5, 'axes.titlesize': 7.5,
    'axes.labelsize': 7.5, 'xtick.labelsize': 6.8, 'ytick.labelsize': 6.8, 'legend.fontsize': 7,
    'axes.edgecolor': AXIS, 'axes.labelcolor': INK2, 'xtick.color': INK2, 'ytick.color': INK2,
    'axes.spines.top': False, 'axes.spines.right': False, 'axes.grid': True, 'grid.color': GRID,
    'grid.linewidth': 0.5, 'axes.axisbelow': True, 'pdf.fonttype': 42, 'lines.linewidth': 1.2})

d = pd.read_csv(RUNS)
d = d[(d.status == 'ok') & d.algo.isin(ALG)].copy()
d['test'] = [int(hashlib.sha256(p.encode()).hexdigest()[:8], 16) % 4 != 0 for p in d.path]
d = d[d.test]
assert d.groupby('path').algo.nunique().eq(len(ALG)).all()
d['random'] = d.family.str.startswith('R-')
fam = d.family.str.extract(r'R-ccr([\d.]+)-b([\d.]+)')
d['ccr'], d['beta'] = fam[0], fam[1]


def save(fig, name):
    fig.savefig(os.path.join(OUT, name + '.pdf'), bbox_inches='tight', pad_inches=0.02)
    fig.savefig(os.path.join(OUT, name + '.png'), bbox_inches='tight', pad_inches=0.02, dpi=200)
    plt.close(fig)


def line(ax, x, y, a):
    ax.plot(x, y, color=COL[a], marker=MRK[a], ms=3.6 if a != 'cgs' else 4.4, mec='#fcfcfb', mew=0.5,
            lw=1.9 if a == 'cgs' else 1.0, zorder=3 if a == 'cgs' else 2, label=NAME[a])


def legend(fig, axes, y=1.04):
    h, l = axes.flat[0].get_legend_handles_labels()
    fig.legend(h, l, loc='upper center', ncol=8, frameon=False, bbox_to_anchor=(0.5, y), handlelength=2.2)


FAMS = ['LIGO', 'Montage', 'Genome', 'FFT', 'GE', 'LA']


def per_family(xcol, xlabel, name, logx=False):
    w = d[~d.random]
    fig, axes = plt.subplots(2, 3, figsize=(7.1, 4.0))
    for ax, f in zip(axes.flat, FAMS):
        g = w[w.family == f].groupby([xcol, 'algo']).slr.mean().unstack()
        for a in ALG:
            line(ax, np.arange(len(g)), g[a].to_numpy(), a)
        ax.set_xticks(np.arange(len(g)), [str(int(x)) for x in g.index])
        ax.set_title(f, color='#0b0b0b', pad=3)
        ax.set_xlabel(xlabel)
        if xcol == 'nodes' and len(g) > 6:
            for lab in ax.get_xticklabels()[1::2]: lab.set_visible(False)
    for ax in axes[:, 0]: ax.set_ylabel('Mean SLR')
    legend(fig, axes, 1.03)
    fig.tight_layout(h_pad=1.0, w_pad=0.8)
    save(fig, name)


per_family('nodes', 'Number of tasks', 'fig_slr_tasks')
per_family('p', 'Number of processors', 'fig_slr_procs')

# random DAGs
r = d[d.random]
fig, axes = plt.subplots(1, 3, figsize=(7.1, 2.05))
for ax, (col, lab, order) in zip(axes, [('ccr', 'CCR', ['0.1', '1', '10']), ('beta', 'Heterogeneity $\\beta$', ['0.1', '0.5', '1']),
                                       ('nodes', 'Number of tasks', [100, 300, 500, 1000])]):
    g = r.groupby([col, 'algo']).slr.mean().unstack().loc[order]
    for a in ALG:
        line(ax, np.arange(len(order)), g[a].to_numpy(), a)
    ax.set_xticks(np.arange(len(order)), [str(x) for x in order])
    ax.set_xlabel(lab)
    if col == 'ccr':
        from matplotlib.ticker import FixedLocator, FixedFormatter, NullLocator
        ax.set_yscale('log'); ax.yaxis.set_major_locator(FixedLocator([2, 3, 5, 10]))
        ax.yaxis.set_major_formatter(FixedFormatter(['2', '3', '5', '10'])); ax.yaxis.set_minor_locator(NullLocator())
axes[0].set_ylabel('Mean SLR')
legend(fig, axes, 1.1)
fig.tight_layout(w_pad=1.0)
save(fig, 'fig_random')

# speedup vs processors
fig, axes = plt.subplots(1, 2, figsize=(7.1, 2.05))
for ax, (sub, title) in zip(axes, [(d[~d.random], 'Workflow instances'), (d[d.random], 'Random instances')]):
    g = sub.groupby(['p', 'algo']).speedup.mean().unstack()
    for a in ALG:
        line(ax, np.arange(len(g)), g[a].to_numpy(), a)
    ax.set_xticks(np.arange(len(g)), [str(int(x)) for x in g.index])
    ax.set_xlabel('Number of processors'); ax.set_title(title, color='#0b0b0b', pad=3)
axes[0].set_ylabel('Mean speedup')
legend(fig, axes, 1.1)
fig.tight_layout(w_pad=1.5)
save(fig, 'fig_speedup')

# pairwise better / equal / worse and gain distribution
mk = d.pivot_table(index='path', columns='algo', values='makespan')
BASE = ALG[1:]
better = [(mk.cgs < mk[b]).mean() * 100 for b in BASE]
equal = [(mk.cgs == mk[b]).mean() * 100 for b in BASE]
worse = [(mk.cgs > mk[b]).mean() * 100 for b in BASE]
fig, axes = plt.subplots(1, 2, figsize=(7.1, 2.45), gridspec_kw=dict(width_ratios=[1, 1.15]))
ax = axes[0]
y = np.arange(len(BASE))[::-1]
ax.barh(y, better, color='#2a78d6', height=0.62, label='CGS better', edgecolor='#fcfcfb', linewidth=1)
ax.barh(y, equal, left=better, color='#c3c2b7', height=0.62, label='Equal', edgecolor='#fcfcfb', linewidth=1)
ax.barh(y, worse, left=np.add(better, equal), color='#eb6834', height=0.62, label='CGS worse', edgecolor='#fcfcfb', linewidth=1)
for yi, b, wv in zip(y, better, worse):
    ax.text(b / 2, yi, f'{b:.1f}', ha='center', va='center', color='#ffffff', fontsize=6.5)
    if wv >= 6:
        ax.text(100 - wv / 2, yi, f'{wv:.1f}', ha='center', va='center', color='#ffffff', fontsize=6.5)
ax.set_yticks(y, [NAME[b] for b in BASE]); ax.set_xlim(0, 100); ax.set_xlabel('Share of test instances (%)')
ax.grid(axis='y', visible=False)
ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.2), ncol=3, frameon=False, handlelength=1.2)
ax = axes[1]
gains = [100 * (mk[b] - mk.cgs) / mk[b] for b in BASE]
bp = ax.boxplot(gains, orientation='horizontal', positions=y, widths=0.55, showfliers=False, patch_artist=True,
                medianprops=dict(color='#0b0b0b', lw=1.1), whiskerprops=dict(color=INK2, lw=0.8),
                capprops=dict(color=INK2, lw=0.8), boxprops=dict(lw=0.8, color=INK2))
for patch in bp['boxes']: patch.set_facecolor('#9ec5f4')
for yi, gv in zip(y, gains):
    ax.plot(gv.mean(), yi, 'o', color='#2a78d6', ms=3.8, mec='#fcfcfb', mew=0.6, zorder=4)
ax.axvline(0, color=AXIS, lw=0.8, zorder=0)
ax.set_yticks(y, [NAME[b] for b in BASE]); ax.set_xlabel('Gain of CGS per instance (%)')
ax.grid(axis='y', visible=False)
ax.set_title('Box: quartiles; whiskers: 1.5 IQR; dot: mean', color=INK2, pad=3, fontsize=6.8)
fig.tight_layout(w_pad=2.0)
save(fig, 'fig_pairwise')
print('result figures written')
