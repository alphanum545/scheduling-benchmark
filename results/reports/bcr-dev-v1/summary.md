# Benchmark report: bcr-dev-v1

Algorithms: bcr_dsla, scct, dsla, heft, left, oppts, aeft, peft, bcr_scct_seed, bcr_dsla_seed, bcr_scct, bcr_dsla_single, bcr_dsla_strict  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo            |   ok | example_problem   |
|:----------------|-----:|:------------------|
| bcr_dsla        |  712 |                   |
| scct            |  712 |                   |
| dsla            |  712 |                   |
| heft            |  712 |                   |
| left            |  712 |                   |
| oppts           |  712 |                   |
| aeft            |  712 |                   |
| peft            |  712 |                   |
| bcr_scct_seed   |  712 |                   |
| bcr_dsla_seed   |  712 |                   |
| bcr_scct        |  712 |                   |
| bcr_dsla_single |  712 |                   |
| bcr_dsla_strict |  712 |                   |

## Overall averages
| algo            |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:----------------|-----------:|--------:|----------:|-------------:|------------:|
| bcr_dsla        |    4298    |  8.5917 |   24.1528 |       1.446  |      0.0712 |
| scct            |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0366 |
| dsla            |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.036  |
| heft            |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0918 |
| left            |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.0347 |
| oppts           |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0994 |
| aeft            |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.098  |
| peft            |    4905.46 | 10.0312 |   20.127  |       1.1942 |      0.0996 |
| bcr_scct_seed   |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0371 |
| bcr_dsla_seed   |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.0361 |
| bcr_scct        |    4530.77 |  8.8626 |   23.1969 |       1.3895 |      0.0829 |
| bcr_dsla_single |    4318.83 |  8.7538 |   23.5481 |       1.4279 |      0.0444 |
| bcr_dsla_strict |    4303.81 |  8.6383 |   23.8963 |       1.4346 |      0.0594 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo            |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:----------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| bcr_dsla        |       0.711  |      2.4992 |        0.5205 |      1.0398 |   0.0165 |   52455.2 |      12.617  |
| scct            |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| dsla            |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |
| heft            |       0.7694 |      2.2526 |        0.5204 |      1.0288 |   0.0183 |   44319.5 |      11.1233 |
| left            |       0.7182 |      2.3751 |        0.4519 |      1.0318 |   0.0189 |   51806.8 |      10.8583 |
| oppts           |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| aeft            |       0.7216 |      2.1409 |        0.5063 |      1.026  |   0.0184 |   42171.4 |      10.7195 |
| peft            |       0.6899 |      2.1941 |        0.5505 |      1.026  |   0.0176 |   45086.3 |      10.4542 |
| bcr_scct_seed   |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| bcr_dsla_seed   |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |
| bcr_scct        |       0.7122 |      2.131  |        0.5294 |      1.0379 |   0.0165 |   41201.8 |      11.6938 |
| bcr_dsla_single |       0.7168 |      2.3422 |        0.515  |      1.0367 |   0.0173 |   52374.1 |      12.2985 |
| bcr_dsla_strict |       0.7128 |      2.4977 |        0.5185 |      1.0395 |   0.0166 |   52501.8 |      12.5539 |

## bcr_dsla versus each algorithm
| versus          |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:----------------|-------------:|------------:|------------:|---------------:|
| scct            |        89.47 |        0.98 |        9.55 |          7.409 |
| dsla            |        90.17 |        9.83 |        0    |          3.227 |
| heft            |        96.91 |        0.56 |        2.53 |          9.26  |
| left            |        95.37 |        0.42 |        4.21 |         11.325 |
| oppts           |        92.42 |        0.98 |        6.6  |          8.362 |
| aeft            |        94.1  |        0.84 |        5.06 |          9.242 |
| peft            |        98.17 |        0.14 |        1.69 |         13.006 |
| bcr_scct_seed   |        89.47 |        0.98 |        9.55 |          7.409 |
| bcr_dsla_seed   |        90.17 |        9.83 |        0    |          3.227 |
| bcr_scct        |        68.96 |        3.79 |       27.25 |          2.843 |
| bcr_dsla_single |        52.81 |       45.79 |        1.4  |          1.758 |
| bcr_dsla_strict |        43.96 |       51.97 |        4.07 |          0.572 |

## Statistical significance

Friedman test over 13 algorithms on 712 instances: chi2 = 4829.5, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.684 ranks.

| algo            |   average_rank | differs_from_best   |
|:----------------|---------------:|:--------------------|
| bcr_dsla        |          2.35  | —                   |
| bcr_dsla_strict |          3.045 | yes                 |
| bcr_dsla_single |          3.712 | yes                 |
| bcr_scct        |          4.554 | yes                 |
| bcr_dsla_seed   |          6.004 | yes                 |
| dsla            |          6.004 | yes                 |
| scct            |          8.11  | yes                 |
| bcr_scct_seed   |          8.11  | yes                 |
| oppts           |          9.033 | yes                 |
| heft            |          9.376 | yes                 |
| aeft            |          9.584 | yes                 |
| left            |         10.154 | yes                 |
| peft            |         10.966 | yes                 |

### Wilcoxon signed-rank: bcr_dsla vs each algorithm (Holm-corrected, two-sided)

gain = (other - bcr_dsla) / other; positive = bcr_dsla has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus          |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:----------------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| scct            | 712 |    637 |      7 |       68 |           7.409 |             6.296 | [5.396, 7.221]   |      0.941 | 5.42e-104 |      409 | 2.71e-103 | yes, better   |         1.34e-63 | yes      |
| dsla            | 712 |    642 |     70 |        0 |           3.227 |             1.712 | [1.473, 2.090]   |      1     | 8.35e-107 |      409 | 5.85e-106 | yes, better   |         1.75e-64 | yes      |
| heft            | 712 |    690 |      4 |       18 |           9.26  |             7.36  | [6.979, 7.813]   |      0.99  | 2.68e-115 |      409 | 2.95e-114 | yes, better   |         6.42e-67 | yes      |
| left            | 712 |    679 |      3 |       30 |          11.325 |            10.214 | [9.102, 11.370]  |      0.984 | 5e-114    |      409 | 5e-113    | yes, better   |         7.34e-67 | yes      |
| oppts           | 712 |    658 |      7 |       47 |           8.362 |             7.111 | [6.186, 8.103]   |      0.969 | 5.16e-110 |      409 | 4.13e-109 | yes, better   |         1.67e-64 | yes      |
| aeft            | 712 |    670 |      6 |       36 |           9.242 |             8.074 | [7.200, 8.880]   |      0.98  | 1.07e-112 |      409 | 9.63e-112 | yes, better   |         3.76e-66 | yes      |
| peft            | 712 |    699 |      1 |       12 |          13.006 |            11.239 | [10.656, 11.830] |      0.994 | 1.05e-116 |      409 | 1.26e-115 | yes, better   |         2.38e-67 | yes      |
| bcr_scct_seed   | 712 |    637 |      7 |       68 |           7.409 |             6.296 | [5.396, 7.221]   |      0.941 | 5.42e-104 |      409 | 2.71e-103 | yes, better   |         1.34e-63 | yes      |
| bcr_dsla_seed   | 712 |    642 |     70 |        0 |           3.227 |             1.712 | [1.473, 2.090]   |      1     | 8.35e-107 |      409 | 5.85e-106 | yes, better   |         1.75e-64 | yes      |
| bcr_scct        | 712 |    491 |     27 |      194 |           2.843 |             1.505 | [1.122, 1.912]   |      0.578 | 3.13e-39  |      409 | 3.13e-39  | yes, better   |         3.49e-31 | yes      |
| bcr_dsla_single | 712 |    376 |    326 |       10 |           1.758 |             0.055 | [0.000, 0.134]   |      0.978 | 3.14e-62  |      409 | 9.42e-62  | yes, better   |         4.18e-46 | yes      |
| bcr_dsla_strict | 712 |    313 |    370 |       29 |           0.572 |             0     | [0.000, 0.000]   |      0.877 | 6.6e-45   |      409 | 1.32e-44  | yes, better   |         4.49e-33 | yes      |
