# Benchmark report: dev-baselines

Algorithms: bla_heft, bppts, heft, peft, ipeft, mpeft, ppts, ippts, aeft, oppts, lstd  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| bla_heft |  712 |                   |
| bppts    |  712 |                   |
| heft     |  712 |                   |
| peft     |  712 |                   |
| ipeft    |  712 |                   |
| mpeft    |  712 |                   |
| ppts     |  712 |                   |
| ippts    |  712 |                   |
| aeft     |  712 |                   |
| oppts    |  712 |                   |
| lstd     |  712 |                   |

## Overall averages
| algo     |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|--------:|----------:|-------------:|------------:|
| bla_heft |    4520.76 |  8.9948 |   22.9455 |       1.3953 |      0.0317 |
| bppts    |    5425.34 | 11.6023 |   16.4928 |       0.9719 |      0.0331 |
| heft     |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0998 |
| peft     |    4905.46 | 10.0312 |   20.127  |       1.1942 |      0.1112 |
| ipeft    |    4814.18 |  9.8459 |   20.7271 |       1.2358 |      0.1166 |
| mpeft    |    4888.88 |  9.9375 |   20.4209 |       1.2065 |      0.1382 |
| ppts     |    4784.25 |  9.732  |   20.474  |       1.2396 |      0.104  |
| ippts    |    5710.9  | 12.9415 |   14.0713 |       0.8498 |      0.1178 |
| aeft     |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.1048 |
| oppts    |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.1118 |
| lstd     |    4687.12 |  9.639  |   21.4261 |       1.3183 |      0.1051 |

## bla_heft versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| bppts    |        94.1  |        0.14 |        5.76 |         20.076 |
| heft     |        82.16 |        2.11 |       15.73 |          5.269 |
| peft     |        86.38 |        0.84 |       12.78 |          9.013 |
| ipeft    |        83.99 |        0.7  |       15.31 |          7.364 |
| mpeft    |        84.83 |        0.28 |       14.89 |          8.338 |
| ppts     |        86.94 |        0.56 |       12.5  |          7.164 |
| ippts    |        96.91 |        0    |        3.09 |         26.879 |
| aeft     |        79.07 |        1.26 |       19.66 |          5.203 |
| oppts    |        74.72 |        2.25 |       23.03 |          4.254 |
| lstd     |        86.8  |        0.84 |       12.36 |          5.791 |

## Statistical significance

Friedman test over 11 algorithms on 712 instances: chi2 = 3219.3, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.566 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| bla_heft |          2.396 | —                   |
| oppts    |          4.191 | yes                 |
| aeft     |          4.758 | yes                 |
| heft     |          5.013 | yes                 |
| lstd     |          5.483 | yes                 |
| ipeft    |          5.624 | yes                 |
| ppts     |          5.841 | yes                 |
| mpeft    |          6.254 | yes                 |
| peft     |          6.779 | yes                 |
| bppts    |          9.4   | yes                 |
| ippts    |         10.26  | yes                 |

### Wilcoxon signed-rank: bla_heft vs each algorithm (Holm-corrected, two-sided)

gain = (other - bla_heft) / other; positive = bla_heft has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| bppts    | 712 |    670 |      1 |       41 |          20.076 |            16.345 | [15.172, 17.721] |      0.961 | 2.97e-109 |      409 | 2.68e-108 | yes, better   |         1.32e-65 | yes      |
| heft     | 712 |    585 |     15 |      112 |           5.269 |             3.603 | [3.257, 4.123]   |      0.811 | 7.96e-77  |      409 | 2.39e-76  | yes, better   |         3.12e-51 | yes      |
| peft     | 712 |    615 |      6 |       91 |           9.013 |             7.255 | [6.681, 7.924]   |      0.857 | 1.2e-86   |      409 | 8.43e-86  | yes, better   |         1.36e-54 | yes      |
| ipeft    | 712 |    598 |      5 |      109 |           7.364 |             5.873 | [5.201, 6.531]   |      0.822 | 5.95e-80  |      409 | 2.97e-79  | yes, better   |         7.65e-51 | yes      |
| mpeft    | 712 |    604 |      2 |      106 |           8.338 |             6.459 | [5.848, 7.154]   |      0.816 | 3.81e-79  |      409 | 1.52e-78  | yes, better   |         1.59e-51 | yes      |
| ppts     | 712 |    619 |      4 |       89 |           7.164 |             5.335 | [4.798, 5.902]   |      0.858 | 4.6e-87   |      409 | 3.68e-86  | yes, better   |         2.97e-55 | yes      |
| ippts    | 712 |    690 |      0 |       22 |          26.879 |            22.295 | [20.226, 25.426] |      0.991 | 3.4e-116  |      409 | 3.4e-115  | yes, better   |         2e-67    | yes      |
| aeft     | 712 |    563 |      9 |      140 |           5.203 |             3.553 | [2.861, 4.274]   |      0.783 | 2.2e-72   |      409 | 4.41e-72  | yes, better   |         4.04e-50 | yes      |
| oppts    | 712 |    532 |     16 |      164 |           4.254 |             2.898 | [2.378, 3.357]   |      0.691 | 3.1e-56   |      409 | 3.1e-56   | yes, better   |         1.01e-40 | yes      |
| lstd     | 712 |    618 |      6 |       88 |           5.791 |             4.661 | [4.102, 5.158]   |      0.854 | 3.95e-86  |      409 | 2.37e-85  | yes, better   |         8.6e-55  | yes      |
