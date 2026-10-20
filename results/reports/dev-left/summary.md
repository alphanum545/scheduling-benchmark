# Benchmark report: dev-left

Algorithms: bla_heft, left, left_d, heft, oppts, aeft, bppts, lstd  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| bla_heft |  712 |                   |
| left     |  712 |                   |
| left_d   |  712 |                   |
| heft     |  712 |                   |
| oppts    |  712 |                   |
| aeft     |  712 |                   |
| bppts    |  712 |                   |
| lstd     |  712 |                   |

## Overall averages
| algo     |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|--------:|----------:|-------------:|------------:|
| bla_heft |    4520.76 |  8.9948 |   22.9455 |       1.3953 |      0.0317 |
| left     |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.0337 |
| left_d   |    4490.15 |  9.602  |   20.4016 |       1.2764 |      0.0347 |
| heft     |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0998 |
| oppts    |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.1118 |
| aeft     |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.1048 |
| bppts    |    5425.34 | 11.6023 |   16.4928 |       0.9719 |      0.0331 |
| lstd     |    4687.12 |  9.639  |   21.4261 |       1.3183 |      0.1051 |

## bla_heft versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| left     |        79.92 |        0.56 |       19.52 |          7.364 |
| left_d   |        79.92 |        0.28 |       19.8  |          7.295 |
| heft     |        82.16 |        2.11 |       15.73 |          5.269 |
| oppts    |        74.72 |        2.25 |       23.03 |          4.254 |
| aeft     |        79.07 |        1.26 |       19.66 |          5.203 |
| bppts    |        94.1  |        0.14 |        5.76 |         20.076 |
| lstd     |        86.8  |        0.84 |       12.36 |          5.791 |

## Statistical significance

Friedman test over 8 algorithms on 712 instances: chi2 = 1530.2, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.393 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| bla_heft |          2.196 | —                   |
| oppts    |          3.853 | yes                 |
| aeft     |          4.178 | yes                 |
| heft     |          4.18  | yes                 |
| lstd     |          4.529 | yes                 |
| left_d   |          4.995 | yes                 |
| left     |          5.083 | yes                 |
| bppts    |          6.987 | yes                 |

### Wilcoxon signed-rank: bla_heft vs each algorithm (Holm-corrected, two-sided)

gain = (other - bla_heft) / other; positive = bla_heft has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| left     | 712 |    569 |      4 |      139 |           7.364 |             6.574 | [5.695, 7.432]   |      0.765 | 1.13e-69  |      409 | 3.39e-69  | yes, better   |         5.65e-43 | yes      |
| left_d   | 712 |    569 |      2 |      141 |           7.295 |             6.452 | [5.556, 7.323]   |      0.761 | 4.52e-69  |      409 | 9.04e-69  | yes, better   |         8.35e-43 | yes      |
| heft     | 712 |    585 |     15 |      112 |           5.269 |             3.603 | [3.257, 4.123]   |      0.811 | 7.96e-77  |      409 | 3.98e-76  | yes, better   |         3.9e-51  | yes      |
| oppts    | 712 |    532 |     16 |      164 |           4.254 |             2.898 | [2.378, 3.357]   |      0.691 | 3.1e-56   |      409 | 3.1e-56   | yes, better   |         1.01e-40 | yes      |
| aeft     | 712 |    563 |      9 |      140 |           5.203 |             3.553 | [2.861, 4.274]   |      0.783 | 2.2e-72   |      409 | 8.81e-72  | yes, better   |         8.08e-50 | yes      |
| bppts    | 712 |    670 |      1 |       41 |          20.076 |            16.345 | [15.172, 17.721] |      0.961 | 2.97e-109 |      409 | 2.08e-108 | yes, better   |         1.03e-65 | yes      |
| lstd     | 712 |    618 |      6 |       88 |           5.791 |             4.661 | [4.102, 5.158]   |      0.854 | 3.95e-86  |      409 | 2.37e-85  | yes, better   |         7.37e-55 | yes      |
