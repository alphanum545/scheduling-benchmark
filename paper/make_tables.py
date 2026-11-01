#!/usr/bin/env python3
"""Write LaTeX table bodies for the CGS manuscript from results.json (no hand-copied numbers),
and SUMMARY.md (the same tables with headers, for reading)."""
import json, math, os, sys
OUT = sys.argv[1]
R = json.load(open('results.json'))
N = dict(heft_ins='HEFT', peft_ins='PEFT', ippts_ins='IPPTS', aeft_ins='AEFT', pmms_ins='PMMS', bppts_ins='BPPTS',
         la_heft='Lookahead HEFT', virtual_best='Virtual best', cgs='CGS',
         heft='HEFT', peft='PEFT', ippts='IPPTS', aeft='AEFT', pmms='PMMS', bppts='BPPTS')
BASE = ['heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']
APPEND = ['heft', 'peft', 'ippts', 'aeft', 'pmms', 'bppts']
MD = []


def f2(x): return f'{x:.2f}'.replace('-', '$-$')


def ci(c): return f'[{f2(c[0])}, {f2(c[1])}]'


def pfmt(p):
    if p == 0: return '$<10^{-300}$'
    e = math.floor(math.log10(p)); m = p / 10 ** e
    return f'${m:.1f}\\times10^{{{e}}}$' if e < -2 else f'{p:.3f}'


def w(name, s):
    open(os.path.join(OUT, name), 'w').write(s)


def md(title, header, rows, note=''):
    """Same rows as the LaTeX body, as a Markdown table."""
    conv = lambda c: (c.replace('$-$', '−').replace('\\times10^{', 'e').replace('}$', '').replace('$<10^{', '<1e')
                      .replace('\\lambda', 'λ').replace('$', '').strip())
    body = [r.rstrip(' \\') for r in rows if r != '\\midrule']
    MD.append(f'## {title}\n\n| ' + ' | '.join(header) + ' |\n|' + '|'.join([':--'] + ['--:'] * (len(header) - 1)) + '|\n' +
              '\n'.join('| ' + ' | '.join(conv(c) for c in r.split(' & ')) + ' |' for r in body) + ('\n\n' + note if note else '') + '\n')


# main comparison
T, A = R['overall']['test'], R['overall']['all']
rows = []
for b in BASE + ['virtual_best']:
    t, a = T[b], A[b]
    p = t.get('p_holm', t['p_cluster'])
    rows.append(f"{N[b]} & {f2(t['mean'])} & {ci(t['ci'])} & {f2(t['median'])} & "
                f"{t['wins']}/{t['ties']}/{t['losses']} & {pfmt(p)} & {f2(a['mean'])} \\\\")
    if b == 'la_heft': rows.append('\\midrule')
w('tab_main_rows.tex', '\n'.join(rows) + '\n')
md('Main comparison (held-out test split, all methods with insertion)',
   ['Baseline', 'Mean gain %', '95 % CI', 'Median %', 'CGS W/T/L', 'Holm p', 'Gain %, all instances'], rows,
   f"Gain = 100 (C_B − C_CGS) / C_B; positive favours CGS. CI: cluster bootstrap. p: cluster-level Wilcoxon, Holm-adjusted. Test instances: {R['counts']['test']}.")

# ranks and SLR
F, S = R['friedman']['test'], R['slr']['test']
order = sorted(F['ranks'], key=lambda a: F['ranks'][a])
SP, EF = R['speedup']['test'], R['efficiency']['test']
rows = [f"{N[a]} & {F['ranks'][a]:.2f} & {S[a]:.2f} \\\\" for a in order]
w('tab_rank_rows.tex', '\n'.join(rows) + '\n')
md('Mean rank and SLR (test)', ['Method', 'Mean Friedman rank', 'Mean SLR'], rows,
   f"Friedman χ² = {F['chi2']:.0f}, N = {F['N']}, Nemenyi critical difference = {F['cd']:.2f}.")
rows = [f"{N[a]} & {SP[a]:.3f} & {EF[a]:.3f} \\\\" for a in order]
w('tab_speedup_rows.tex', '\n'.join(rows) + '\n')
md('Mean speedup and efficiency (test)', ['Method', 'Speedup', 'Efficiency'], rows)
w('friedman.tex', f"\\newcommand{{\\FriedChi}}{{{F['chi2']:.0f}}}\\newcommand{{\\FriedCD}}{{{F['cd']:.2f}}}\\newcommand{{\\FriedN}}{{{F['N']}}}\n")

# replays
rows = []
for b in BASE:
    n, c = R['replay']['noise'][b], R['replay']['contention'][b]
    rows.append(f"{N[b]} & {f2(T[b]['mean'])} & {f2(n['mean'])} {ci(n['ci'])} & {f2(c['mean'])} {ci(c['ci'])} \\\\")
w('tab_replay_rows.tex', '\n'.join(rows) + '\n')
md('Gain under perturbed and contended replays (test)', ['Baseline', 'Static gain %', 'Noise replay gain % [CI]', 'Contention replay gain % [CI]'], rows,
   'Noise: 30 replays with every cost scaled by U[0.8, 1.2], same mapping and order. Contention: one-port network replay.')
sec = R['secondary']
w('tab_secondary_rows.tex', '\n'.join(
    f"{N[a]} & {sec[a]['utilization']:.3f} & {sec[a]['comm_ratio']:.3f} & {sec[a]['rob_ratio']:.3f} & {sec[a]['cont_ratio']:.2f} \\\\"
    for a in ['cgs'] + BASE) + '\n')
md('Secondary schedule metrics (test means)', ['Method', 'Utilization', 'Comm. ratio', 'Robustness ratio', 'Contention ratio'],
   [f"{N[a]} & {sec[a]['utilization']:.3f} & {sec[a]['comm_ratio']:.3f} & {sec[a]['rob_ratio']:.3f} & {sec[a]['cont_ratio']:.2f} \\\\" for a in ['cgs'] + BASE])

# ablation
if R.get('ablation', {}).get('test'):
    L = dict(cgs_single='Without joint placement (single-task only)', cgs_credit0='Plain rank gate (no communication credit)',
             cgs_nogate='No gate (closures always formed)', cgs_lam0='Selection by EFT only ($\\lambda=0$)',
             cgs_tailnone='Closure key without downstream term', cgs_commitall='Commit the join with its parents',
             cgs_parents3='Up to three remaining parents')
    AT, AA = R['ablation']['test'], R['ablation']['all']
    keys = [k for k in L if k in AT]
    rows = [f"{L[k]} & {f2(AT[k]['mean'])} & {ci(AT[k]['ci'])} & {AT[k]['wins']}/{AT[k]['ties']}/{AT[k]['losses']} & {pfmt(AT[k]['p_holm'])} & {f2(AA[k]['mean'])} \\\\" for k in keys]
    w('tab_ablation_rows.tex', '\n'.join(rows) + '\n')
    md('Ablations (test; gain of full CGS over the variant)', ['Variant', 'Mean gain %', '95 % CI', 'CGS W/T/L', 'Holm p', 'Gain %, all'], rows)
    G = R['mechanism_by_group']
    rows = [f"{g} & {G[g]['n']} & {f2(G[g]['joint_placement'])} & {f2(G[g]['comm_credit'])} & {f2(G[g]['gate'])} & {f2(G[g]['table_in_selection'])} & {100 * G[g]['changed_by_closures']:.1f} \\\\"
            for g in ['workflow', 'ccr0.1', 'ccr1', 'ccr10']]
    w('tab_mechanism_rows.tex', '\n'.join(rows) + '\n')
    md('Component effect by group (test, mean gain %)', ['Group', 'n', 'Joint placement', 'Comm. credit', 'Gate', 'Table in selection', 'Instances changed by closures %'], rows)
# context: append-only originals
if R.get('append'):
    C_, I_ = R['append']['cgs_vs_original'], R['append']['insertion_effect']
    rows = [f"{N[b]} & {f2(C_[b]['mean'])} & {ci(C_[b]['ci'])} & {C_[b]['wins']}/{C_[b]['ties']}/{C_[b]['losses']} & {f2(I_[b]['mean'])} & {I_[b]['wins']}/{I_[b]['ties']}/{I_[b]['losses']} \\\\" for b in APPEND]
    w('tab_append_rows.tex', '\n'.join(rows) + '\n')
    md('Context: repository originals with append-only placement (test)', ['Baseline', 'CGS gain %', '95 % CI', 'CGS W/T/L', 'Gain of insertion alone %', 'Insertion W/T/L'], rows,
       'The last two columns compare each baseline with insertion against the same baseline without it.')
# methods not used as baselines
if R.get('extra'):
    XN = dict(cpop='CPOP', ppts='PPTS', ipeft='IPEFT', mpeft='MPEFT', lstd='LSTD', left='LEFT', left_d='LEFT-D')
    rows = [f"{XN[k]} & {v['n']} & {f2(v['mean'])} & {100 * v['wins'] / v['n']:.1f} & {100 * v['losses'] / v['n']:.1f} \\\\"
            for k, v in R['extra'].items()]
    w('tab_extra_rows.tex', '\n'.join(rows) + '\n')
    md('Methods discussed but not used as baselines (test, append-only as in the repository)', ['Method', 'n', 'CGS gain %', 'CGS better %', 'CGS worse %'], rows)
# strata
for col, title in [('ccr', 'CCR (random DAGs)'), ('p', 'processors'), ('beta', 'heterogeneity β (random DAGs)'), ('kind', 'instance kind'), ('size', 'task count'), ('family', 'family')]:
    st = R['strata'][col]
    rows = [f"{k} & {st[k]['heft_ins']['n']} & " + ' & '.join(f2(st[k][b]['mean']) for b in BASE + ['virtual_best']) + ' \\\\' for k in st]
    w(f'tab_strata_{col}_rows.tex', '\n'.join(rows) + '\n')
    md(f'Mean gain % by {title} (test)', [col, 'n'] + [N[b] for b in BASE + ['virtual_best']], rows)
open(os.path.join(OUT, '..', 'SUMMARY.md'), 'w').write('# CGS evaluation: results\n\nGenerated by paper/make_tables.py from results.json.\n\n' + '\n'.join(MD))
print('tables written')
