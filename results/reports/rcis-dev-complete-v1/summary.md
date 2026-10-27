# Benchmark report: rcis-dev-complete-v1

Algorithms: rcis, rcis_single, rcis_append, rcis_no_common_target, heft, peft, ppts, ippts, ipeft, mpeft, aeft, oppts, left, scct, dsla  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo                  |   ok | example_problem   |
|:----------------------|-----:|:------------------|
| rcis                  |  712 |                   |
| rcis_single           |  712 |                   |
| rcis_append           |  712 |                   |
| rcis_no_common_target |  712 |                   |
| heft                  |  712 |                   |
| peft                  |  712 |                   |
| ppts                  |  712 |                   |
| ippts                 |  712 |                   |
| ipeft                 |  712 |                   |
| mpeft                 |  712 |                   |
| aeft                  |  712 |                   |
| oppts                 |  712 |                   |
| left                  |  712 |                   |
| scct                  |  712 |                   |
| dsla                  |  712 |                   |

## Overall averages
| algo                  |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:----------------------|-----------:|--------:|----------:|-------------:|------------:|
| rcis                  |    4698.88 |  9.9287 |   20.3961 |       1.2078 |      0.0413 |
| rcis_single           |    4545.55 |  9.7797 |   20.2998 |       1.2616 |      0.036  |
| rcis_append           |    5151.62 | 10.8435 |   18.2553 |       1.0747 |      0.0373 |
| rcis_no_common_target |    4708.24 | 10.0986 |   19.9146 |       1.1973 |      0.0391 |
| heft                  |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0983 |
| peft                  |    4905.46 | 10.0312 |   20.127  |       1.1942 |      0.1085 |
| ppts                  |    4784.25 |  9.732  |   20.474  |       1.2396 |      0.107  |
| ippts                 |    5710.9  | 12.9415 |   14.0713 |       0.8498 |      0.1087 |
| ipeft                 |    4814.18 |  9.8459 |   20.7271 |       1.2358 |      0.1141 |
| mpeft                 |    4888.88 |  9.9375 |   20.4209 |       1.2065 |      0.1292 |
| aeft                  |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.1052 |
| oppts                 |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.1077 |
| left                  |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.0362 |
| scct                  |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0393 |
| dsla                  |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.0399 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo                  |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:----------------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| rcis                  |       0.6431 |      2.3108 |        0.535  |      1.0316 |   0.0184 |   53680.8 |      10.3367 |
| rcis_single           |       0.7271 |      2.3172 |        0.5145 |      1.0332 |   0.0182 |   51142.7 |      10.8572 |
| rcis_append           |       0.639  |      2.1527 |        0.5167 |      1.0249 |   0.0183 |   48600.7 |       9.2515 |
| rcis_no_common_target |       0.6737 |      2.1789 |        0.5296 |      1.0337 |   0.0181 |   48987.7 |      10.4331 |
| heft                  |       0.7694 |      2.2526 |        0.5204 |      1.0288 |   0.0183 |   44319.5 |      11.1233 |
| peft                  |       0.6899 |      2.1941 |        0.5505 |      1.026  |   0.0176 |   45086.3 |      10.4542 |
| ppts                  |       0.6972 |      2.0806 |        0.5377 |      1.0261 |   0.0182 |   43736   |      10.3769 |
| ippts                 |       0.6879 |      1.8867 |        0.6055 |      1.0221 |   0.0171 |   43596.5 |       7.6734 |
| ipeft                 |       0.699  |      2.2883 |        0.5504 |      1.0266 |   0.018  |   46310.6 |      10.8622 |
| mpeft                 |       0.6822 |      2.193  |        0.5557 |      1.0263 |   0.0173 |   45209.1 |      10.5721 |
| aeft                  |       0.7216 |      2.1409 |        0.5063 |      1.026  |   0.0184 |   42171.4 |      10.7195 |
| oppts                 |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| left                  |       0.7182 |      2.3751 |        0.4519 |      1.0318 |   0.0189 |   51806.8 |      10.8583 |
| scct                  |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| dsla                  |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |

## rcis versus each algorithm
| versus                |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:----------------------|-------------:|------------:|------------:|---------------:|
| rcis_single           |        35.67 |        1.12 |       63.2  |         -1.395 |
| rcis_append           |        79.92 |       16.99 |        3.09 |          7.886 |
| rcis_no_common_target |        45.37 |       18.54 |       36.1  |          1.346 |
| heft                  |        36.1  |        0.84 |       63.06 |         -4.082 |
| peft                  |        60.25 |        0.28 |       39.47 |          0.879 |
| ppts                  |        36.52 |        0.28 |       63.2  |         -1.351 |
| ippts                 |        94.24 |        0.28 |        5.48 |         21.329 |
| ipeft                 |        43.96 |        0.84 |       55.2  |         -1.115 |
| mpeft                 |        55.34 |        0.56 |       44.1  |          0.136 |
| aeft                  |        35.11 |        0.28 |       64.61 |         -3.846 |
| oppts                 |        29.78 |        0.84 |       69.38 |         -4.831 |
| left                  |        47.33 |        0.14 |       52.53 |         -1.497 |
| scct                  |        28.79 |        0.42 |       70.79 |         -6.034 |
| dsla                  |        13.2  |        0.7  |       86.1  |        -11.072 |

## Statistical significance

Friedman test over 15 algorithms on 712 instances: chi2 = 3614.2, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.804 ranks.

| algo                  |   average_rank | differs_from_best   |
|:----------------------|---------------:|:--------------------|
| dsla                  |          2.686 | —                   |
| scct                  |          4.933 | yes                 |
| oppts                 |          5.848 | yes                 |
| aeft                  |          6.625 | yes                 |
| heft                  |          6.705 | yes                 |
| ipeft                 |          7.719 | yes                 |
| rcis_single           |          7.728 | yes                 |
| ppts                  |          7.951 | yes                 |
| left                  |          8.323 | yes                 |
| rcis                  |          8.374 | yes                 |
| mpeft                 |          8.723 | yes                 |
| rcis_no_common_target |          9.138 | yes                 |
| peft                  |          9.393 | yes                 |
| rcis_append           |         11.788 | yes                 |
| ippts                 |         14.068 | yes                 |

### Wilcoxon signed-rank: rcis vs each algorithm (Holm-corrected, two-sided)

gain = (other - rcis) / other; positive = rcis has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus                |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:----------------------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| rcis_single           | 712 |    254 |      8 |      450 |          -1.395 |            -1.5   | [-1.938, -1.038] |     -0.222 | 3.3e-07   |      409 | 1.98e-06  | yes, worse    |         2.01e-05 | yes      |
| rcis_append           | 712 |    569 |    121 |       22 |           7.886 |             5.975 | [5.386, 6.718]   |      0.982 | 5.71e-95  |      409 | 7.42e-94  | yes, better   |         1.85e-59 | yes      |
| rcis_no_common_target | 712 |    323 |    132 |      257 |           1.346 |             0     | [0.000, 0.000]   |      0.235 | 9.29e-07  |      409 | 4.64e-06  | yes, better   |         5.16e-05 | yes      |
| heft                  | 712 |    257 |      6 |      449 |          -4.082 |            -1.754 | [-2.559, -1.345] |     -0.302 | 3.58e-12  |      409 | 2.86e-11  | yes, worse    |         1.12e-07 | yes      |
| peft                  | 712 |    429 |      2 |      281 |           0.879 |             1.141 | [0.820, 1.595]   |      0.189 | 1.26e-05  |      409 | 5.06e-05  | yes, better   |         0.000435 | yes      |
| ppts                  | 712 |    260 |      2 |      450 |          -1.351 |            -1.977 | [-2.689, -1.228] |     -0.226 | 1.79e-07  |      409 | 1.25e-06  | yes, worse    |         0.00161  | yes      |
| ippts                 | 712 |    671 |      2 |       39 |          21.329 |            19.625 | [17.607, 21.460] |      0.971 | 3.08e-111 |      409 | 4.31e-110 | yes, better   |         1.67e-65 | yes      |
| ipeft                 | 712 |    313 |      6 |      393 |          -1.115 |            -0.583 | [-0.858, -0.218] |     -0.115 | 0.00835   |      409 | 0.0251    | yes, worse    |         0.117    | no       |
| mpeft                 | 712 |    394 |      4 |      314 |           0.136 |             0.587 | [0.210, 0.969]   |      0.073 | 0.0931    |      409 | 0.0931    | no            |         0.0754   | no       |
| aeft                  | 712 |    250 |      2 |      460 |          -3.846 |            -3.379 | [-4.962, -1.717] |     -0.388 | 3.53e-19  |      409 | 3.18e-18  | yes, worse    |         4.3e-11  | yes      |
| oppts                 | 712 |    212 |      6 |      494 |          -4.831 |            -5.059 | [-5.982, -3.754] |     -0.495 | 4.48e-30  |      409 | 4.48e-29  | yes, worse    |         2.3e-17  | yes      |
| left                  | 712 |    337 |      1 |      374 |          -1.497 |            -0.927 | [-2.540, 0.369]  |     -0.109 | 0.0121    |      409 | 0.0251    | yes, worse    |         0.105    | no       |
| scct                  | 712 |    205 |      3 |      504 |          -6.034 |            -5.766 | [-6.977, -4.451] |     -0.542 | 8.04e-36  |      409 | 8.85e-35  | yes, worse    |         3.45e-22 | yes      |
| dsla                  | 712 |     94 |      5 |      613 |         -11.072 |            -8.454 | [-9.556, -7.192] |     -0.853 | 4.72e-86  |      409 | 5.66e-85  | yes, worse    |         6.84e-57 | yes      |
