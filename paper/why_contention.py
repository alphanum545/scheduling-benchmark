#!/usr/bin/env python3
"""why_contention.py: what the network-contention replay measures, and why CGS loses ground under it.

  python3 paper/why_contention.py <data_index.csv> <data_dir> <bin_dir> <runs.csv> <outputs_dir>

Re-runs CGS (and its single / nogate variants) and the seven insertion baselines on every test instance,
validates each schedule with <bin_dir>/evaluate and exits 1 unless its makespan and contention makespan equal
runs.csv. Also runs CGS without closures at selection weights 1 and 2 (weight 0 is HEFT with insertion,
0.5 is cgs_single), to separate the effect of the weight from the table; these are validated but are not in
runs.csv, and are excluded from the inflation range and the correlations. For each schedule it then computes, from the one-port model of tools/evaluate.cpp (each processor
sends one and receives one message at a time; a remote message occupies both ports for its whole cost):

  remote volume   total cost of messages between different processors
  port bound      the largest total message cost on any one in- or out-port; a lower bound on the
                  contention makespan, since a port carries one message at a time
  in-port concentration   busiest in-port load / mean in-port load over receiving processors

Per group (the six workflow families pooled and singly; random DAGs by CCR) it reports how much each method
inflates under contention, how often the busiest port alone exceeds the planned makespan, contention makespan
over the port bound, the within-instance correlation (across the nine schedules) of the contention makespan
with the port bound and with the planned makespan, each method's mean difference from AEFT in planned
makespan, remote volume, port bound and contention makespan, and how CGS's contention losses to AEFT split
by whether its busiest port is heavier.

Writes  <outputs>/why_contention/why_contention.json, schedules.csv, WHY_CONTENTION.md and
        <outputs>/tables/tab_contention_rows.tex, contention_macros.tex (every number the text quotes).
"""
import hashlib, json, os, subprocess, sys, tempfile
from concurrent.futures import ProcessPoolExecutor
import numpy as np, pandas as pd

INDEX, DATA, BIN, RUNS, OUT = sys.argv[1:6]
ALG = {'cgs': ['cgs'], 'cgs_single': ['cgs', 'single'], 'cgs_nogate': ['cgs', 'nogate'],
       'aeft_ins': ['aeft_ins'], 'heft_ins': ['heft_ins'], 'pmms_ins': ['pmms_ins'], 'peft_ins': ['peft_ins'],
       'la_heft': ['la_heft'], 'bppts_ins': ['bppts_ins'],
       # selection-weight sensitivity (no closures); weight 0 is HEFT with insertion, weight 0.5 is cgs_single
       'cgs_single_lam1': ['cgs', 'single', 'lam=1'], 'cgs_single_lam2': ['cgs', 'single', 'lam=2']}
MAIN = ['cgs', 'cgs_single', 'cgs_nogate', 'aeft_ins', 'heft_ins', 'pmms_ins', 'peft_ins', 'la_heft', 'bppts_ins']
NAME = dict(cgs='CGS', cgs_single='CGS without joint placement', cgs_nogate='CGS with ungated closures',
            aeft_ins='AEFT', heft_ins='HEFT', pmms_ins='PMMS', peft_ins='PEFT', la_heft='Lookahead HEFT',
            bppts_ins='BPPTS', cgs_single_lam1='\\quad selection weight 1', cgs_single_lam2='\\quad selection weight 2')
KEY = dict(cgs='Cgs', cgs_single='CgsSingle', cgs_nogate='CgsNogate', aeft_ins='Aeft', heft_ins='Heft',
           pmms_ins='Pmms', peft_ins='Peft', la_heft='Lah', bppts_ins='Bppts', cgs_single_lam1='WeightOne',
           cgs_single_lam2='WeightTwo')
WF = ['FFT', 'GE', 'Genome', 'LA', 'LIGO', 'Montage']
GROUPS = ['workflow'] + WF + ['ccr0.1', 'ccr1', 'ccr10']
GKEY = dict(workflow='Wf', FFT='Fft', GE='Ge', Genome='Genome', LA='La', LIGO='Ligo', Montage='Montage',
            **{'ccr0.1': 'RLow', 'ccr1': 'RMid', 'ccr10': 'RHigh'})
REF = 'aeft_ins'


def is_test(path):
    return int(hashlib.sha256(path.encode()).hexdigest()[:8], 16) % 4 != 0


def parse(text, n):
    """Schedule rows as tools/evaluate.cpp reads them: 4 integers on lines after the first containing a letter."""
    pr = np.full(n, -1)
    header = False
    for line in text.splitlines():
        if not header:
            header = any(ch.isalpha() for ch in line)
            continue
        tok = line.split()
        if len(tok) < 4:
            continue
        try:
            i, _, _, q = map(int, tok[:4])
        except ValueError:
            continue
        if 1 <= i <= n:
            pr[i - 1] = q - 1
    return pr


def measure(path):
    rows = []
    with tempfile.TemporaryDirectory() as tmp:
        d3, sched = os.path.join(tmp, 'x.d3'), os.path.join(tmp, 's.txt')
        subprocess.run([os.path.join(BIN, 'evaluate'), 'strip', os.path.join(DATA, path), d3], capture_output=True, check=True)
        a = np.fromfile(d3, sep=' ', dtype=np.int64)
        n, p = int(a[0]), int(a[1])
        C = a[2 + n * p:2 + n * p + n * n].reshape(n, n)
        U, V = np.nonzero(C != -1)
        c = C[U, V]
        for algo, cmd in ALG.items():
            with open(d3) as f:
                out = subprocess.run([os.path.join(BIN, cmd[0])] + cmd[1:], stdin=f, capture_output=True, text=True, check=True).stdout
            open(sched, 'w').write(out)
            ev = json.loads(subprocess.run([os.path.join(BIN, 'evaluate'), 'check', d3, sched],
                                           capture_output=True, text=True, check=True).stdout)
            if not ev.get('valid'):
                raise SystemExit(f'invalid schedule: {algo} on {path}')
            pr = parse(out, n)
            rem = (pr[U] != pr[V]) & (c > 0)
            inl = np.bincount(pr[V][rem], weights=c[rem], minlength=p)
            outl = np.bincount(pr[U][rem], weights=c[rem], minlength=p)
            rows.append(dict(path=path, algo=algo, makespan=int(ev['makespan']), cont_mk=int(ev['cont_mk']),
                             remote_vol=float(c[rem].sum()), port_bound=float(max(inl.max(), outl.max())),
                             in_conc=float(inl.max() / inl[inl > 0].mean()) if (inl > 0).any() else 1.0))
    return rows


def group_of(family):
    if not family.startswith('R-'):
        return family
    return 'ccr' + family.split('-')[1][3:]


def main():
    idx = pd.read_csv(INDEX)
    idx = idx[[is_test(p) for p in idx.path]]
    with ProcessPoolExecutor(os.cpu_count()) as ex:
        s = pd.DataFrame([r for chunk in ex.map(measure, idx.path, chunksize=4) for r in chunk])
    runs = pd.read_csv(RUNS)
    runs = runs[runs.status == 'ok'].set_index(['path', 'algo'])
    k = s[s.algo.isin(MAIN)].set_index(['path', 'algo'])   # weight variants are not in runs.csv
    ref = runs.reindex(k.index)
    bad = int(((ref.makespan != k.makespan) | (ref.cont_mk != k.cont_mk)).sum())
    if bad:
        sys.exit(f'{bad} re-run schedules differ from runs.csv (makespan or cont_mk)')
    fam = idx.set_index('path').family
    s['family'] = s.path.map(fam)
    s['group'] = s.family.map(group_of)

    def members(g):
        return s[~s.family.str.startswith('R-')] if g == 'workflow' else s[s.group == g]

    res = {'schedules_checked': int(len(s)), 'groups': {}}
    for g in GROUPS:
        m = members(g)
        P = lambda col: m.pivot(index='path', columns='algo', values=col)
        mk, cm, pb, rv = P('makespan'), P('cont_mk'), P('port_bound'), P('remote_vol')
        per = {}
        for a in ALG:
            pos = pb[a] > 0
            diff = lambda t: float((100 * (t[a] - t[REF]) / t[REF]).mean())
            per[a] = dict(inflation=float((cm[a] / mk[a]).mean()),
                          pct_port_exceeds_plan=float(100 * (pb[a] > mk[a]).mean()),
                          median_cont_over_port=float((cm[a][pos] / pb[a][pos]).median()),
                          median_in_conc=float(P('in_conc')[a].median()),
                          diff_planned=diff(mk), diff_remote_vol=diff(rv), diff_port_bound=diff(pb), diff_cont=diff(cm),
                          gain_cgs_cont=float((100 * (cm[a] - cm.cgs) / cm[a]).mean()),
                          gain_cgs_planned=float((100 * (mk[a] - mk.cgs) / mk[a]).mean()))
        lm, lc, lp = np.log(mk[MAIN]), np.log(cm[MAIN]), np.log(pb[MAIN].where(pb[MAIN] > 0))
        corr_port, corr_plan = [], []
        for path in mk.index:
            x, y, z = lc.loc[path], lp.loc[path], lm.loc[path]
            if y.notna().all() and y.std() > 0 and x.std() > 0:
                corr_port.append(float(np.corrcoef(x, y)[0, 1]))
            if z.std() > 0 and x.std() > 0:
                corr_plan.append(float(np.corrcoef(x, z)[0, 1]))
        lose, heavy = cm.cgs > cm[REF], pb.cgs > pb[REF]
        res['groups'][g] = dict(
            instances=int(len(mk)), per_algo=per,
            inflation_min=float(min(per[a]['inflation'] for a in MAIN)),
            inflation_max=float(max(per[a]['inflation'] for a in MAIN)),
            median_corr_cont_port=float(np.median(corr_port)), median_corr_cont_planned=float(np.median(corr_plan)),
            cgs_loses_to_aeft=int(lose.sum()),
            pct_losses_with_heavier_port=float(100 * heavy[lose].mean()) if lose.any() else 0.0,
            pct_lose_when_heavier=float(100 * lose[heavy].mean()) if heavy.any() else 0.0,
            pct_lose_when_lighter=float(100 * lose[~heavy].mean()) if (~heavy).any() else 0.0)

    os.makedirs(os.path.join(OUT, 'why_contention'), exist_ok=True)
    os.makedirs(os.path.join(OUT, 'tables'), exist_ok=True)
    json.dump(res, open(os.path.join(OUT, 'why_contention', 'why_contention.json'), 'w'), indent=1)
    s.to_csv(os.path.join(OUT, 'why_contention', 'schedules.csv'), index=False)
    write_tables(res)
    w = res['groups']['workflow']
    print(f"why_contention: {len(s)} schedules re-run; the {int(s.algo.isin(MAIN).sum())} also in runs.csv equal it")
    print(f"  workflows: inflation {w['inflation_min']:.2f}-{w['inflation_max']:.2f}x; corr(cont, port) {w['median_corr_cont_port']:.2f}, "
          f"corr(cont, planned) {w['median_corr_cont_planned']:.2f}; CGS vs AEFT planned {w['per_algo']['cgs']['diff_planned']:.2f}%, "
          f"remote vol {w['per_algo']['cgs']['diff_remote_vol']:.2f}%, port {w['per_algo']['cgs']['diff_port_bound']:.2f}%, "
          f"contention {w['per_algo']['cgs']['diff_cont']:.2f}%")


def _fmt(x, d):
    s = f'{x:.{d}f}'
    return (s[1:] if float(s) == 0 else s).replace('-', '$-$')


def f1(x): return _fmt(x, 1)
def f2(x): return _fmt(x, 2)


def write_tables(res):
    G = res['groups']
    w, r10 = G['workflow']['per_algo'], G['ccr10']['per_algo']
    order = ['cgs', 'cgs_single', 'cgs_single_lam1', 'cgs_single_lam2', 'cgs_nogate', 'aeft_ins', 'pmms_ins',
             'heft_ins', 'peft_ins', 'la_heft', 'bppts_ins']
    rows = []
    for a in order:
        cells = [w[a][k] for k in ('diff_planned', 'diff_remote_vol', 'diff_port_bound', 'diff_cont')] + \
                [r10[a][k] for k in ('diff_remote_vol', 'diff_cont')]
        body = ' & '.join('---' if a == REF else f2(v) for v in cells)
        rows.append(f"{NAME[a]} & {w[a]['inflation']:.2f} & {body} \\\\")
    open(os.path.join(OUT, 'tables', 'tab_contention_rows.tex'), 'w').write('\n'.join(rows) + '\n')
    L = ['% generated by paper/why_contention.py; do not edit']
    for g in GROUPS:
        gr, gk = G[g], GKEY[g]
        L += [f"\\newcommand{{\\WC{gk}N}}{{{gr['instances']}}}",
              f"\\newcommand{{\\WC{gk}InflMin}}{{{gr['inflation_min']:.1f}}}",
              f"\\newcommand{{\\WC{gk}InflMax}}{{{gr['inflation_max']:.1f}}}",
              f"\\newcommand{{\\WC{gk}CorrPort}}{{{f2(gr['median_corr_cont_port'])}}}",
              f"\\newcommand{{\\WC{gk}CorrPlan}}{{{f2(gr['median_corr_cont_planned'])}}}",
              f"\\newcommand{{\\WC{gk}Lose}}{{{gr['cgs_loses_to_aeft']}}}",
              f"\\newcommand{{\\WC{gk}LoseHeavy}}{{{f1(gr['pct_losses_with_heavier_port'])}}}",
              f"\\newcommand{{\\WC{gk}LoseIfHeavy}}{{{f1(gr['pct_lose_when_heavier'])}}}",
              f"\\newcommand{{\\WC{gk}LoseIfLight}}{{{f1(gr['pct_lose_when_lighter'])}}}"]
        for a, v in gr['per_algo'].items():
            ak = KEY[a]
            L += [f"\\newcommand{{\\WC{gk}{ak}Infl}}{{{v['inflation']:.1f}}}",
                  f"\\newcommand{{\\WC{gk}{ak}PortExceeds}}{{{f1(v['pct_port_exceeds_plan'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}ContOverPort}}{{{f2(v['median_cont_over_port'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}InConc}}{{{f2(v['median_in_conc'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}Planned}}{{{f2(v['diff_planned'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}Vol}}{{{f2(v['diff_remote_vol'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}Port}}{{{f2(v['diff_port_bound'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}Cont}}{{{f2(v['diff_cont'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}GainCont}}{{{f2(v['gain_cgs_cont'])}}}",
                  f"\\newcommand{{\\WC{gk}{ak}GainPlan}}{{{f2(v['gain_cgs_planned'])}}}"]
    open(os.path.join(OUT, 'tables', 'contention_macros.tex'), 'w').write('\n'.join(L) + '\n')
    md = ['# What the contention replay measures (test split)', '',
          'Mean difference from AEFT (%, positive = larger than AEFT). Inflation = mean contention makespan / planned makespan.', '',
          '| Method | Inflation (workflows) | Planned | Remote volume | Busiest port | Contention makespan | Remote volume, random CCR 10 | Contention makespan, random CCR 10 |',
          '|:--|--:|--:|--:|--:|--:|--:|--:|']
    md += ['| ' + ' | '.join(c.replace('$-$', '−').strip(' \\') for c in r.split(' & ')) + ' |' for r in rows]
    md += ['', '| Group | Instances | Inflation range (×) | corr(contention, port) | corr(contention, planned) | CGS losses to AEFT | % of losses with heavier port | P(lose | heavier) % | P(lose | lighter) % |',
           '|:--|--:|--:|--:|--:|--:|--:|--:|--:|']
    for g in GROUPS:
        gr = G[g]
        md.append(f"| {g} | {gr['instances']} | {gr['inflation_min']:.1f}–{gr['inflation_max']:.1f} | {gr['median_corr_cont_port']:.2f} | "
                  f"{gr['median_corr_cont_planned']:.2f} | {gr['cgs_loses_to_aeft']} | {gr['pct_losses_with_heavier_port']:.1f} | "
                  f"{gr['pct_lose_when_heavier']:.1f} | {gr['pct_lose_when_lighter']:.1f} |")
    open(os.path.join(OUT, 'why_contention', 'WHY_CONTENTION.md'), 'w').write('\n'.join(md) + '\n')


if __name__ == '__main__':
    main()
