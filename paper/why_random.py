#!/usr/bin/env python3
"""why_random.py: why CGS's gains are smaller on the random DAGs than on the workflows.

  python3 paper/why_random.py <data_index.csv> <data_dir> <bin_dir> <runs.csv> <outputs_dir>

Needs <bin_dir>/evaluate and <bin_dir>/cgs_probe (tools/cgs_probe.cpp, CGS with closure counters).
Held-out test split only. Groups: the six workflow families pooled, and the random families by CCR.

Measures, per group
  structure   mean parents per non-entry task; share of non-entry tasks with 1-2 parents.
  closures    (cgs_probe, pooled over all closures in the group) parents per join; share of closures whose
              join ready time was already set by parents placed earlier; mean share of the join's ready time
              that the closure's parents add beyond what the earlier parents impose (0 when they add nothing);
              number of schedules that closures shorten / lengthen (cgs vs cgs_single).
  components  mean paired gain of CGS over cgs_single (joint placement) and over cgs_lam0 (table T in
              processor selection), as in results.json; share of schedules that closures change.
  headroom    bound = max(critical path with minimum costs, sum of minimum costs / p); it ignores
              communication, so it is informative only at CCR 0.1. Median gap of AEFT and of CGS to it, and
              median spread among the three strongest baselines (AEFT, PMMS, HEFT).

Writes  <outputs>/why_random/why_random.json, instances.csv, WHY_RANDOM.md and
        <outputs>/tables/tab_why_random_rows.tex, why_random_macros.tex (every number the text quotes).
Exits 1 if cgs_probe's makespan differs from cgs's on any instance (the probe must not change schedules).
"""
import hashlib, json, os, re, subprocess, sys, tempfile
from concurrent.futures import ProcessPoolExecutor
import numpy as np, pandas as pd

INDEX, DATA, BIN, RUNS, OUT = sys.argv[1:6]
WF = ['FFT', 'GE', 'Genome', 'LA', 'LIGO', 'Montage']
GROUPS = ['workflow', 'ccr0.1', 'ccr1', 'ccr10', 'random']
LABEL = {'workflow': 'Workflows', 'ccr0.1': 'Random, CCR 0.1', 'ccr1': 'Random, CCR 1',
         'ccr10': 'Random, CCR 10', 'random': 'Random, all'}
MACRO = {'workflow': 'Wf', 'ccr0.1': 'RLow', 'ccr1': 'RMid', 'ccr10': 'RHigh', 'random': 'Rand'}
TOP3 = ['aeft_ins', 'pmms_ins', 'heft_ins']
BASE = ['heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']


def is_test(path):
    return int(hashlib.sha256(path.encode()).hexdigest()[:8], 16) % 4 != 0


def group_of(family):
    if not family.startswith('R-'):
        return 'workflow'
    return 'ccr' + re.match(r'R-ccr([\d.]+)-', family).group(1)


def measure(path):
    with tempfile.TemporaryDirectory() as tmp:
        d3 = os.path.join(tmp, 'x.d3')
        subprocess.run([os.path.join(BIN, 'evaluate'), 'strip', os.path.join(DATA, path), d3], capture_output=True, check=True)
        a = np.fromfile(d3, sep=' ', dtype=np.int64)
        n, p = int(a[0]), int(a[1])
        W = a[2:2 + n * p].reshape(n, p)
        E = a[2 + n * p:2 + n * p + n * n].reshape(n, n) != -1
        with open(d3) as f:
            err = subprocess.run([os.path.join(BIN, 'cgs_probe')], stdin=f, capture_output=True, text=True, check=True).stderr
    indeg = E.sum(0)
    nonentry = indeg > 0
    probe = {k: float(v) for k, v in re.findall(r'(\w+)=([\d.]+)', err)}
    return dict(path=path, n=n, p=p, mean_parents=float(indeg[nonentry].mean()),
                share_1_2_parents=float(((indeg >= 1) & (indeg <= 2)).sum() / nonentry.sum()),
                load_bound=float(W.min(1).sum() / p), probe_makespan=int(probe['makespan']),
                closures=int(probe['closures']), preds=int(probe['preds']), rem=int(probe['rem']),
                bound=int(probe['bound']), slack=probe['slack'])


def main():
    idx = pd.read_csv(INDEX)
    idx = idx[[is_test(p) for p in idx.path]]
    with ProcessPoolExecutor(os.cpu_count()) as ex:
        inst = pd.DataFrame(list(ex.map(measure, idx.path, chunksize=8))).set_index('path')
    d = pd.read_csv(RUNS)
    d = d[d.status == 'ok']
    mk = d.pivot_table(index='path', columns='algo', values='makespan', aggfunc='first').loc[inst.index]
    lb = d.drop_duplicates('path').set_index('path').lb.loc[inst.index]
    diff = int((inst.probe_makespan != mk.cgs).sum())
    if diff:
        sys.exit(f'cgs_probe changed the schedule on {diff} instances; the probe must be a pure measurement copy')
    inst['family'] = idx.set_index('path').family.loc[inst.index]
    inst['group'] = inst.family.map(group_of)
    gain = lambda b: 100 * (mk[b] - mk.cgs) / mk[b]
    inst['gain_joint'], inst['gain_table'] = gain('cgs_single'), gain('cgs_lam0')
    inst['gain_aeft'], inst['gain_heft'] = gain('aeft_ins'), gain('heft_ins')
    inst['changed'] = (mk.cgs != mk.cgs_single).astype(float)
    inst['closures_shorten'] = (mk.cgs < mk.cgs_single).astype(int)
    inst['closures_lengthen'] = (mk.cgs > mk.cgs_single).astype(int)
    bound = np.maximum(lb, inst.load_bound)
    inst['aeft_gap'] = 100 * (mk.aeft_ins - bound) / mk.aeft_ins
    inst['cgs_gap'] = 100 * (mk.cgs - bound) / mk.cgs
    inst['best_gap'] = 100 * (mk[BASE].min(axis=1) - bound) / mk[BASE].min(axis=1)
    top = mk[TOP3]
    inst['top3_spread'] = 100 * (top.max(axis=1) - top.min(axis=1)) / top.min(axis=1)

    def summarise(g):
        c = int(g.closures.sum())
        return dict(instances=int(len(g)), mean_parents=float(g.mean_parents.mean()),
                    pct_tasks_1_2_parents=float(100 * g.share_1_2_parents.mean()),
                    closures=c, parents_per_join=float(g.preds.sum() / c),
                    pct_join_set_by_earlier_parents=float(100 * g.bound.sum() / c),
                    mean_ready_time_shift_pct=float(100 * g.slack.sum() / c),
                    pct_schedules_changed_by_closures=float(100 * g.changed.mean()),
                    schedules_shortened_by_closures=int(g.closures_shorten.sum()),
                    schedules_lengthened_by_closures=int(g.closures_lengthen.sum()),
                    gain_joint_placement=float(g.gain_joint.mean()), gain_table_in_selection=float(g.gain_table.mean()),
                    gain_vs_aeft=float(g.gain_aeft.mean()), gain_vs_heft=float(g.gain_heft.mean()),
                    median_aeft_gap_to_bound=float(g.aeft_gap.median()), median_cgs_gap_to_bound=float(g.cgs_gap.median()),
                    median_best_baseline_gap_to_bound=float(g.best_gap.median()),
                    median_top3_spread=float(g.top3_spread.median()))

    res = {'groups': {}, 'workflow_families': {}}
    for gname in GROUPS:
        g = inst[inst.group.str.startswith('ccr')] if gname == 'random' else inst[inst.group == gname]
        res['groups'][gname] = summarise(g)
    for f in WF:
        res['workflow_families'][f] = summarise(inst[inst.family == f])
    res['probe_schedules_identical'] = int(len(inst))

    os.makedirs(os.path.join(OUT, 'why_random'), exist_ok=True)
    os.makedirs(os.path.join(OUT, 'tables'), exist_ok=True)
    json.dump(res, open(os.path.join(OUT, 'why_random', 'why_random.json'), 'w'), indent=1)
    inst.drop(columns=['probe_makespan']).to_csv(os.path.join(OUT, 'why_random', 'instances.csv'))
    write_tables(res)
    print(f"why_random: {len(inst)} test instances, probe schedules identical to cgs on all")
    for gname in GROUPS:
        r = res['groups'][gname]
        print(f"  {gname:9s} parents {r['mean_parents']:.2f}  1-2 parents {r['pct_tasks_1_2_parents']:.1f}%  "
              f"set earlier {r['pct_join_set_by_earlier_parents']:.1f}%  shift {r['mean_ready_time_shift_pct']:.1f}%  "
              f"joint {r['gain_joint_placement']:.2f}  table {r['gain_table_in_selection']:.2f}  "
              f"vs AEFT {r['gain_vs_aeft']:.2f}  AEFT gap {r['median_aeft_gap_to_bound']:.2f}  top3 {r['median_top3_spread']:.2f}")


def _fmt(x, d):
    s = f'{x:.{d}f}'
    return (s[1:] if float(s) == 0 else s).replace('-', '$-$')   # no "-0.00"


def f1(x): return _fmt(x, 1)
def f2(x): return _fmt(x, 2)


def write_tables(res):
    G = res['groups']
    cols = [('mean_parents', f1), ('pct_tasks_1_2_parents', f1), ('pct_join_set_by_earlier_parents', f1),
            ('mean_ready_time_shift_pct', f1), ('gain_joint_placement', f2), ('gain_table_in_selection', f2),
            ('median_top3_spread', f2), ('gain_vs_aeft', f2)]
    rows = [f"{LABEL[g]} & {G[g]['instances']} & " + ' & '.join(fmt(G[g][k]) for k, fmt in cols) + ' \\\\'
            for g in ['workflow', 'ccr0.1', 'ccr1', 'ccr10']]
    open(os.path.join(OUT, 'tables', 'tab_why_random_rows.tex'), 'w').write('\n'.join(rows) + '\n')
    # one macro per quoted number: \WR<Group><Metric>, values formatted as in the table
    names = dict(mean_parents=('Parents', f1), pct_tasks_1_2_parents=('FewParents', f1),
                 parents_per_join=('JoinParents', f1), pct_join_set_by_earlier_parents=('SetEarlier', f1),
                 mean_ready_time_shift_pct=('Shift', f1), pct_schedules_changed_by_closures=('Changed', f1),
                 gain_joint_placement=('Joint', f2), gain_table_in_selection=('Table', f2), gain_vs_aeft=('VsAeft', f2),
                 gain_vs_heft=('VsHeft', f2), median_aeft_gap_to_bound=('AeftGap', f1),
                 median_best_baseline_gap_to_bound=('BestGap', f1), median_top3_spread=('Spread', f1),
                 schedules_shortened_by_closures=('Shorter', str), schedules_lengthened_by_closures=('Longer', str),
                 instances=('N', str))
    lines = ['% generated by paper/why_random.py; do not edit']
    for g in GROUPS:
        for k, (m, fmt) in names.items():
            lines.append(f"\\newcommand{{\\WR{MACRO[g]}{m}}}{{{fmt(G[g][k])}}}")
    fam = res['workflow_families']
    for k, m in (('mean_parents', 'Parents'), ('pct_tasks_1_2_parents', 'FewParents'),
                 ('pct_join_set_by_earlier_parents', 'SetEarlier'), ('mean_ready_time_shift_pct', 'Shift'),
                 ('gain_table_in_selection', 'Table'), ('median_top3_spread', 'Spread')):
        v = [fam[f][k] for f in WF]
        lines.append(f"\\newcommand{{\\WRWfMin{m}}}{{{f1(min(v))}}}")
        lines.append(f"\\newcommand{{\\WRWfMax{m}}}{{{f1(max(v))}}}")
    open(os.path.join(OUT, 'tables', 'why_random_macros.tex'), 'w').write('\n'.join(lines) + '\n')
    hdr = ['Group', 'Test instances', 'Parents per task', 'Tasks with 1–2 parents %', 'Join ready time set earlier %',
           'Ready time added by closure %', 'Joint placement gain %', 'Table-in-selection gain %',
           'Top-3 baseline spread % (median)', 'CGS vs AEFT %']
    md = ['# Why CGS gains less on the random DAGs (test split)', '',
          '| ' + ' | '.join(hdr) + ' |', '|:--|' + '--:|' * (len(hdr) - 1)]
    for r in rows:
        md.append('| ' + ' | '.join(c.replace('$-$', '−').strip(' \\') for c in r.split(' & ')) + ' |')
    md += ['', 'Per workflow family:', '', '| Family | ' + ' | '.join(hdr[1:]) + ' |', '|:--|' + '--:|' * (len(hdr) - 1)]
    for f in WF:
        md.append(f'| {f} | {fam[f]["instances"]} | ' + ' | '.join(fmt(fam[f][k]).replace('$-$', '−') for k, fmt in cols) + ' |')
    md += ['', 'Headroom (bound ignores communication; meaningful at CCR 0.1 only): median gap of AEFT / best baseline to the bound, '
           + '; '.join(f"{LABEL[g]} {f1(G[g]['median_aeft_gap_to_bound'])} / {f1(G[g]['median_best_baseline_gap_to_bound'])} %"
                       for g in ['workflow', 'ccr0.1', 'ccr1', 'ccr10']) + '.', '']
    open(os.path.join(OUT, 'why_random', 'WHY_RANDOM.md'), 'w').write('\n'.join(md))


if __name__ == '__main__':
    main()
