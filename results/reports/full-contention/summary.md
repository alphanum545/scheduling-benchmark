# Benchmark report: full-contention

Algorithms: z_best10, bla_heft, pmms, heft, peft, ipeft, mpeft, ppts, ippts, lstd, aeft, oppts, bppts, left, left_d  
Instances compared: 2814 of 2814 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| z_best10 | 2814 |                   |
| bla_heft | 2814 |                   |
| pmms     | 2814 |                   |
| heft     | 2814 |                   |
| peft     | 2814 |                   |
| ipeft    | 2814 |                   |
| mpeft    | 2814 |                   |
| ppts     | 2814 |                   |
| ippts    | 2814 |                   |
| lstd     | 2814 |                   |
| aeft     | 2814 |                   |
| oppts    | 2814 |                   |
| bppts    | 2814 |                   |
| left     | 2814 |                   |
| left_d   | 2814 |                   |

## Overall averages
| algo     |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|--------:|----------:|-------------:|------------:|
| z_best10 |    4469.6  |  8.822  |   22.1652 |       1.3845 |      0.0386 |
| bla_heft |    4664.7  |  8.9551 |   22.107  |       1.3783 |      0.0345 |
| pmms     |    4880.54 |  9.4398 |   20.2802 |       1.2726 |      0.0311 |
| heft     |    4831.49 |  9.5658 |   20.7725 |       1.3167 |      0.0895 |
| peft     |    5072.72 |  9.9806 |   19.3879 |       1.1739 |      0.1033 |
| ipeft    |    4978.1  |  9.8099 |   19.918  |       1.2141 |      0.111  |
| mpeft    |    5116.35 |  9.9272 |   19.6266 |       1.1839 |      0.1271 |
| ppts     |    4958.85 |  9.6896 |   19.7151 |       1.22   |      0.1021 |
| ippts    |    5903.11 | 12.8342 |   13.7405 |       0.8396 |      0.0989 |
| lstd     |    4852.49 |  9.5988 |   20.5963 |       1.2999 |      0.0926 |
| aeft     |    4869.45 |  9.3994 |   20.2681 |       1.2725 |      0.1032 |
| oppts    |    4849.97 |  9.3535 |   20.541  |       1.285  |      0.1031 |
| bppts    |    5644.41 | 11.5189 |   16.0479 |       0.9603 |      0.032  |
| left     |    4627.92 |  9.5247 |   19.6019 |       1.2583 |      0.0322 |
| left_d   |    4624.13 |  9.5204 |   19.6146 |       1.259  |      0.0345 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo     |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:---------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| z_best10 |       0.7126 |      2.4208 |        0.5125 |      1.0316 |   0.0177 |   55586.2 |      11.4288 |
| bla_heft |       0.7098 |      2.6293 |        0.5179 |      1.0278 |   0.0178 |   54160.8 |      11.1202 |
| pmms     |       0.7233 |      2.1844 |        0.5144 |      1.027  |   0.018  |   46426.8 |      10.0691 |
| heft     |       0.7642 |      2.2867 |        0.5232 |      1.0286 |   0.018  |   47596.9 |      10.4747 |
| peft     |       0.6834 |      2.2139 |        0.5515 |      1.0261 |   0.0175 |   48736.2 |       9.8425 |
| ipeft    |       0.694  |      2.331  |        0.5526 |      1.0272 |   0.0176 |   49914   |      10.235  |
| mpeft    |       0.6762 |      2.215  |        0.5553 |      1.0267 |   0.0172 |   48621.3 |       9.9451 |
| ppts     |       0.6908 |      2.1235 |        0.539  |      1.026  |   0.0178 |   48025.2 |       9.8079 |
| ippts    |       0.6849 |      1.8989 |        0.6033 |      1.0226 |   0.0169 |   47535.8 |       7.3831 |
| lstd     |       0.5903 |      2.1942 |        0.5183 |      1.0073 |   0.019  |   44862.4 |       9.645  |
| aeft     |       0.7161 |      2.1525 |        0.5081 |      1.0263 |   0.0181 |   45945.4 |      10.0902 |
| oppts    |       0.6997 |      2.1456 |        0.516  |      1.0262 |   0.018  |   48299.2 |      10.2239 |
| bppts    |       0.6363 |      2.1229 |        0.5051 |      1.0233 |   0.0173 |   45003.8 |       7.8739 |
| left     |       0.7107 |      2.4109 |        0.4549 |      1.0317 |   0.0188 |   55301.5 |      10.2934 |
| left_d   |       0.5509 |      2.3493 |        0.4553 |      1.0007 |   0.0196 |   51795.3 |       9.6685 |

## z_best10 versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| bla_heft |        23.17 |       76.83 |        0    |          1.04  |
| pmms     |        87.46 |        0.96 |       11.58 |          6.631 |
| heft     |        86.57 |        1.28 |       12.15 |          6.323 |
| peft     |        87.95 |        0.53 |       11.51 |         10.18  |
| ipeft    |        86.71 |        0.82 |       12.47 |          8.673 |
| mpeft    |        87.21 |        0.53 |       12.26 |          9.756 |
| ppts     |        87.99 |        0.5  |       11.51 |          8.322 |
| ippts    |        98.08 |        0.07 |        1.85 |         27.744 |
| lstd     |        91.29 |        0.82 |        7.89 |          7.026 |
| aeft     |        85.47 |        0.89 |       13.65 |          6.369 |
| oppts    |        79.46 |        1.39 |       19.15 |          5.445 |
| bppts    |        95.38 |        0.11 |        4.51 |         21.189 |
| left     |        89.23 |        1.46 |        9.31 |          8.333 |
| left_d   |        88.49 |        1.31 |       10.2  |          8.216 |

## Statistical significance

Friedman test over 15 algorithms on 2814 instances: chi2 = 16065.6, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.404 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| z_best10 |          2.818 | —                   |
| bla_heft |          3.547 | yes                 |
| oppts    |          6.239 | yes                 |
| aeft     |          7.007 | yes                 |
| heft     |          7.109 | yes                 |
| pmms     |          7.175 | yes                 |
| lstd     |          7.851 | yes                 |
| ipeft    |          8.165 | yes                 |
| ppts     |          8.368 | yes                 |
| left_d   |          8.399 | yes                 |
| left     |          8.55  | yes                 |
| mpeft    |          8.691 | yes                 |
| peft     |          9.322 | yes                 |
| bppts    |         12.821 | yes                 |
| ippts    |         13.938 | yes                 |

### Wilcoxon signed-rank: z_best10 vs each algorithm (Holm-corrected, two-sided)

gain = (other - z_best10) / other; positive = z_best10 has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|-----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| bla_heft | 2814 |    652 |   2162 |        0 |           1.04  |             0     | [0.000, 0.000]   |      1     | 1.95e-108 |      529 | 1.95e-108 | yes, better   |         1.2e-57  | yes      |
| pmms     | 2814 |   2461 |     27 |      326 |           6.631 |             4.582 | [4.336, 4.866]   |      0.887 | 0         |      529 | 0         | yes, better   |         1.21e-86 | yes      |
| heft     | 2814 |   2436 |     36 |      342 |           6.323 |             5.099 | [4.784, 5.347]   |      0.897 | 0         |      529 | 0         | yes, better   |         1.4e-86  | yes      |
| peft     | 2814 |   2475 |     15 |      324 |          10.18  |             8.646 | [8.302, 8.960]   |      0.895 | 0         |      529 | 0         | yes, better   |         1.09e-85 | yes      |
| ipeft    | 2814 |   2440 |     23 |      351 |           8.673 |             7.859 | [7.504, 8.277]   |      0.886 | 0         |      529 | 0         | yes, better   |         2.99e-84 | yes      |
| mpeft    | 2814 |   2454 |     15 |      345 |           9.756 |             8.003 | [7.715, 8.321]   |      0.871 | 0         |      529 | 0         | yes, better   |         7.92e-85 | yes      |
| ppts     | 2814 |   2476 |     14 |      324 |           8.322 |             6.565 | [6.193, 6.945]   |      0.886 | 0         |      529 | 0         | yes, better   |         3.95e-84 | yes      |
| ippts    | 2814 |   2760 |      2 |       52 |          27.744 |            23.693 | [22.805, 24.881] |      0.994 | 0         |      529 | 0         | yes, better   |         3.26e-87 | yes      |
| lstd     | 2814 |   2569 |     23 |      222 |           7.026 |             6.303 | [5.952, 6.539]   |      0.94  | 0         |      529 | 0         | yes, better   |         4.01e-87 | yes      |
| aeft     | 2814 |   2405 |     25 |      384 |           6.369 |             4.511 | [4.174, 4.830]   |      0.859 | 0         |      529 | 0         | yes, better   |         1.99e-85 | yes      |
| oppts    | 2814 |   2236 |     39 |      539 |           5.445 |             3.737 | [3.493, 3.959]   |      0.77  | 1.53e-270 |      529 | 3.07e-270 | yes, better   |         2.62e-77 | yes      |
| bppts    | 2814 |   2684 |      3 |      127 |          21.189 |            17.802 | [17.232, 18.351] |      0.979 | 0         |      529 | 0         | yes, better   |         3.43e-87 | yes      |
| left     | 2814 |   2511 |     41 |      262 |           8.333 |             6.729 | [6.466, 7.155]   |      0.956 | 0         |      529 | 0         | yes, better   |         3.47e-87 | yes      |
| left_d   | 2814 |   2490 |     37 |      287 |           8.216 |             6.642 | [6.286, 7.048]   |      0.944 | 0         |      529 | 0         | yes, better   |         6.39e-87 | yes      |
