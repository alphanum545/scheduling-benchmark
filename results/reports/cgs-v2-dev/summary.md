# Benchmark report: cgs-v2-dev

Algorithms: cgs, cgs_v2_w000, cgs_v2_w010, cgs_v2_w025, cgs_v2_w050, cgs_v2_w100, la_heft, heft_ins  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo        |   ok | example_problem   |
|:------------|-----:|:------------------|
| cgs         |  712 |                   |
| cgs_v2_w000 |  712 |                   |
| cgs_v2_w010 |  712 |                   |
| cgs_v2_w025 |  712 |                   |
| cgs_v2_w050 |  712 |                   |
| cgs_v2_w100 |  712 |                   |
| la_heft     |  712 |                   |
| heft_ins    |  712 |                   |

## Overall averages
| algo        |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:------------|-----------:|--------:|----------:|-------------:|------------:|
| cgs         |    4352.14 |  8.8194 |   23.7392 |       1.4339 |      0.037  |
| cgs_v2_w000 |    4352.14 |  8.8194 |   23.7392 |       1.4339 |      0.0382 |
| cgs_v2_w010 |    4350.51 |  8.8094 |   23.7334 |       1.4293 |      0.0801 |
| cgs_v2_w025 |    4353.49 |  8.8352 |   23.6682 |       1.4201 |      0.0761 |
| cgs_v2_w050 |    4458.41 |  8.9385 |   23.4552 |       1.3972 |      0.0734 |
| cgs_v2_w100 |    4867.14 | 10.2004 |   19.0119 |       1.1869 |      0.0758 |
| la_heft     |    4802.1  |  9.6408 |   22.0328 |       1.2847 |      0.1562 |
| heft_ins    |    4479.08 |  9.453  |   21.9582 |       1.3623 |      0.03   |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo        |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| cgs         |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v2_w000 |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v2_w010 |       0.6881 |      2.3898 |        0.5359 |      1.0328 |   0.0181 |   51696.9 |      12.1389 |
| cgs_v2_w025 |       0.6757 |      2.5024 |        0.5385 |      1.0321 |   0.018  |   52357.7 |      12.0547 |
| cgs_v2_w050 |       0.6593 |      2.699  |        0.5416 |      1.0316 |   0.0178 |   50535   |      11.7501 |
| cgs_v2_w100 |       0.6567 |      3.1333 |        0.5251 |      1.0259 |   0.016  |   44496.5 |       8.5935 |
| la_heft     |       0.6497 |      2.7219 |        0.5551 |      1.0287 |   0.0177 |   49475.1 |      10.7889 |
| heft_ins    |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |

## cgs versus each algorithm
| versus      |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:------------|-------------:|------------:|------------:|---------------:|
| cgs_v2_w000 |         0    |      100    |        0    |          0     |
| cgs_v2_w010 |        40.73 |       23.46 |       35.81 |         -0.128 |
| cgs_v2_w025 |        50.42 |        9.69 |       39.89 |          0.077 |
| cgs_v2_w050 |        58.29 |        4.21 |       37.5  |          0.964 |
| cgs_v2_w100 |        84.97 |        0.84 |       14.19 |         11.019 |
| la_heft     |        81.74 |        0.56 |       17.7  |          6.707 |
| heft_ins    |        84.55 |        0.84 |       14.61 |          5.523 |

## Statistical significance

Friedman test over 8 algorithms on 712 instances: chi2 = 1799.8, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.393 ranks.

| algo        |   average_rank | differs_from_best   |
|:------------|---------------:|:--------------------|
| cgs_v2_w010 |          3.282 | —                   |
| cgs         |          3.295 | no                  |
| cgs_v2_w000 |          3.295 | no                  |
| cgs_v2_w025 |          3.48  | no                  |
| cgs_v2_w050 |          4.036 | yes                 |
| heft_ins    |          5.852 | yes                 |
| la_heft     |          6.211 | yes                 |
| cgs_v2_w100 |          6.551 | yes                 |

### Wilcoxon signed-rank: cgs vs each algorithm (Holm-corrected, two-sided)

gain = (other - cgs) / other; positive = cgs has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus      |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:------------|----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| cgs_v2_w000 | 712 |      0 |    712 |        0 |           0     |             0     | [0.000, 0.000] |      0     |  1        |      409 | 1        | no            |         1        | no       |
| cgs_v2_w010 | 712 |    290 |    167 |      255 |          -0.128 |             0     | [0.000, 0.000] |      0.038 |  0.44     |      409 | 0.88     | no            |         1        | no       |
| cgs_v2_w025 | 712 |    359 |     69 |      284 |           0.077 |             0.035 | [0.000, 0.173] |      0.146 |  0.0013   |      409 | 0.00389  | yes, better   |         0.043    | yes      |
| cgs_v2_w050 | 712 |    415 |     30 |      267 |           0.964 |             0.506 | [0.254, 0.740] |      0.294 |  2.95e-11 |      409 | 1.18e-10 | yes, better   |         2.51e-09 | yes      |
| cgs_v2_w100 | 712 |    605 |      6 |      101 |          11.019 |             6.454 | [5.587, 7.297] |      0.851 |  1.97e-85 |      409 | 1.38e-84 | yes, better   |         1.13e-53 | yes      |
| la_heft     | 712 |    582 |      4 |      126 |           6.707 |             5.791 | [4.824, 6.483] |      0.73  |  1.69e-63 |      409 | 8.44e-63 | yes, better   |         5.69e-48 | yes      |
| heft_ins    | 712 |    602 |      6 |      104 |           5.523 |             3.549 | [3.210, 3.934] |      0.83  |  2.26e-81 |      409 | 1.35e-80 | yes, better   |         8.65e-54 | yes      |
