# Benchmark report: cgs-v4-dev

Algorithms: cgs, cgs_v4_off, cgs_v4_g2, cgs_v4_g3, cgs_v4_g5, cgs_v4_g2w05, cgs_v4_g2w01, la_heft, heft_ins  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo         |   ok | example_problem   |
|:-------------|-----:|:------------------|
| cgs          |  712 |                   |
| cgs_v4_off   |  712 |                   |
| cgs_v4_g2    |  712 |                   |
| cgs_v4_g3    |  712 |                   |
| cgs_v4_g5    |  712 |                   |
| cgs_v4_g2w05 |  712 |                   |
| cgs_v4_g2w01 |  712 |                   |
| la_heft      |  712 |                   |
| heft_ins     |  712 |                   |

## Overall averages
| algo         |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:-------------|-----------:|-------:|----------:|-------------:|------------:|
| cgs          |    4352.14 | 8.8194 |   23.7392 |       1.4339 |      0.037  |
| cgs_v4_off   |    4352.14 | 8.8194 |   23.7392 |       1.4339 |      0.0371 |
| cgs_v4_g2    |    4336.43 | 8.809  |   23.7405 |       1.434  |      0.0443 |
| cgs_v4_g3    |    4336.43 | 8.809  |   23.7405 |       1.434  |      0.0439 |
| cgs_v4_g5    |    4336.43 | 8.809  |   23.7405 |       1.434  |      0.0441 |
| cgs_v4_g2w05 |    4412.39 | 8.8362 |   23.7367 |       1.4336 |      0.0443 |
| cgs_v4_g2w01 |    4345.81 | 8.8158 |   23.7397 |       1.434  |      0.0446 |
| la_heft      |    4802.1  | 9.6408 |   22.0328 |       1.2847 |      0.1562 |
| heft_ins     |    4479.08 | 9.453  |   21.9582 |       1.3623 |      0.03   |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo         |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:-------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| cgs          |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v4_off   |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v4_g2    |       0.6886 |      2.5154 |        0.5347 |      1.0326 |   0.018  |   52440.4 |      12.1372 |
| cgs_v4_g3    |       0.6886 |      2.5154 |        0.5347 |      1.0326 |   0.018  |   52440.4 |      12.1372 |
| cgs_v4_g5    |       0.6886 |      2.5154 |        0.5347 |      1.0326 |   0.018  |   52440.4 |      12.1372 |
| cgs_v4_g2w05 |       0.6812 |      2.6736 |        0.5344 |      1.0317 |   0.018  |   50670.8 |      11.9451 |
| cgs_v4_g2w01 |       0.6936 |      2.3954 |        0.5347 |      1.033  |   0.0181 |   51720.9 |      12.1477 |
| la_heft      |       0.6497 |      2.7219 |        0.5551 |      1.0287 |   0.0177 |   49475.1 |      10.7889 |
| heft_ins     |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |

## cgs versus each algorithm
| versus       |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:-------------|-------------:|------------:|------------:|---------------:|
| cgs_v4_off   |         0    |      100    |        0    |          0     |
| cgs_v4_g2    |         2.67 |       92.56 |        4.78 |         -0.122 |
| cgs_v4_g3    |         2.67 |       92.56 |        4.78 |         -0.122 |
| cgs_v4_g5    |         2.67 |       92.56 |        4.78 |         -0.122 |
| cgs_v4_g2w05 |         5.06 |       92.56 |        2.39 |          0.141 |
| cgs_v4_g2w01 |         2.81 |       92.98 |        4.21 |         -0.045 |
| la_heft      |        81.74 |        0.56 |       17.7  |          6.707 |
| heft_ins     |        84.55 |        0.84 |       14.61 |          5.523 |

## Statistical significance

Friedman test over 9 algorithms on 712 instances: chi2 = 2432.7, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.450 ranks.

| algo         |   average_rank | differs_from_best   |
|:-------------|---------------:|:--------------------|
| cgs_v4_g3    |          4.265 | —                   |
| cgs_v4_g2    |          4.265 | no                  |
| cgs_v4_g5    |          4.265 | no                  |
| cgs_v4_g2w01 |          4.312 | no                  |
| cgs          |          4.355 | no                  |
| cgs_v4_off   |          4.355 | no                  |
| cgs_v4_g2w05 |          4.458 | no                  |
| la_heft      |          7.357 | yes                 |
| heft_ins     |          7.367 | yes                 |

### Wilcoxon signed-rank: cgs vs each algorithm (Holm-corrected, two-sided)

gain = (other - cgs) / other; positive = cgs has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus       |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:-------------|----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| cgs_v4_off   | 712 |      0 |    712 |        0 |           0     |             0     | [0.000, 0.000] |      0     |  1        |      409 | 1        | no            |         1        | no       |
| cgs_v4_g2    | 712 |     19 |    659 |       34 |          -0.122 |             0     | [0.000, 0.000] |     -0.452 |  0.00419  |      409 | 0.0251   | yes, worse    |         0.207    | no       |
| cgs_v4_g3    | 712 |     19 |    659 |       34 |          -0.122 |             0     | [0.000, 0.000] |     -0.452 |  0.00419  |      409 | 0.0251   | yes, worse    |         0.207    | no       |
| cgs_v4_g5    | 712 |     19 |    659 |       34 |          -0.122 |             0     | [0.000, 0.000] |     -0.452 |  0.00419  |      409 | 0.0251   | yes, worse    |         0.207    | no       |
| cgs_v4_g2w05 | 712 |     36 |    659 |       17 |           0.141 |             0     | [0.000, 0.000] |      0.423 |  0.00741  |      409 | 0.0251   | yes, better   |         0.177    | no       |
| cgs_v4_g2w01 | 712 |     20 |    662 |       30 |          -0.045 |             0     | [0.000, 0.000] |     -0.36  |  0.0262   |      409 | 0.0523   | no            |         0.207    | no       |
| la_heft      | 712 |    582 |      4 |      126 |           6.707 |             5.791 | [4.824, 6.483] |      0.73  |  1.69e-63 |      409 | 1.18e-62 | yes, better   |         7.96e-48 | yes      |
| heft_ins     | 712 |    602 |      6 |      104 |           5.523 |             3.549 | [3.210, 3.934] |      0.83  |  2.26e-81 |      409 | 1.8e-80  | yes, better   |         9.89e-54 | yes      |
