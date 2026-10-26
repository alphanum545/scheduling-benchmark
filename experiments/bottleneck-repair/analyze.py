#!/usr/bin/env python3
"""Postprocess an existing BCR report; never run a scheduler or select data.
All finite valid observations are retained; incomplete/duplicate data is an error.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import numpy as np
import pandas as pd
from scipy.stats import wilcoxon

CANDIDATE = 'bcr_dsla'
MEASUREMENT_RUN = '40312108945'
EXPECTED = ['scct','dsla','heft','left','oppts','aeft','peft',
            'bcr_scct_seed','bcr_dsla_seed','bcr_scct',
            'bcr_dsla_single','bcr_dsla','bcr_dsla_strict']


def holm(values):
    order = np.argsort(values)
    out = np.zeros(len(values))
    high = 0.0
    for j, i in enumerate(order):
        high = max(high, min(1.0, (len(values)-j)*values[i]))
        out[i] = high
    return out


def probability(x):
    x = np.asarray(x, dtype=float)
    nz = x[x != 0]
    return float(wilcoxon(nz, alternative='two-sided').pvalue) if len(nz) else 1.0


def clustered_mean_ci(gain, info, repeats=4000):
    """Instance-weighted mean; resample named-DAG clusters within each family.
    This is not proof that the filename-derived groups are independent DAGs.
    """
    z = info[['family','group']].copy()
    z['gain'] = gain
    groups = z.groupby(['family','group']).gain.agg(['sum','count'])
    rng = np.random.default_rng(20261026)
    numerator = np.zeros(repeats)
    denominator = np.zeros(repeats)
    for _, v in groups.groupby(level=0):
        sums = v['sum'].to_numpy()
        counts = v['count'].to_numpy()
        idx = rng.integers(0, len(v), size=(repeats,len(v)))
        numerator += sums[idx].sum(axis=1)
        denominator += counts[idx].sum(axis=1)
    return np.percentile(numerator/denominator,[2.5,97.5]).tolist()


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--report',default='bcr-dev-v1')
    parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[2])
    args=parser.parse_args()
    root=args.root.resolve()
    folder=root/'results'/'reports'/args.report
    path=folder/'results.csv'
    df=pd.read_csv(path)
    if set(df.algo)!=set(EXPECTED):
        raise ValueError('Unexpected comparison set: '+repr(sorted(df.algo.unique())))
    if df.duplicated(['path','algo']).any():
        raise ValueError('Duplicate path/algorithm observations')
    if (df.status!='ok').any():
        raise ValueError('Non-valid rows must be audited, not silently omitted')
    makes=df.pivot(index='path',columns='algo',values='makespan')[EXPECTED]
    if makes.isna().any().any() or (makes<=0).any().any():
        raise ValueError('Incomplete coverage or nonpositive makespan')
    info=df.drop_duplicates('path').set_index('path').loc[makes.index]
    group_columns=['dataset','family','nodes','instance']
    info['group']=info[group_columns].astype(str).agg('/'.join,axis=1)
    runtime=df.pivot(index='path',columns='algo',values='runtime_s')[EXPECTED]
    if (runtime<=0).any().any():
        raise ValueError('Nonpositive scheduler runtime')
    seed_diff={
        'scct':int((makes.scct!=makes.bcr_scct_seed).sum()),
        'dsla':int((makes.dsla!=makes.bcr_dsla_seed).sum())}
    n=len(makes)
    variants=['scct','dsla','bcr_scct_seed','bcr_dsla_seed',
              'bcr_scct','bcr_dsla_single','bcr_dsla_strict','bcr_dsla']
    cfg=[]
    for a in variants:
        ds=100*(makes.dsla-makes[a])/makes.dsla
        sc=100*(makes.scct-makes[a])/makes.scct
        cfg.append({'algorithm':a,'mean_makespan':float(makes[a].mean()),
                    'mean_gain_vs_dsla':float(ds.mean()),'mean_gain_vs_scct':float(sc.mean()),
                    'mean_runtime_s':float(runtime[a].mean()),
                    'median_runtime_s':float(runtime[a].median()),
                    'p95_runtime_s':float(runtime[a].quantile(.95)),
                    'max_runtime_s':float(runtime[a].max()),
                    'median_runtime_ratio_vs_dsla':float((runtime[a]/runtime.dsla).median()),
                    'wins_vs_dsla':int((ds>0).sum()),'ties_vs_dsla':int((ds==0).sum()),
                    'losses_vs_dsla':int((ds<0).sum())})
    configs=pd.DataFrame(cfg)
    configs.to_csv(folder/'bcr_configurations.csv',index=False,float_format='%.9g')
    pairs=[]
    for a in EXPECTED:
        if a==CANDIDATE:continue
        gain=100*(makes[a]-makes[CANDIDATE])/makes[a]
        bydag=gain.groupby(info.group).mean()
        lo,hi=clustered_mean_ci(gain,info)
        pairs.append({'baseline':a,'n':n,'wins':int((gain>0).sum()),
            'ties':int((gain==0).sum()),'losses':int((gain<0).sum()),
            'mean_gain_percent':float(gain.mean()),'median_gain_percent':float(gain.median()),
            'worst_gain_percent':float(gain.min()),'best_gain_percent':float(gain.max()),
            'mean_ci_low':float(lo),'mean_ci_high':float(hi),
            'p_raw':probability(gain),'p_group_raw':probability(bydag),
            'ratio_of_mean_makespans_gain':float(100*(1-makes[CANDIDATE].mean()/makes[a].mean()))})
    paired=pd.DataFrame(pairs)
    paired['p_holm']=holm(paired.p_raw.to_numpy())
    paired['p_group_holm']=holm(paired.p_group_raw.to_numpy())
    paired.to_csv(folder/'bcr_pairwise.csv',index=False,float_format='%.10g')
    fam=[]
    for family, idx in info.groupby('family').groups.items():
        m=makes.loc[idx]
        ds=100*(m.dsla-m[CANDIDATE])/m.dsla
        single=100*(m.bcr_dsla_single-m[CANDIDATE])/m.bcr_dsla_single
        fam.append({'family':family,'n':len(m),
                    'mean_gain_vs_dsla':float(ds.mean()),
                    'mean_gain_vs_scct':float((100*(m.scct-m[CANDIDATE])/m.scct).mean()),
                    'mean_gain_vs_single':float(single.mean()),
                    'wins_vs_dsla':int((ds>0).sum()),'ties_vs_dsla':int((ds==0).sum()),
                    'losses_vs_dsla':int((ds<0).sum()),
                    'worst_gain_vs_dsla':float(ds.min()),
                    'mean_runtime_s':float(runtime.loc[idx,CANDIDATE].mean())})
    families=pd.DataFrame(fam)
    families.to_csv(folder/'bcr_families.csv',index=False,float_format='%.9g')
    manifest={
        'measurement_run':MEASUREMENT_RUN,'report':args.report,'instances':n,'rows':len(df),
        'named_dag_groups':int(info.group.nunique()),'seed_makespan_mismatches':seed_diff,
        'raw_results_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
        'candidate_source_sha256':hashlib.sha256((root/'algorithms/makespan/bcr.cpp').read_bytes()).hexdigest(),
        'observed_algorithm_hashes':{a:sorted(df.loc[df.algo==a,'algo_hash'].unique()) for a in EXPECTED},
        'bootstrap_replicates':4000,'bootstrap_seed':20261026,
        'split':'development','group_independence_verified':False,
        'all_rows_valid':True,'complete_paired_coverage':True}
    (folder/'bcr_analysis_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    display=configs[['algorithm','mean_gain_vs_dsla','mean_gain_vs_scct','mean_runtime_s',
                     'median_runtime_s','p95_runtime_s','wins_vs_dsla','ties_vs_dsla','losses_vs_dsla']]
    pdsp=paired[['baseline','wins','ties','losses','mean_gain_percent','median_gain_percent',
                'mean_ci_low','mean_ci_high','p_holm','p_group_holm']].copy()
    for col in ['p_holm','p_group_holm']:pdsp[col]=pdsp[col].map(lambda x:f'{x:.3e}')
    lines=['# BCR development audit','',f'Measurement run: {MEASUREMENT_RUN}. Development split only.',
           f'Valid report rows: {len(df)}; fully paired instances: {n}; named-DAG groups: {info.group.nunique()}.',
           'Seed makespan mismatches: '+json.dumps(seed_diff)+'.',
           'Candidate source SHA-256: '+manifest['candidate_source_sha256']+'.','',
           '## All configurations','',display.to_markdown(index=False,floatfmt='.4f'),'',
           '## Full candidate versus comparators','',pdsp.to_markdown(index=False,floatfmt='.4f'),'',
           'Mean confidence intervals use family-stratified resampling of filename-derived DAG groups. '
           'They are sensitivity intervals, not a certificate of independent samples. Tests are two-sided, '
           'Holm-corrected across all comparisons shown.','',
           '## Family results','',families.to_markdown(index=False,floatfmt='.4f'),'',
           '## Interpretation limits','',
           'All gains are arithmetic means of paired relative differences, in percent. '
           'The separately exported ratio-of-means measure is different. Runtime includes seed construction '
           'and is measured in the same public workflow, but the runs share hosts and are not isolated microbenchmarks. '
           'Source-hash matching and instance inclusion are recorded in bcr_analysis_manifest.json. '
           'Seed agreement here concerns makespans, not byte-identical full schedule traces. '
           'Full repair must be compared with single-task repair, not only with its seed. '
           'All candidates are development-exposed; statistical significance here is not confirmatory validation. '
           'No test-split results were computed by this analysis.']
    (folder/'bcr_analysis.md').write_text('\n'.join(lines)+'\n')
    if any(seed_diff.values()):
        raise AssertionError('Seed conformance failed; results cannot support seed guarantees')
    if (makes[CANDIDATE]>makes.dsla).any() or (makes.bcr_scct>makes.scct).any():
        raise AssertionError('Non-worsening property failed')
    print(f'BCR analysis complete: {n} paired instances; {len(df)} rows; all invariants passed.')


if __name__=='__main__':
    main()
