#!/usr/bin/env python3
"""Worked example (Section 6): tables and Gantt charts for one small instance.

  python3 worked_example.py <instance.d3> <bin_dir> <evaluate> <out_dir> [<original.d3>]

<bin_dir> holds compiled heft, peft, ippts, aeft, pmms, bppts and dsla (dsla.cpp).
Writes example.json, tab_ex_static.tex (rank, I, K), tab_ex_trace.tex (DSLA decisions),
tab_ex_makespan.tex and fig_ex_gantt.pdf. Every schedule is validated with tools/evaluate.
"""
import json, os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from port import dsla, read_d3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

inst, bindir, tool, out = sys.argv[1:5]
orig = read_d3(open(sys.argv[5]).read())[2] if len(sys.argv) > 5 else None
os.makedirs(out, exist_ok=True)
n, p, W, C = read_d3(open(inst).read())
ALG = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts', 'dsla']
NAME = dict(dsla='DSLA', heft='HEFT', peft='PEFT', ippts='IPPTS', aeft='AEFT', pmms='PMMS', bppts='BPPTS')

# ---- schedules of all algorithms, validated
sched = {}
for a in ALG:
    o = subprocess.run([os.path.join(bindir, a)], stdin=open(inst), capture_output=True, text=True, check=True).stdout
    of = os.path.join(out, f'{a}.out'); open(of, 'w').write(o)
    chk = json.loads(subprocess.run([tool, 'check', inst, of], capture_output=True, text=True, check=True).stdout)
    assert chk['valid'], a
    rows = [l.split() for l in o.split('\n') if l.strip() and l.split()[0].isdigit() and len(l.split()) == 4]
    sched[a] = dict(makespan=chk['makespan'], tasks=[tuple(int(x) for x in r) for r in rows])
    os.remove(of)

# ---- static information (same recurrences as dsla.cpp)
succ = [[(j, C[i][j]) for j in range(n) if C[i][j] != -1] for i in range(n)]
pred = [[i for i in range(n) if C[i][j] != -1] for j in range(n)]
indeg = [len(x) for x in pred]; topo = []; q = [i for i in range(n) if not indeg[i]]
while q:
    t = q.pop(0); topo.append(t)
    for s, _ in succ[t]:
        indeg[s] -= 1
        if not indeg[s]: q.append(s)
I = [[0] * p for _ in range(n)]; mn = [0] * n
for t in reversed(topo):
    for k in range(p):
        I[t][k] = W[t][k] + max([min(I[c][k], mn[c] + cc) for c, cc in succ[t]], default=0)
    mn[t] = min(I[t])
rnk = [0.0] * n
for t in reversed(topo):
    rnk[t] = sum(W[t]) / p + max([cc + rnk[c] for c, cc in succ[t]], default=0)
mio = [sum(I[t]) for t in range(n)]
K = [[c for c, _ in sorted(succ[t], key=lambda x: -mio[x[0]])[:3]] for t in range(n)]

# ---- DSLA traces of both builds
tr_r, tr_e = [], []
mk_r = dsla(n, p, W, C, mode=0, trace=tr_r)[0]
mk_e = dsla(n, p, W, C, mode=1, trace=tr_e)[0]
assert min(mk_r, mk_e) == sched['dsla']['makespan']
chosen = 'rank' if mk_r <= mk_e else 'eft'
trace, lam = (tr_r, 0.5) if chosen == 'rank' else (tr_e, 0.1)
for x in trace:
    t = x['task']
    x['L'] = None if x['score'] is None else [round((x['score'][k] - (1 - lam) * x['eft'][k]) / lam - W[t][k], 6) for k in range(p)]
# HEFT choice under DSLA's own partial schedule, for comparison
for x in trace:
    x['eft_choice'] = min(range(p), key=lambda k: (x['eft'][k], k))

json.dump(dict(W=W, rank=rnk, I=I, K=K, makespans={a: sched[a]['makespan'] for a in ALG},
               dsla_builds=dict(rank=mk_r, eft=mk_e, chosen=chosen), trace=trace), open(os.path.join(out, 'example.json'), 'w'), indent=1)

# ---- LaTeX tables
def fmt(v): return f'{v:.2f}'.rstrip('0').rstrip('.') if isinstance(v, float) else str(v)
rows = []
for t in sorted(range(n), key=lambda t: -rnk[t]):
    mark = lambda k: (r'$^\dagger$' if orig is not None and orig[t][k] != W[t][k] else '')
    rows.append(f"$T_{{{t+1}}}$ & " + ' & '.join(f'{W[t][k]}{mark(k)}' for k in range(p)) + f" & {rnk[t]:.2f} & " +
                ' & '.join(str(I[t][k]) for k in range(p)) + ' & ' + (', '.join(f'$T_{{{c+1}}}$' for c in K[t]) or '--') + r' \\')
open(os.path.join(out, 'tab_ex_static.tex'), 'w').write('\n'.join(rows) + '\n')
rows = []
for x in trace:
    t = x['task']
    eft = ', '.join(str(v) for v in x['eft'])
    if x['score'] is None:
        L = sc = '--'
    else:
        L = ', '.join(fmt(v) for v in x['L'])
        sc = ', '.join(fmt(round(v, 2)) for v in x['score'])
    mark = r'$^{\ast}$' if x['choice'] != x['eft_choice'] else ''
    rows.append(f"$T_{{{t+1}}}$ & {eft} & {L} & {sc} & $P_{{{x['choice']+1}}}${mark} & {x['eft'][x['choice']]} " + r'\\')
open(os.path.join(out, 'tab_ex_trace.tex'), 'w').write('\n'.join(rows) + '\n')
open(os.path.join(out, 'tab_ex_makespan.tex'), 'w').write(
    ' & '.join(NAME[a] for a in ALG) + r' \\' + '\n\\midrule\n' + ' & '.join(str(sched[a]['makespan']) for a in ALG) + r' \\' + '\n')

# ---- Gantt charts
plt.rcParams.update({'font.family': 'serif', 'font.serif': ['DejaVu Serif'], 'font.size': 7, 'pdf.fonttype': 42,
                     'axes.edgecolor': '#c3c2b7', 'xtick.color': '#52514e', 'ytick.color': '#52514e'})
show = ['dsla', 'heft', 'aeft', 'pmms']
mx = max(sched[a]['makespan'] for a in show)
fig, axes = plt.subplots(len(show), 1, figsize=(3.45, 0.62 * len(show) * p / 3 + 0.5), sharex=True)
for ax, a in zip(axes, show):
    for (t, s, f, pr) in sched[a]['tasks']:
        ax.barh(pr, f - s, left=s, height=0.72, color='#2a78d6' if a == 'dsla' else '#9ec5f4',
                edgecolor='#fcfcfb', linewidth=1.2)
        if f - s >= 4:
            ax.text((s + f) / 2, pr, f'$T_{{{t}}}$', ha='center', va='center', fontsize=6,
                    color='#ffffff' if a == 'dsla' else '#0b0b0b')
    ax.axvline(sched[a]['makespan'], color='#eb6834' if a != 'dsla' else '#0b0b0b', lw=0.9, ls='--')
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
print('makespans', {a: sched[a]['makespan'] for a in ALG}, 'DSLA builds', mk_r, mk_e)
