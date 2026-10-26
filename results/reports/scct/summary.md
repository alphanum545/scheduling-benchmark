# Benchmark report: scct

Algorithms: scct, heft, peft, ipeft, mpeft, aeft, ppts, ippts, bppts, oppts, pmms, left, lstd  
Instances compared: 2814 of 2814 (only those where every algorithm is valid)

## Validity
| algo   |   ok | example_problem   |
|:-------|-----:|:------------------|
| scct   | 2814 |                   |
| heft   | 2814 |                   |
| peft   | 2814 |                   |
| ipeft  | 2814 |                   |
| mpeft  | 2814 |                   |
| aeft   | 2814 |                   |
| ppts   | 2814 |                   |
| ippts  | 2814 |                   |
| bppts  | 2814 |                   |
| oppts  | 2814 |                   |
| pmms   | 2814 |                   |
| left   | 2814 |                   |
| lstd   | 2814 |                   |

## Overall averages
| algo   |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:-------|-----------:|--------:|----------:|-------------:|------------:|
| scct   |    4785.65 |  9.2407 |   20.8269 |       1.3073 |      0.0338 |
| heft   |    4831.49 |  9.5658 |   20.7725 |       1.3167 |      0.0895 |
| peft   |    5072.72 |  9.9806 |   19.3879 |       1.1739 |      0.1033 |
| ipeft  |    4978.1  |  9.8099 |   19.918  |       1.2141 |      0.111  |
| mpeft  |    5116.35 |  9.9272 |   19.6266 |       1.1839 |      0.1271 |
| aeft   |    4869.45 |  9.3994 |   20.2681 |       1.2725 |      0.1032 |
| ppts   |    4958.85 |  9.6896 |   19.7151 |       1.22   |      0.1021 |
| ippts  |    5903.11 | 12.8342 |   13.7405 |       0.8396 |      0.0989 |
| bppts  |    5644.41 | 11.5189 |   16.0479 |       0.9603 |      0.032  |
| oppts  |    4849.97 |  9.3535 |   20.541  |       1.285  |      0.1031 |
| pmms   |    4880.54 |  9.4398 |   20.2802 |       1.2726 |      0.0311 |
| left   |    4627.92 |  9.5247 |   19.6019 |       1.2583 |      0.0322 |
| lstd   |    4852.49 |  9.5988 |   20.5963 |       1.2999 |      0.0926 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo   |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:-------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| scct   |       0.7169 |      2.0335 |        0.5131 |      1.0266 |   0.0181 |   45038.8 |      10.3597 |
| heft   |       0.7642 |      2.2867 |        0.5232 |      1.0286 |   0.018  |   47596.9 |      10.4747 |
| peft   |       0.6834 |      2.2139 |        0.5515 |      1.0261 |   0.0175 |   48736.2 |       9.8425 |
| ipeft  |       0.694  |      2.331  |        0.5526 |      1.0272 |   0.0176 |   49914   |      10.235  |
| mpeft  |       0.6762 |      2.215  |        0.5553 |      1.0267 |   0.0172 |   48621.3 |       9.9451 |
| aeft   |       0.7161 |      2.1525 |        0.5081 |      1.0263 |   0.0181 |   45945.4 |      10.0902 |
| ppts   |       0.6908 |      2.1235 |        0.539  |      1.026  |   0.0178 |   48025.2 |       9.8079 |
| ippts  |       0.6849 |      1.8989 |        0.6033 |      1.0226 |   0.0169 |   47535.8 |       7.3831 |
| bppts  |       0.6363 |      2.1229 |        0.5051 |      1.0233 |   0.0173 |   45003.8 |       7.8739 |
| oppts  |       0.6997 |      2.1456 |        0.516  |      1.0262 |   0.018  |   48299.2 |      10.2239 |
| pmms   |       0.7233 |      2.1844 |        0.5144 |      1.027  |   0.018  |   46426.8 |      10.0691 |
| left   |       0.7107 |      2.4109 |        0.4549 |      1.0317 |   0.0188 |   55301.5 |      10.2934 |
| lstd   |       0.5903 |      2.1942 |        0.5183 |      1.0073 |   0.019  |   44862.4 |       9.645  |

## scct versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| heft     |        60.2  |        0.75 |       39.05 |          1.414 |
| peft     |        78.57 |        0.89 |       20.54 |          5.891 |
| ipeft    |        69.65 |        0.85 |       29.5  |          4.191 |
| mpeft    |        73.99 |        0.96 |       25.05 |          5.406 |
| aeft     |        70.65 |        4.83 |       24.52 |          1.873 |
| ppts     |        79.07 |        0.92 |       20.01 |          3.936 |
| ippts    |        95.88 |        0.18 |        3.94 |         24.38  |
| bppts    |        93.67 |        0.32 |        6.01 |         17.465 |
| oppts    |        63.15 |        2.74 |       34.12 |          0.88  |
| pmms     |        71.07 |        1.24 |       27.68 |          2.105 |
| left     |        67.09 |        0.36 |       32.55 |          3.47  |
| lstd     |        65.25 |        0.68 |       34.08 |          2.188 |

## Statistical significance

Friedman test over 13 algorithms on 2814 instances: chi2 = 11538.1, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.344 ranks.

| algo   |   average_rank | differs_from_best   |
|:-------|---------------:|:--------------------|
| scct   |          4.044 | —                   |
| oppts  |          4.931 | yes                 |
| heft   |          5.634 | yes                 |
| aeft   |          5.649 | yes                 |
| pmms   |          5.739 | yes                 |
| lstd   |          6.301 | yes                 |
| ipeft  |          6.638 | yes                 |
| ppts   |          6.898 | yes                 |
| left   |          6.998 | yes                 |
| mpeft  |          7.187 | yes                 |
| peft   |          7.824 | yes                 |
| bppts  |         11.093 | yes                 |
| ippts  |         12.063 | yes                 |

### Wilcoxon signed-rank: scct vs each algorithm (Holm-corrected, two-sided)

gain = (other - scct) / other; positive = scct has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|-----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| heft     | 2814 |   1694 |     21 |     1099 |           1.414 |             1.294 | [1.079, 1.682]   |      0.248 | 8.26e-30  |      529 | 8.26e-30  | yes, better   |         1.29e-06 | yes      |
| peft     | 2814 |   2211 |     25 |      578 |           5.891 |             4.804 | [4.412, 5.202]   |      0.722 | 5.65e-239 |      529 | 5.65e-238 | yes, better   |         8.87e-69 | yes      |
| ipeft    | 2814 |   1960 |     24 |      830 |           4.191 |             3.645 | [3.175, 3.974]   |      0.589 | 5.29e-160 |      529 | 3.18e-159 | yes, better   |         3.7e-49  | yes      |
| mpeft    | 2814 |   2082 |     27 |      705 |           5.406 |             3.89  | [3.554, 4.305]   |      0.636 | 5.66e-186 |      529 | 4.53e-185 | yes, better   |         5.78e-57 | yes      |
| aeft     | 2814 |   1988 |    136 |      690 |           1.873 |             1.542 | [1.433, 1.701]   |      0.611 | 5.56e-165 |      529 | 3.89e-164 | yes, better   |         4.98e-66 | yes      |
| ppts     | 2814 |   2225 |     26 |      563 |           3.936 |             3.375 | [3.146, 3.614]   |      0.717 | 5.08e-236 |      529 | 4.58e-235 | yes, better   |         7.36e-69 | yes      |
| ippts    | 2814 |   2698 |      5 |      111 |          24.38  |            20.752 | [19.921, 21.609] |      0.981 | 0         |      529 | 0         | yes, better   |         2.76e-86 | yes      |
| bppts    | 2814 |   2636 |      9 |      169 |          17.465 |            14.804 | [14.267, 15.227] |      0.953 | 0         |      529 | 0         | yes, better   |         7.84e-87 | yes      |
| oppts    | 2814 |   1777 |     77 |      960 |           0.88  |             0.977 | [0.845, 1.087]   |      0.368 | 1.94e-62  |      529 | 5.82e-62  | yes, better   |         7.21e-24 | yes      |
| pmms     | 2814 |   2000 |     35 |      779 |           2.105 |             1.701 | [1.542, 1.865]   |      0.55  | 2.51e-139 |      529 | 1.26e-138 | yes, better   |         1.7e-68  | yes      |
| left     | 2814 |   1888 |     10 |      916 |           3.47  |             3.323 | [2.925, 3.666]   |      0.42  | 1.61e-82  |      529 | 6.44e-82  | yes, better   |         3.22e-24 | yes      |
| lstd     | 2814 |   1836 |     19 |      959 |           2.188 |             2.385 | [2.131, 2.694]   |      0.362 | 7.54e-62  |      529 | 1.51e-61  | yes, better   |         1.68e-20 | yes      |
