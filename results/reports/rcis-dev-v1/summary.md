# Benchmark report: rcis-dev-v1

Algorithms: rcis, rcis_single, rcis_append, rcis_no_common_target, heft, peft, ppts, ippts, ipeft, lstd, mpeft, aeft, oppts, left, scct, dsla  
Instances compared: 500 of 712 (only those where every algorithm is valid)

## Validity
| algo                  |   invalid |   ok | example_problem                        |
|:----------------------|----------:|-----:|:---------------------------------------|
| rcis                  |         0 |  712 |                                        |
| rcis_single           |         0 |  712 |                                        |
| rcis_append           |         0 |  712 |                                        |
| rcis_no_common_target |         0 |  712 |                                        |
| heft                  |         0 |  712 |                                        |
| peft                  |         0 |  712 |                                        |
| ppts                  |         0 |  712 |                                        |
| ippts                 |         0 |  712 |                                        |
| ipeft                 |         0 |  712 |                                        |
| lstd                  |       212 |  500 | t1->t2 starts 50 before data ready 102 |
| mpeft                 |         0 |  712 |                                        |
| aeft                  |         0 |  712 |                                        |
| oppts                 |         0 |  712 |                                        |
| left                  |         0 |  712 |                                        |
| scct                  |         0 |  712 |                                        |
| dsla                  |         0 |  712 |                                        |

## Overall averages
| algo                  |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:----------------------|-----------:|--------:|----------:|-------------:|------------:|
| rcis                  |    4812.81 | 10.6143 |   18.2481 |       1.3149 |      0.0425 |
| rcis_single           |    4611.85 | 10.2029 |   18.6836 |       1.394  |      0.0371 |
| rcis_append           |    5203.92 | 11.5864 |   16.2681 |       1.175  |      0.0383 |
| rcis_no_common_target |    4817.98 | 10.7032 |   18.0628 |       1.3109 |      0.0405 |
| heft                  |    4656.99 | 10.0325 |   19.6963 |       1.4602 |      0.1007 |
| peft                  |    4923.95 | 10.6577 |   18.1928 |       1.3115 |      0.1089 |
| ppts                  |    4753.87 | 10.254  |   18.564  |       1.365  |      0.1082 |
| ippts                 |    5881.25 | 13.8642 |   12.4554 |       0.9333 |      0.1115 |
| ipeft                 |    4836.83 | 10.4312 |   18.7712 |       1.3528 |      0.1148 |
| lstd                  |    4689.14 | 10.0775 |   19.5758 |       1.4462 |      0.0995 |
| mpeft                 |    4902.88 | 10.559  |   18.4723 |       1.3266 |      0.1283 |
| aeft                  |    4609.6  |  9.8454 |   19.1767 |       1.428  |      0.1065 |
| oppts                 |    4604.67 |  9.8268 |   19.3921 |       1.4378 |      0.1084 |
| left                  |    4531.84 |  9.9468 |   18.712  |       1.4121 |      0.0371 |
| scct                  |    4551.62 |  9.6867 |   19.7487 |       1.4653 |      0.0394 |
| dsla                  |    4389.95 |  9.3626 |   20.915  |       1.5372 |      0.0403 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo                  |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:----------------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| rcis                  |       0.6515 |      1.7553 |        0.6181 |      1.029  |   0.0168 |   36075.4 |       9.6752 |
| rcis_single           |       0.7329 |      1.807  |        0.6007 |      1.0302 |   0.0166 |   35517.4 |      10.3884 |
| rcis_append           |       0.6376 |      1.7071 |        0.5973 |      1.0225 |   0.0167 |   33629   |       8.7076 |
| rcis_no_common_target |       0.6726 |      1.7419 |        0.6129 |      1.0309 |   0.0166 |   33849.6 |       9.8936 |
| heft                  |       0.7648 |      1.756  |        0.6119 |      1.0257 |   0.0167 |   31647.2 |      10.7369 |
| peft                  |       0.6912 |      1.7831 |        0.6359 |      1.0226 |   0.0162 |   31941.6 |      10.0309 |
| ppts                  |       0.6935 |      1.7547 |        0.6258 |      1.0228 |   0.0167 |   31258.3 |       9.9804 |
| ippts                 |       0.685  |      1.5914 |        0.6854 |      1.019  |   0.0159 |   31648.5 |       7.0793 |
| ipeft                 |       0.7013 |      1.8252 |        0.6374 |      1.0237 |   0.0164 |   32462.9 |      10.4154 |
| lstd                  |       0.7623 |      1.7315 |        0.605  |      1.0233 |   0.0167 |   31771.9 |      10.617  |
| mpeft                 |       0.6843 |      1.7909 |        0.6407 |      1.0233 |   0.0159 |   32171.8 |      10.1597 |
| aeft                  |       0.7164 |      1.7783 |        0.5954 |      1.0226 |   0.0169 |   30334   |      10.4039 |
| oppts                 |       0.7001 |      1.7771 |        0.6034 |      1.0223 |   0.0169 |   31444.7 |      10.4842 |
| left                  |       0.7187 |      1.8592 |        0.5339 |      1.029  |   0.0173 |   35589.4 |      10.3176 |
| scct                  |       0.7169 |      1.7206 |        0.6011 |      1.0233 |   0.0169 |   29935.3 |      10.7191 |
| dsla                  |       0.7259 |      1.7899 |        0.5956 |      1.0279 |   0.0167 |   35971.3 |      11.4895 |

## rcis versus each algorithm
| versus                |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:----------------------|-------------:|------------:|------------:|---------------:|
| rcis_single           |         27.4 |         1.2 |        71.4 |         -3.312 |
| rcis_append           |         81.8 |        14.4 |         3.8 |          7.787 |
| rcis_no_common_target |         38.8 |        25.2 |        36   |          0.647 |
| heft                  |         25.6 |         0.6 |        73.8 |         -6.289 |
| peft                  |         55.4 |         0.4 |        44.2 |          0.02  |
| ppts                  |         26.8 |         0.4 |        72.8 |         -2.862 |
| ippts                 |         95.4 |         0.4 |         4.2 |         21.889 |
| ipeft                 |         36.4 |         1   |        62.6 |         -2.12  |
| lstd                  |         33.8 |         0.6 |        65.6 |         -5.402 |
| mpeft                 |         51.4 |         0.4 |        48.2 |         -0.752 |
| aeft                  |         24   |         0.4 |        75.6 |         -6.2   |
| oppts                 |         20.2 |         0.6 |        79.2 |         -6.919 |
| left                  |         40   |         0   |        60   |         -3.804 |
| scct                  |         19.4 |         0   |        80.6 |         -8.31  |
| dsla                  |          8.8 |         0.4 |        90.8 |        -12.429 |

## Statistical significance

Friedman test over 16 algorithms on 500 instances: chi2 = 3199.7, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 1.032 ranks.

| algo                  |   average_rank | differs_from_best   |
|:----------------------|---------------:|:--------------------|
| dsla                  |          2.623 | —                   |
| scct                  |          4.645 | yes                 |
| oppts                 |          5.688 | yes                 |
| aeft                  |          6.269 | yes                 |
| heft                  |          6.788 | yes                 |
| lstd                  |          7.793 | yes                 |
| rcis_single           |          8.086 | yes                 |
| ppts                  |          8.311 | yes                 |
| ipeft                 |          8.469 | yes                 |
| left                  |          8.474 | yes                 |
| mpeft                 |          9.752 | yes                 |
| rcis                  |          9.918 | yes                 |
| rcis_no_common_target |         10.255 | yes                 |
| peft                  |         10.45  | yes                 |
| rcis_append           |         13.162 | yes                 |
| ippts                 |         15.317 | yes                 |

### Wilcoxon signed-rank: rcis vs each algorithm (Holm-corrected, two-sided)

gain = (other - rcis) / other; positive = rcis has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus                |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95       |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:----------------------|----:|-------:|-------:|---------:|----------------:|------------------:|:------------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| rcis_single           | 500 |    137 |      6 |      357 |          -3.312 |            -2.558 | [-3.135, -1.921]  |     -0.466 |  3.08e-19 |      333 | 1.85e-18 | yes, worse    |         2.51e-14 | yes      |
| rcis_append           | 500 |    409 |     72 |       19 |           7.787 |             5.87  | [5.306, 6.631]    |      0.976 |  1.42e-68 |      333 | 1.85e-67 | yes, better   |         2.14e-48 | yes      |
| rcis_no_common_target | 500 |    194 |    126 |      180 |           0.647 |             0     | [0.000, 0.000]    |      0.114 |  0.0564   |      333 | 0.169    | no            |         0.119    | no       |
| heft                  | 500 |    128 |      3 |      369 |          -6.289 |            -3.879 | [-4.841, -2.591]  |     -0.553 |  1.21e-26 |      333 | 1.09e-25 | yes, worse    |         3.99e-18 | yes      |
| peft                  | 500 |    277 |      2 |      221 |           0.02  |             0.706 | [0.235, 1.026]    |      0.086 |  0.0979   |      333 | 0.196    | no            |         0.119    | no       |
| ppts                  | 500 |    134 |      2 |      364 |          -2.862 |            -3.07  | [-3.642, -2.391]  |     -0.476 |  3.22e-20 |      333 | 2.58e-19 | yes, worse    |         2.95e-12 | yes      |
| ippts                 | 500 |    477 |      2 |       21 |          21.889 |            20.469 | [17.936, 23.553]  |      0.976 |  1.8e-79  |      333 | 2.7e-78  | yes, better   |         7.82e-54 | yes      |
| ipeft                 | 500 |    182 |      5 |      313 |          -2.12  |            -1.009 | [-1.526, -0.705]  |     -0.295 |  1.27e-08 |      333 | 5.09e-08 | yes, worse    |         0.000107 | yes      |
| lstd                  | 500 |    169 |      3 |      328 |          -5.402 |            -3.482 | [-4.365, -2.142]  |     -0.469 |  1.42e-19 |      333 | 9.93e-19 | yes, worse    |         2.91e-13 | yes      |
| mpeft                 | 500 |    257 |      2 |      241 |          -0.752 |             0.122 | [-0.307, 0.529]   |     -0.047 |  0.359    |      333 | 0.359    | no            |         0.887    | no       |
| aeft                  | 500 |    120 |      2 |      378 |          -6.2   |            -6.759 | [-7.802, -5.172]  |     -0.632 |  2.44e-34 |      333 | 2.44e-33 | yes, worse    |         2.3e-22  | yes      |
| oppts                 | 500 |    101 |      3 |      396 |          -6.919 |            -6.82  | [-7.655, -5.938]  |     -0.713 |  4e-43    |      333 | 4.4e-42  | yes, worse    |         3.19e-28 | yes      |
| left                  | 500 |    200 |      0 |      300 |          -3.804 |            -4.104 | [-5.655, -2.571]  |     -0.319 |  6.52e-10 |      333 | 3.26e-09 | yes, worse    |         5.63e-07 | yes      |
| scct                  | 500 |     97 |      0 |      403 |          -8.31  |            -7.868 | [-8.905, -6.995]  |     -0.733 |  8.33e-46 |      333 | 1e-44    | yes, worse    |         6.37e-31 | yes      |
| dsla                  | 500 |     44 |      2 |      454 |         -12.429 |            -9.884 | [-11.175, -8.964] |     -0.919 |  1.43e-70 |      333 | 2e-69    | yes, worse    |         4.45e-49 | yes      |
