# Benchmark report: cgs-v3-dev

Algorithms: cgs, la_heft, heft_ins  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| cgs      |  712 |                   |
| la_heft  |  712 |                   |
| heft_ins |  712 |                   |

## Overall averages
| algo     |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|-------:|----------:|-------------:|------------:|
| cgs      |    4352.14 | 8.8194 |   23.7392 |       1.4339 |      0.037  |
| la_heft  |    4802.1  | 9.6408 |   22.0328 |       1.2847 |      0.1562 |
| heft_ins |    4479.08 | 9.453  |   21.9582 |       1.3623 |      0.03   |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo     |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:---------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| cgs      |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| la_heft  |       0.6497 |      2.7219 |        0.5551 |      1.0287 |   0.0177 |   49475.1 |      10.7889 |
| heft_ins |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |

## cgs versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| la_heft  |        81.74 |        0.56 |       17.7  |          6.707 |
| heft_ins |        84.55 |        0.84 |       14.61 |          5.523 |

## Statistical significance

Friedman test over 3 algorithms on 712 instances: chi2 = 490.4, p = 3.26e-107; Nemenyi critical difference (alpha 0.05) = 0.124 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| cgs      |          1.33  | —                   |
| heft_ins |          2.256 | yes                 |
| la_heft  |          2.414 | yes                 |

### Wilcoxon signed-rank: cgs vs each algorithm (Holm-corrected, two-sided)

gain = (other - cgs) / other; positive = cgs has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| la_heft  | 712 |    582 |      4 |      126 |           6.707 |             5.791 | [4.824, 6.483] |       0.73 |  1.69e-63 |      409 | 1.69e-63 | yes, better   |         1.14e-48 | yes      |
| heft_ins | 712 |    602 |      6 |      104 |           5.523 |             3.549 | [3.210, 3.934] |       0.83 |  2.26e-81 |      409 | 4.51e-81 | yes, better   |         2.47e-54 | yes      |
