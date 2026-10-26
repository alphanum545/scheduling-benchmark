# Benchmark report: dsla-weight-ablation

Algorithms: dsla, dsla_fix, slas, slas_fix, heft, peft, oppts  
Instances compared: 2814 of 2814 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| dsla     | 2814 |                   |
| dsla_fix | 2814 |                   |
| slas     | 2814 |                   |
| slas_fix | 2814 |                   |
| heft     | 2814 |                   |
| peft     | 2814 |                   |
| oppts    | 2814 |                   |

## Overall averages
| algo     |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|-------:|----------:|-------------:|------------:|
| dsla     |    4469.6  | 8.822  |   22.1652 |       1.3845 |      0.0368 |
| dsla_fix |    4503.66 | 8.8893 |   21.8919 |       1.346  |      0.0326 |
| slas     |    4664.7  | 8.9551 |   22.107  |       1.3783 |      0.0334 |
| slas_fix |    4808.27 | 9.2538 |   21.6783 |       1.3126 |      0.0298 |
| heft     |    4831.49 | 9.5658 |   20.7725 |       1.3167 |      0.0895 |
| peft     |    5072.72 | 9.9806 |   19.3879 |       1.1739 |      0.1033 |
| oppts    |    4849.97 | 9.3535 |   20.541  |       1.285  |      0.1031 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo     |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:---------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| dsla     |       0.7126 |      2.4208 |        0.5125 |      1.0316 |   0.0177 |   55586.2 |      11.4288 |
| dsla_fix |       0.6894 |      2.4763 |        0.5179 |      1.0337 |   0.0175 |   55424.7 |      11.2378 |
| slas     |       0.7098 |      2.6293 |        0.5179 |      1.0278 |   0.0178 |   54160.8 |      11.1202 |
| slas_fix |       0.6859 |      2.7129 |        0.545  |      1.0282 |   0.0173 |   54515.7 |      10.8756 |
| heft     |       0.7642 |      2.2867 |        0.5232 |      1.0286 |   0.018  |   47596.9 |      10.4747 |
| peft     |       0.6834 |      2.2139 |        0.5515 |      1.0261 |   0.0175 |   48736.2 |       9.8425 |
| oppts    |       0.6997 |      2.1456 |        0.516  |      1.0262 |   0.018  |   48299.2 |      10.2239 |

## dsla versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| dsla_fix |        54.87 |       12.62 |       32.52 |          1.028 |
| slas     |        23.17 |       76.83 |        0    |          1.04  |
| slas_fix |        68.41 |        5.08 |       26.51 |          3.379 |
| heft     |        86.57 |        1.28 |       12.15 |          6.323 |
| peft     |        87.95 |        0.53 |       11.51 |         10.18  |
| oppts    |        79.46 |        1.39 |       19.15 |          5.445 |

## Statistical significance

Friedman test over 7 algorithms on 2814 instances: chi2 = 5895.6, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.170 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| dsla     |          2.507 | —                   |
| slas     |          2.977 | yes                 |
| dsla_fix |          3.012 | yes                 |
| slas_fix |          4.011 | yes                 |
| oppts    |          4.566 | yes                 |
| heft     |          4.94  | yes                 |
| peft     |          5.986 | yes                 |

### Wilcoxon signed-rank: dsla vs each algorithm (Holm-corrected, two-sided)

gain = (other - dsla) / other; positive = dsla has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|-----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| dsla_fix | 2814 |   1544 |    355 |      915 |           1.028 |             0.268 | [0.175, 0.342] |      0.325 | 3.42e-44  |      529 | 3.42e-44  | yes, better   |         4.73e-32 | yes      |
| slas     | 2814 |    652 |   2162 |        0 |           1.04  |             0     | [0.000, 0.000] |      1     | 1.95e-108 |      529 | 3.9e-108  | yes, better   |         2.4e-57  | yes      |
| slas_fix | 2814 |   1925 |    143 |      746 |           3.379 |             2.842 | [2.445, 3.208] |      0.612 | 5.44e-165 |      529 | 1.63e-164 | yes, better   |         4.18e-66 | yes      |
| heft     | 2814 |   2436 |     36 |      342 |           6.323 |             5.099 | [4.784, 5.347] |      0.897 | 0         |      529 | 0         | yes, better   |         1.05e-86 | yes      |
| peft     | 2814 |   2475 |     15 |      324 |          10.18  |             8.646 | [8.302, 8.960] |      0.895 | 0         |      529 | 0         | yes, better   |         7.79e-86 | yes      |
| oppts    | 2814 |   2236 |     39 |      539 |           5.445 |             3.737 | [3.493, 3.959] |      0.77  | 1.53e-270 |      529 | 6.14e-270 | yes, better   |         5.24e-77 | yes      |
