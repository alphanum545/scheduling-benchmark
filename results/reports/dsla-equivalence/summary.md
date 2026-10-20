# Benchmark report: dsla-equivalence

Algorithms: dsla, slas, z_best10, bla_heft  
Instances compared: 2814 of 2814 (only those where every algorithm is valid)

## Validity
| algo     |   ok | example_problem   |
|:---------|-----:|:------------------|
| dsla     | 2814 |                   |
| slas     | 2814 |                   |
| z_best10 | 2814 |                   |
| bla_heft | 2814 |                   |

## Overall averages
| algo     |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:---------|-----------:|-------:|----------:|-------------:|------------:|
| dsla     |     4469.6 | 8.822  |   22.1652 |       1.3845 |      0.0368 |
| slas     |     4664.7 | 8.9551 |   22.107  |       1.3783 |      0.0334 |
| z_best10 |     4469.6 | 8.822  |   22.1652 |       1.3845 |      0.0386 |
| bla_heft |     4664.7 | 8.9551 |   22.107  |       1.3783 |      0.0345 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo     |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:---------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| dsla     |       0.7126 |      2.4208 |        0.5125 |      1.0316 |   0.0177 |   55586.2 |      11.4288 |
| slas     |       0.7098 |      2.6293 |        0.5179 |      1.0278 |   0.0178 |   54160.8 |      11.1202 |
| z_best10 |       0.7126 |      2.4208 |        0.5125 |      1.0316 |   0.0177 |   55586.2 |      11.4288 |
| bla_heft |       0.7098 |      2.6293 |        0.5179 |      1.0278 |   0.0178 |   54160.8 |      11.1202 |

## dsla versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| slas     |        23.17 |       76.83 |           0 |           1.04 |
| z_best10 |         0    |      100    |           0 |           0    |
| bla_heft |        23.17 |       76.83 |           0 |           1.04 |

## Statistical significance

Friedman test over 4 algorithms on 2814 instances: chi2 = 1956.0, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.088 ranks.

| algo     |   average_rank | differs_from_best   |
|:---------|---------------:|:--------------------|
| dsla     |          2.268 | —                   |
| z_best10 |          2.268 | no                  |
| slas     |          2.732 | yes                 |
| bla_heft |          2.732 | yes                 |

### Wilcoxon signed-rank: dsla vs each algorithm (Holm-corrected, two-sided)

gain = (other - dsla) / other; positive = dsla has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus   |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:---------|-----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| slas     | 2814 |    652 |   2162 |        0 |            1.04 |                 0 | [0.000, 0.000] |          1 | 1.95e-108 |      529 | 5.85e-108 | yes, better   |          3.6e-57 | yes      |
| z_best10 | 2814 |      0 |   2814 |        0 |            0    |                 0 | [0.000, 0.000] |          0 | 1         |      529 | 1         | no            |          1       | no       |
| bla_heft | 2814 |    652 |   2162 |        0 |            1.04 |                 0 | [0.000, 0.000] |          1 | 1.95e-108 |      529 | 5.85e-108 | yes, better   |          3.6e-57 | yes      |
