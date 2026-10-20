# Benchmark report: dev-round5-oppts-v2

Algorithms: l4_hr_b50_ins, l4_hr_pcm_ins, l4_hr_ins, l4_ins, l4_hr_pcm, l4_heft_ins, l4_heft, l4_oppts, oppts, heft, left, aeft, pmms, ppts, ippts, dsla  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo          |   ok | example_problem   |
|:--------------|-----:|:------------------|
| l4_hr_b50_ins |  712 |                   |
| l4_hr_pcm_ins |  712 |                   |
| l4_hr_ins     |  712 |                   |
| l4_ins        |  712 |                   |
| l4_hr_pcm     |  712 |                   |
| l4_heft_ins   |  712 |                   |
| l4_heft       |  712 |                   |
| l4_oppts      |  712 |                   |
| oppts         |  712 |                   |
| heft          |  712 |                   |
| left          |  712 |                   |
| aeft          |  712 |                   |
| pmms          |  712 |                   |
| ppts          |  712 |                   |
| ippts         |  712 |                   |
| dsla          |  712 |                   |

## Overall averages
| algo          |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:--------------|-----------:|--------:|----------:|-------------:|------------:|
| l4_hr_b50_ins |    4377.89 |  8.9055 |   23.4975 |       1.4169 |      0.0306 |
| l4_hr_pcm_ins |    4357.33 |  8.9385 |   22.9195 |       1.3997 |      0.0309 |
| l4_hr_ins     |    4387.46 |  8.9742 |   22.9354 |       1.3858 |      0.03   |
| l4_ins        |    4402.18 |  8.9921 |   22.8576 |       1.3821 |      0.0298 |
| l4_hr_pcm     |    4571.74 |  9.1949 |   22.0178 |       1.3516 |      0.0293 |
| l4_heft_ins   |    4479.08 |  9.453  |   21.9582 |       1.3623 |      0.0304 |
| l4_heft       |    4669.1  |  9.6198 |   21.5457 |       1.3318 |      0.0296 |
| l4_oppts      |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0306 |
| oppts         |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0973 |
| heft          |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0868 |
| left          |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.033  |
| aeft          |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.1008 |
| pmms          |    4696.44 |  9.4892 |   21.0138 |       1.2917 |      0.0298 |
| ppts          |    4784.25 |  9.732  |   20.474  |       1.2396 |      0.0983 |
| ippts         |    5710.9  | 12.9415 |   14.0713 |       0.8498 |      0.0985 |
| dsla          |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.0366 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo          |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:--------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| l4_hr_b50_ins |       0.6875 |      2.389  |        0.5306 |      1.0321 |   0.0182 |   52797.5 |      11.925  |
| l4_hr_pcm_ins |       0.6875 |      2.4823 |        0.5298 |      1.0313 |   0.0182 |   53273.4 |      11.7864 |
| l4_hr_ins     |       0.6716 |      2.4149 |        0.5319 |      1.0307 |   0.0184 |   53315.9 |      11.6817 |
| l4_ins        |       0.6726 |      2.4068 |        0.5301 |      1.0315 |   0.0185 |   52911.2 |      11.5767 |
| l4_hr_pcm     |       0.7184 |      2.2271 |        0.5183 |      1.027  |   0.0183 |   47031.6 |      11.3328 |
| l4_heft_ins   |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |
| l4_heft       |       0.7694 |      2.2521 |        0.5204 |      1.0289 |   0.0183 |   44279.4 |      11.1223 |
| l4_oppts      |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| oppts         |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| heft          |       0.7694 |      2.2526 |        0.5204 |      1.0288 |   0.0183 |   44319.5 |      11.1233 |
| left          |       0.7182 |      2.3751 |        0.4519 |      1.0318 |   0.0189 |   51806.8 |      10.8583 |
| aeft          |       0.7216 |      2.1409 |        0.5063 |      1.026  |   0.0184 |   42171.4 |      10.7195 |
| pmms          |       0.7294 |      2.1594 |        0.5136 |      1.0272 |   0.0182 |   42337.2 |      10.6531 |
| ppts          |       0.6972 |      2.0806 |        0.5377 |      1.0261 |   0.0182 |   43736   |      10.3769 |
| ippts         |       0.6879 |      1.8867 |        0.6055 |      1.0221 |   0.0171 |   43596.5 |       7.6734 |
| dsla          |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |

## l4_hr_b50_ins versus each algorithm
| versus        |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:--------------|-------------:|------------:|------------:|---------------:|
| l4_hr_pcm_ins |        51.12 |        2.81 |       46.07 |          0.795 |
| l4_hr_ins     |        57.72 |        4.78 |       37.5  |          1.042 |
| l4_ins        |        61.38 |        3.65 |       34.97 |          1.244 |
| l4_hr_pcm     |        70.22 |        1.69 |       28.09 |          3.921 |
| l4_heft_ins   |        78.37 |        2.11 |       19.52 |          4.592 |
| l4_heft       |        89.47 |        1.83 |        8.71 |          6.736 |
| l4_oppts      |        78.93 |        2.53 |       18.54 |          5.726 |
| oppts         |        78.93 |        2.53 |       18.54 |          5.726 |
| heft          |        89.47 |        1.83 |        8.71 |          6.735 |
| left          |        79.92 |        0.7  |       19.38 |          8.799 |
| aeft          |        79.78 |        1.12 |       19.1  |          6.598 |
| pmms          |        82.58 |        1.69 |       15.73 |          6.83  |
| ppts          |        90.45 |        0.84 |        8.71 |          8.592 |
| ippts         |        97.61 |        0.42 |        1.97 |         28.007 |
| dsla          |        50.28 |        4.21 |       45.51 |          0.388 |

## Statistical significance

Friedman test over 16 algorithms on 712 instances: chi2 = 4381.7, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.864 ranks.

| algo          |   average_rank | differs_from_best   |
|:--------------|---------------:|:--------------------|
| l4_hr_b50_ins |          4.474 | —                   |
| dsla          |          4.524 | no                  |
| l4_hr_pcm_ins |          4.543 | no                  |
| l4_hr_ins     |          5.304 | no                  |
| l4_ins        |          5.664 | yes                 |
| l4_hr_pcm     |          7.169 | yes                 |
| l4_heft_ins   |          8.39  | yes                 |
| l4_oppts      |          9.173 | yes                 |
| oppts         |          9.173 | yes                 |
| aeft          |          9.706 | yes                 |
| pmms          |          9.904 | yes                 |
| heft          |         10.091 | yes                 |
| l4_heft       |         10.092 | yes                 |
| left          |         10.933 | yes                 |
| ppts          |         11.695 | yes                 |
| ippts         |         15.164 | yes                 |

### Wilcoxon signed-rank: l4_hr_b50_ins vs each algorithm (Holm-corrected, two-sided)

gain = (other - l4_hr_b50_ins) / other; positive = l4_hr_b50_ins has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus        |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:--------------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| l4_hr_pcm_ins | 712 |    364 |     20 |      328 |           0.795 |             0.116 | [0.000, 0.323]   |      0.179 | 4.34e-05  |      409 | 8.68e-05  | yes, better   |         0.000369 | yes      |
| l4_hr_ins     | 712 |    411 |     34 |      267 |           1.042 |             0.463 | [0.278, 0.742]   |      0.343 | 9.62e-15  |      409 | 2.89e-14  | yes, better   |         2.71e-10 | yes      |
| l4_ins        | 712 |    437 |     26 |      249 |           1.244 |             0.836 | [0.561, 1.155]   |      0.375 | 1.75e-17  |      409 | 6.99e-17  | yes, better   |         1.05e-13 | yes      |
| l4_hr_pcm     | 712 |    500 |     12 |      200 |           3.921 |             2.242 | [1.783, 2.888]   |      0.65  | 2.96e-50  |      409 | 1.48e-49  | yes, better   |         5.99e-38 | yes      |
| l4_heft_ins   | 712 |    558 |     15 |      139 |           4.592 |             2.619 | [2.251, 3.132]   |      0.711 | 1.74e-59  |      409 | 1.04e-58  | yes, better   |         2.34e-40 | yes      |
| l4_heft       | 712 |    637 |     13 |       62 |           6.736 |             4.911 | [4.273, 5.383]   |      0.92  | 1.23e-98  |      409 | 1.72e-97  | yes, better   |         4.13e-62 | yes      |
| l4_oppts      | 712 |    562 |     18 |      132 |           5.726 |             3.952 | [3.142, 4.496]   |      0.793 | 3.4e-73   |      409 | 2.72e-72  | yes, better   |         2.93e-52 | yes      |
| oppts         | 712 |    562 |     18 |      132 |           5.726 |             3.952 | [3.142, 4.496]   |      0.793 | 3.4e-73   |      409 | 2.72e-72  | yes, better   |         2.93e-52 | yes      |
| heft          | 712 |    637 |     13 |       62 |           6.735 |             4.911 | [4.273, 5.383]   |      0.92  | 1.23e-98  |      409 | 1.72e-97  | yes, better   |         4.13e-62 | yes      |
| left          | 712 |    569 |      5 |      138 |           8.799 |             7.937 | [6.565, 8.963]   |      0.847 | 8.86e-85  |      409 | 9.75e-84  | yes, better   |         2.31e-58 | yes      |
| aeft          | 712 |    568 |      8 |      136 |           6.598 |             4.643 | [3.769, 5.631]   |      0.805 | 2.31e-76  |      409 | 2.08e-75  | yes, better   |         3.31e-51 | yes      |
| pmms          | 712 |    588 |     12 |      112 |           6.83  |             4.905 | [4.123, 5.762]   |      0.851 | 1.06e-84  |      409 | 1.06e-83  | yes, better   |         1.52e-56 | yes      |
| ppts          | 712 |    644 |      6 |       62 |           8.592 |             7.105 | [6.343, 7.730]   |      0.913 | 5.08e-98  |      409 | 6.09e-97  | yes, better   |         2.33e-60 | yes      |
| ippts         | 712 |    695 |      3 |       14 |          28.007 |            23.176 | [21.247, 26.314] |      0.997 | 6.11e-117 |      409 | 9.16e-116 | yes, better   |         1.55e-67 | yes      |
| dsla          | 712 |    358 |     30 |      324 |           0.388 |             0.039 | [0.000, 0.245]   |      0.095 | 0.0312    |      409 | 0.0312    | yes, better   |         0.0449   | yes      |
