#!/usr/bin/env python3
"""split_check.py: does CGS's margin on the workflows survive a graph-level hold-out?

The dev/test split hashes the instance path, and a path includes the processor count, so one
workflow graph can have some processor counts in dev and others in test. This script re-reads the
existing results (no scheduling is rerun) and recomputes the test-split gains on three subsets:

  all      every workflow test instance (must reproduce the manuscript's numbers)
  clean    test instances whose graph (family, nodes, instance) has NO instance in dev
  unseen   test instances whose edge structure appears in NO dev instance, under any name
  seen*    the remaining test instances of the families that occur in `unseen` (matched control)

`unseen` is the strict graph-level hold-out: the structure is read from the archives, so two files
with the same DAG count as one graph even if they are named as different instances.

Usage (from the root of branch paper/cgs-reproduction):
  python3 paper/split_check.py outputs/runs.csv.gz data/archives [split_check.json]

Conventions are those of paper/analysis.py: gain = 100 (C_B - C_CGS) / C_B, positive favours CGS;
95 % percentile bootstrap over clusters (family, nodes, instance), 10,000 resamples, seed 20261031.
Needs numpy, pandas, scipy (versions in requirements.txt).
"""
import glob, hashlib, json, os, re, sys, zipfile
import numpy as np, pandas as pd
from scipy import stats

PROP = 'cgs'
BASE = ['heft_ins', 'peft_ins', 'ippts_ins', 'aeft_ins', 'pmms_ins', 'bppts_ins', 'la_heft']
ABL = ['cgs_single', 'cgs_nogate', 'cgs_lam0']
NAMES = {'heft_ins': 'HEFT', 'peft_ins': 'PEFT', 'ippts_ins': 'IPPTS', 'aeft_ins': 'AEFT', 'pmms_ins': 'PMMS',
         'bppts_ins': 'BPPTS', 'la_heft': 'Lookahead HEFT', 'cgs_single': 'No joint placement',
         'cgs_nogate': 'No gate', 'cgs_lam0': 'EFT-only selection'}
B_REPS, SEED = 10000, 20261031
PATH_RE = re.compile(r"(?:^|/)(?P<family>[^/]+)/(?P<nodes>\d+)Nodes/(?P<procs>\d+)PROCESSORS/(?P<file>\d+_\d+\.txt)$")


def split_of(path):
    """Identical to split_of in tools/bench.py."""
    return 'dev' if int(hashlib.sha256(path.encode()).hexdigest()[:8], 16) % 4 == 0 else 'test'


def topologies(archives):
    """path -> hash of the edge set (which entries of the n x n communication matrix are not -1)."""
    topo = {}
    for z in sorted(glob.glob(os.path.join(archives, '*.zip'))):
        with zipfile.ZipFile(z) as zf:
            for name in zf.namelist():
                m = PATH_RE.search(name)
                if not m or '__MACOSX' in name or os.path.basename(name).startswith('._'):
                    continue
                t = np.array(zf.read(name).split(), dtype=np.float64)
                if t.size < 2:
                    continue
                n, p = int(t[0]), int(t[1])
                # D2 has 5 header vectors of length p, D1 has 6; then n x p costs and n x n edges
                if n <= 0 or t.size not in (2 + 5 * p + n * p + n * n, 2 + 6 * p + n * p + n * n):
                    continue                                    # the six empty Montage files
                edges = t[-n * n:].reshape(n, n) != -1
                rel = f"{m['family']}/{m['nodes']}Nodes/{m['procs']}PROCESSORS/{m['file']}"
                topo[rel] = hashlib.sha256(str(n).encode() + np.packbits(edges).tobytes()).hexdigest()[:16]
    return topo


def boot_means(g, cl, rng):
    df = pd.DataFrame({'g': g, 'c': cl})
    agg = df.groupby('c').g.agg(['sum', 'count'])
    s, n = agg['sum'].to_numpy(), agg['count'].to_numpy()
    idx = rng.integers(0, len(s), size=(B_REPS, len(s)))
    return s[idx].sum(1) / n[idx].sum(1)


def compare(mk, meta, b, rng):
    x, y = mk.loc[meta.index, PROP].to_numpy(float), mk.loc[meta.index, b].to_numpy(float)
    g = 100 * (y - x) / y
    bm = boot_means(g, meta.cluster.to_numpy(), rng)
    cm = pd.Series(g, index=meta.index).groupby(meta.cluster).mean()
    nz = cm[cm != 0]
    return dict(mean=float(g.mean()), median=float(np.median(g)),
                ci=[float(np.percentile(bm, 2.5)), float(np.percentile(bm, 97.5))],
                wins=int((x < y).sum()), ties=int((x == y).sum()), losses=int((x > y).sum()),
                n=int(len(g)), clusters=int(cm.size),
                p_cluster=float(stats.wilcoxon(nz).pvalue) if len(nz) else 1.0), bm


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    runs, archives = sys.argv[1], sys.argv[2]
    out_path = sys.argv[3] if len(sys.argv) > 3 else 'split_check.json'

    d = pd.read_csv(runs)
    d = d[(d.status == 'ok') & ~d.family.str.startswith('R-')]
    mk = d.pivot_table(index='path', columns='algo', values='makespan', aggfunc='first')
    meta = d.drop_duplicates('path').set_index('path')[['family', 'nodes', 'p', 'instance']].loc[mk.index]
    meta['split'] = [split_of(p) for p in meta.index]
    meta['cluster'] = meta.family + '/' + meta.nodes.astype(str) + '/' + meta.instance.astype(str)

    topo = topologies(archives)
    missing = [p for p in meta.index if p not in topo]
    if missing:
        sys.exit(f'{len(missing)} instances in {runs} are not in {archives}, e.g. {missing[0]}')
    meta['topo'] = [topo[p] for p in meta.index]

    dev, test = meta[meta.split == 'dev'], meta[meta.split == 'test']
    unseen_mask = ~test.topo.isin(set(dev.topo))
    fams = sorted(test[unseen_mask].family.unique())
    subsets = {
        'all': test,
        'clean': test[~test.cluster.isin(set(dev.cluster))],
        'unseen': test[unseen_mask],
        'seen*': test[~unseen_mask & test.family.isin(fams)],
    }

    per_cluster = meta.groupby('cluster').topo.nunique()
    out = {'counts': dict(
        workflow_instances=int(len(meta)), dev=int(len(dev)), test=int(len(test)),
        graphs_by_name=int(meta.cluster.nunique()), distinct_structures=int(meta.topo.nunique()),
        graphs_with_one_structure_across_processor_counts=int((per_cluster == 1).sum()),
        test_structures=int(test.topo.nunique()), test_structures_also_in_dev=int(test.topo.isin(set(dev.topo)).groupby(test.topo).any().sum()),
        subsets={k: dict(instances=int(len(v)), graphs=int(v.cluster.nunique()), structures=int(v.topo.nunique()),
                         by_family=v.family.value_counts().sort_index().to_dict()) for k, v in subsets.items()}),
        'unseen_families': fams, 'gain': {}, 'unseen_minus_seen': {}}

    rng = np.random.default_rng(SEED)
    for b in BASE + [a for a in ABL if a in mk.columns]:
        boots = {}
        out['gain'][b] = {}
        for k, m in subsets.items():
            out['gain'][b][k], boots[k] = compare(mk, m, b, rng) if len(m) else (None, None)
        if boots['unseen'] is not None and boots['seen*'] is not None:
            diff = boots['unseen'] - boots['seen*']             # the two subsets share no cluster
            out['unseen_minus_seen'][b] = dict(
                diff=out['gain'][b]['unseen']['mean'] - out['gain'][b]['seen*']['mean'],
                ci=[float(np.percentile(diff, 2.5)), float(np.percentile(diff, 97.5))])
    json.dump(out, open(out_path, 'w'), indent=1)

    c = out['counts']
    print(f"Workflow instances: {c['workflow_instances']} ({c['dev']} dev, {c['test']} test); "
          f"{c['graphs_by_name']} graphs by name, {c['distinct_structures']} distinct edge structures.")
    print(f"Graphs with one structure across all their processor counts: "
          f"{c['graphs_with_one_structure_across_processor_counts']} of {c['graphs_by_name']}.")
    print(f"Test structures that also occur in dev: {c['test_structures_also_in_dev']} of {c['test_structures']}.\n")
    print('| Subset | Instances | Graphs | Structures | Families |\n|:--|--:|--:|--:|:--|')
    for k, v in c['subsets'].items():
        print(f"| {k} | {v['instances']} | {v['graphs']} | {v['structures']} | "
              + ', '.join(f'{f} {n}' for f, n in v['by_family'].items()) + ' |')
    cell = lambda r: '-' if r is None else f"{r['mean']:.2f} [{r['ci'][0]:.2f}, {r['ci'][1]:.2f}]"
    wtl = lambda r: '-' if r is None else f"{r['wins']}/{r['ties']}/{r['losses']}"
    print('\nMean gain of CGS % [95 % CI], test split, workflows\n')
    print('| Against | all | clean | unseen | seen* | unseen W/T/L | unseen - seen* |\n|:--|--:|--:|--:|--:|--:|--:|')
    for b, r in out['gain'].items():
        dm = out['unseen_minus_seen'].get(b)
        print(f"| {NAMES[b]} | {cell(r['all'])} | {cell(r['clean'])} | {cell(r['unseen'])} | {cell(r['seen*'])} | "
              f"{wtl(r['unseen'])} | " + ('-' if dm is None else f"{dm['diff']:+.2f} [{dm['ci'][0]:.2f}, {dm['ci'][1]:.2f}]") + ' |')
    print(f"\nseen* = test instances of {', '.join(fams)} whose structure also occurs in dev. Written {out_path}.")


if __name__ == '__main__':
    main()
