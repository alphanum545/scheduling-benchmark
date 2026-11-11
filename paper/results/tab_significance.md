## Statistical significance

Friedman test over 7 algorithms on 2814 instances: chi2 = 7748.6, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.170 ranks.

| algo   |   average_rank | differs_from_best   |
|:-------|---------------:|:--------------------|
| DSLA   |          1.626 | —                   |
| AEFT   |          3.626 | yes                 |
| PMMS   |          3.701 | yes                 |
| HEFT   |          3.76  | yes                 |
| LEFT   |          4.321 | yes                 |
| PPTS   |          4.39  | yes                 |
| IPPTS  |          6.576 | yes                 |

### Wilcoxon signed-rank: DSLA vs each algorithm (Holm-corrected, two-sided)

gain = (other - DSLA) / other; positive = DSLA has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|-----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| LEFT     | 2814 |   2511 |     41 |      262 |           8.333 |             6.729 | [6.466, 7.155]   |      0.956 |         0 |      529 |        0 | yes, better   |         1.44e-87 | yes      |
| HEFT     | 2814 |   2436 |     36 |      342 |           6.323 |             5.099 | [4.784, 5.347]   |      0.897 |         0 |      529 |        0 | yes, better   |         5.36e-87 | yes      |
| AEFT     | 2814 |   2405 |     25 |      384 |           6.369 |             4.511 | [4.174, 4.830]   |      0.859 |         0 |      529 |        0 | yes, better   |         6.65e-86 | yes      |
| PMMS     | 2814 |   2461 |     27 |      326 |           6.631 |             4.582 | [4.336, 4.866]   |      0.887 |         0 |      529 |        0 | yes, better   |         5.36e-87 | yes      |
| PPTS     | 2814 |   2476 |     14 |      324 |           8.322 |             6.565 | [6.193, 6.945]   |      0.886 |         0 |      529 |        0 | yes, better   |         1.32e-84 | yes      |
| IPPTS    | 2814 |   2760 |      2 |       52 |          27.744 |            23.693 | [22.805, 24.881] |      0.994 |         0 |      529 |        0 | yes, better   |         1.4e-87  | yes      |

