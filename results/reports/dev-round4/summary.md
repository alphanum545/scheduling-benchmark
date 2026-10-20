# Benchmark report: dev-round4

Algorithms: z_best10, bla_heft, z_e10, z_best25, z_auto2, z_auto3, z_auto5, left, left_d, heft, oppts, aeft  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| z_best10 |  712 |                   |
| bla_heft |  712 |                   |
| z_e10    |  712 |                   |
| z_best25 |  712 |                   |
| z_auto2  |  712 |                   |
| z_auto3  |  712 |                   |
| z_auto5  |  712 |                   |
| left     |  712 |                   |
| left_d   |  712 |                   |
| heft     |  712 |                   |
| oppts    |  712 |                   |
| aeft     |  712 |                   |

## Overall averages
| algo     |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|-------:|----------:|-------------:|------------:|
| z_best10 |    4340.09 | 8.8686 |   23.0137 |       1.4021 |      0.0327 |
| bla_heft |    4520.76 | 8.9948 |   22.9455 |       1.3953 |      0.0317 |
| z_e10    |    4459.37 | 9.3549 |   21.0926 |       1.2963 |      0.0312 |
| z_best25 |    4342.2  | 8.8563 |   23.0558 |       1.4031 |      0.0325 |
| z_auto2  |    4369.69 | 8.9362 |   22.9531 |       1.396  |      0.0296 |
| z_auto3  |    4369.69 | 8.9362 |   22.9531 |       1.396  |      0.0298 |
| z_auto5  |    4369.69 | 8.9362 |   22.9531 |       1.396  |      0.0295 |
| left     |    4493.97 | 9.5985 |   20.3849 |       1.2754 |      0.0337 |
| left_d   |    4490.15 | 9.602  |   20.4016 |       1.2764 |      0.0347 |
| heft     |    4668.99 | 9.6197 |   21.5457 |       1.3318 |      0.0998 |
| oppts    |    4671.13 | 9.387  |   21.2845 |       1.3025 |      0.1118 |
| aeft     |    4688.64 | 9.4375 |   21.0459 |       1.2909 |      0.1048 |

## z_best10 versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| bla_heft |        23.74 |       76.26 |        0    |          1.027 |
| z_e10    |        75.56 |       24.44 |        0    |          6.417 |
| z_best25 |        12.5  |       71.21 |       16.29 |         -0.108 |
| z_auto2  |        17.98 |       82.02 |        0    |          0.468 |
| z_auto3  |        17.98 |       82.02 |        0    |          0.468 |
| z_auto5  |        17.98 |       82.02 |        0    |          0.468 |
| left     |        88.62 |        2.25 |        9.13 |          8.463 |
| left_d   |        88.06 |        1.97 |        9.97 |          8.394 |
| heft     |        85.67 |        2.11 |       12.22 |          6.281 |
| oppts    |        79.07 |        2.39 |       18.54 |          5.246 |
| aeft     |        84.97 |        1.12 |       13.9  |          6.186 |

## Statistical significance

Friedman test over 12 algorithms on 712 instances: chi2 = 3016.6, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.624 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| z_best25 |          3.814 | —                   |
| z_best10 |          3.94  | no                  |
| z_auto3  |          4.793 | yes                 |
| z_auto2  |          4.793 | yes                 |
| z_auto5  |          4.793 | yes                 |
| bla_heft |          5.095 | yes                 |
| oppts    |          7.739 | yes                 |
| z_e10    |          8.237 | yes                 |
| aeft     |          8.296 | yes                 |
| heft     |          8.388 | yes                 |
| left_d   |          9.008 | yes                 |
| left     |          9.105 | yes                 |

### Wilcoxon signed-rank: z_best10 vs each algorithm (Holm-corrected, two-sided)

gain = (other - z_best10) / other; positive = z_best10 has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| bla_heft | 712 |    169 |    543 |        0 |           1.027 |             0     | [0.000, 0.000] |      1     | 1.75e-29  |      409 | 8.73e-29  | yes, better   |         1.62e-24 | yes      |
| z_e10    | 712 |    538 |    174 |        0 |           6.417 |             4.75  | [4.039, 5.700] |      1     | 7.9e-90   |      409 | 6.32e-89  | yes, better   |         2.04e-56 | yes      |
| z_best25 | 712 |     89 |    507 |      116 |          -0.108 |             0     | [0.000, 0.000] |     -0.15  | 0.0633    |      409 | 0.0633    | no            |         0.0821   | no       |
| z_auto2  | 712 |    128 |    584 |        0 |           0.468 |             0     | [0.000, 0.000] |      1     | 9.51e-23  |      409 | 3.81e-22  | yes, better   |         1.64e-19 | yes      |
| z_auto3  | 712 |    128 |    584 |        0 |           0.468 |             0     | [0.000, 0.000] |      1     | 9.51e-23  |      409 | 3.81e-22  | yes, better   |         1.64e-19 | yes      |
| z_auto5  | 712 |    128 |    584 |        0 |           0.468 |             0     | [0.000, 0.000] |      1     | 9.51e-23  |      409 | 3.81e-22  | yes, better   |         1.64e-19 | yes      |
| left     | 712 |    631 |     16 |       65 |           8.463 |             6.721 | [5.950, 7.622] |      0.957 | 4.86e-106 |      409 | 5.35e-105 | yes, better   |         4.3e-64  | yes      |
| left_d   | 712 |    627 |     14 |       71 |           8.394 |             6.717 | [5.915, 7.553] |      0.951 | 4.44e-105 |      409 | 4.44e-104 | yes, better   |         2.07e-63 | yes      |
| heft     | 712 |    610 |     15 |       87 |           6.281 |             4.973 | [4.283, 5.572] |      0.892 | 1.53e-92  |      409 | 1.37e-91  | yes, better   |         1.25e-59 | yes      |
| oppts    | 712 |    563 |     17 |      132 |           5.246 |             3.788 | [3.215, 4.230] |      0.773 | 9.71e-70  |      409 | 5.82e-69  | yes, better   |         2.54e-47 | yes      |
| aeft     | 712 |    605 |      8 |       99 |           6.186 |             4.445 | [3.817, 4.919] |      0.861 | 3.62e-87  |      409 | 2.53e-86  | yes, better   |         1.47e-55 | yes      |
