#!/usr/bin/env python3
"""Worked examples: makespans of every method on three small DAGs, and Gantt charts on the classic one.

  python3 worked_example.py <bin_dir> <evaluate> <out_dir> <examples_dir>

<bin_dir> holds compiled cgs and the insertion baselines (heft_ins, peft_ins, ippts_ins, aeft_ins, pmms_ins,
bppts_ins, la_heft). Examples (paper/examples):
  classic_topcuoglu.d3     the ten-task DAG of Topcuoglu et al. (TPDS 2002), which is also Fig. 1 / Table 3 of the
                           AEFT paper (published: HEFT 80, AEFT 76)
  lookahead_heft_fig1.d3   Lookahead HEFT paper, Fig. 1 (published: HEFT 260, Lookahead HEFT 184)
Writes example.json, tab_ex_makespan.tex and fig_ex_gantt.pdf/png. Every schedule is validated with tools/evaluate.
"""
import json, os, subprocess, sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

bindir, tool, out, exdir = sys.argv[1:5]
os.makedirs(out, exist_ok=True)
ALG = ['cgs', 'heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']
NAME = dict(cgs='CGS', heft_ins='HEFT', peft_ins='PEFT', ippts_ins='IPPTS', aeft_ins='AEFT', pmms_ins='PMMS',
            bppts_ins='BPPTS', la_heft='Lookahead HEFT')
EX = dict(classic_topcuoglu='Topcuoglu et al. (classic)', lookahead_heft_fig1='Lookahead HEFT paper, Fig. 1')


def run(a, inst):
    o = subprocess.run([os.path.join(bindir, a)], stdin=open(inst), capture_output=True, text=True, check=True).stdout
    of = os.path.join(out, f'{a}.out'); open(of, 'w').write(o)
    chk = json.loads(subprocess.run([tool, 'check', inst, of], capture_output=True, text=True, check=True).stdout)
    os.remove(of)
    assert chk['valid'], (a, inst)
    rows = [l.split() for l in o.split('\n') if l.strip() and l.split()[0].isdigit() and len(l.split()) == 4]
    return dict(makespan=chk['makespan'], tasks=[tuple(int(x) for x in r) for r in rows])


res = {e: {a: run(a, os.path.join(exdir, e + '.d3')) for a in ALG} for e in EX}
json.dump({e: {a: res[e][a]['makespan'] for a in ALG} for e in EX}, open(os.path.join(out, 'example.json'), 'w'), indent=1)
open(os.path.join(out, 'tab_ex_makespan.tex'), 'w').write(
    '\n'.join(f"{EX[e]} & " + ' & '.join(str(res[e][a]['makespan']) for a in ALG) + r' \\' for e in EX) + '\n')

# Gantt charts on the classic DAG
sched = res['classic_topcuoglu']
p = max(t[3] for a in ALG for t in sched[a]['tasks'])
plt.rcParams.update({'font.family': 'serif', 'font.serif': ['DejaVu Serif'], 'font.size': 7, 'pdf.fonttype': 42,
                     'axes.edgecolor': '#c3c2b7', 'xtick.color': '#52514e', 'ytick.color': '#52514e'})
show = ['cgs', 'heft_ins', 'aeft_ins', 'la_heft']
mx = max(sched[a]['makespan'] for a in show)
fig, axes = plt.subplots(len(show), 1, figsize=(3.45, 0.62 * len(show) * p / 3 + 0.5), sharex=True)
for ax, a in zip(axes, show):
    for (t, s, f, pr) in sched[a]['tasks']:
        ax.barh(pr, f - s, left=s, height=0.72, color='#2a78d6' if a == 'cgs' else '#9ec5f4',
                edgecolor='#fcfcfb', linewidth=1.2)
        if f - s >= 4:
            ax.text((s + f) / 2, pr, f'$T_{{{t}}}$', ha='center', va='center', fontsize=6,
                    color='#ffffff' if a == 'cgs' else '#0b0b0b')
    ax.axvline(sched[a]['makespan'], color='#eb6834' if a != 'cgs' else '#0b0b0b', lw=0.9, ls='--')
    ax.text(sched[a]['makespan'] + 0.8, p + 0.15, str(sched[a]['makespan']), fontsize=6.5, va='center')
    ax.set_yticks(range(1, p + 1), [f'$P_{k}$' for k in range(1, p + 1)])
    ax.set_ylim(0.4, p + 0.6); ax.invert_yaxis()
    ax.set_xlim(0, mx * 1.08)
    ax.set_ylabel(NAME[a], rotation=0, ha='right', va='center', labelpad=14, color='#0b0b0b')
    for sp in ('top', 'right'): ax.spines[sp].set_visible(False)
    ax.grid(axis='x', color='#e1e0d9', lw=0.5); ax.set_axisbelow(True)
axes[-1].set_xlabel('Time')
fig.tight_layout(h_pad=0.4)
fig.savefig(os.path.join(out, 'fig_ex_gantt.pdf'), bbox_inches='tight', pad_inches=0.02)
fig.savefig(os.path.join(out, 'fig_ex_gantt.png'), bbox_inches='tight', pad_inches=0.02, dpi=200)
for e in EX:
    print(e, {a: res[e][a]['makespan'] for a in ALG})
