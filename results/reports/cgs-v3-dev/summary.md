# Benchmark report: cgs-v3-dev

Algorithms: cgs, cgs_v3, cgs_v3_fan, cgs_v3_b025, cgs_v3_b050, cgs_v3_b100, cgs_v3_b200, la_heft, heft_ins  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo        |   ok | example_problem   |
|:------------|-----:|:------------------|
| cgs         |  712 |                   |
| cgs_v3      |  712 |                   |
| cgs_v3_fan  |  712 |                   |
| cgs_v3_b025 |  712 |                   |
| cgs_v3_b050 |  712 |                   |
| cgs_v3_b100 |  712 |                   |
| cgs_v3_b200 |  712 |                   |
| la_heft     |  712 |                   |
| heft_ins    |  712 |                   |

## Overall averages
| algo        |   makespan |    slr |   speedup |   efficiency |   runtime_s |
|:------------|-----------:|-------:|----------:|-------------:|------------:|
| cgs         |    4352.14 | 8.8194 |   23.7392 |       1.4339 |      0.037  |
| cgs_v3      |    4352.14 | 8.8194 |   23.7392 |       1.4339 |      0.0405 |
| cgs_v3_fan  |    4942.49 | 9.7644 |   21.4944 |       1.2511 |      0.0793 |
| cgs_v3_b025 |    4735.87 | 9.2858 |   23.0912 |       1.3439 |      0.0794 |
| cgs_v3_b050 |    4636.79 | 9.1176 |   23.4831 |       1.3758 |      0.0809 |
| cgs_v3_b100 |    4491.08 | 8.951  |   23.7386 |       1.4019 |      0.0801 |
| cgs_v3_b200 |    4386.96 | 8.8489 |   23.8553 |       1.4197 |      0.0808 |
| la_heft     |    4802.1  | 9.6408 |   22.0328 |       1.2847 |      0.1562 |
| heft_ins    |    4479.08 | 9.453  |   21.9582 |       1.3623 |      0.03   |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo        |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| cgs         |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v3      |       0.6957 |      2.3677 |        0.5347 |      1.0331 |   0.018  |   51436.4 |      12.1393 |
| cgs_v3_fan  |       0.6687 |      2.8416 |        0.578  |      1.0358 |   0.0152 |   43809.6 |      10.5334 |
| cgs_v3_b025 |       0.6504 |      2.7489 |        0.5515 |      1.0301 |   0.0177 |   42704.2 |      11.1511 |
| cgs_v3_b050 |       0.6531 |      2.7587 |        0.5483 |      1.0308 |   0.0178 |   44235   |      11.3878 |
| cgs_v3_b100 |       0.6623 |      2.66   |        0.5443 |      1.032  |   0.0177 |   49635   |      11.8239 |
| cgs_v3_b200 |       0.6718 |      2.5271 |        0.5417 |      1.0326 |   0.0179 |   52190.5 |      12.0547 |
| la_heft     |       0.6497 |      2.7219 |        0.5551 |      1.0287 |   0.0177 |   49475.1 |      10.7889 |
| heft_ins    |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |

## cgs versus each algorithm
| versus      |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:------------|-------------:|------------:|------------:|---------------:|
| cgs_v3      |         0    |      100    |        0    |          0     |
| cgs_v3_fan  |        84.27 |        1.26 |       14.47 |          8.622 |
| cgs_v3_b025 |        71.63 |        2.11 |       26.26 |          3.759 |
| cgs_v3_b050 |        66.29 |        2.95 |       30.76 |          2.235 |
| cgs_v3_b100 |        62.08 |        3.93 |       33.99 |          0.895 |
| cgs_v3_b200 |        53.37 |        7.16 |       39.47 |          0.035 |
| la_heft     |        81.74 |        0.56 |       17.7  |          6.707 |
| heft_ins    |        84.55 |        0.84 |       14.61 |          5.523 |

## Statistical significance

Friedman test over 9 algorithms on 712 instances: chi2 = 2023.6, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.450 ranks.

| algo        |   average_rank | differs_from_best   |
|:------------|---------------:|:--------------------|
| cgs         |          3.367 | —                   |
| cgs_v3      |          3.367 | no                  |
| cgs_v3_b200 |          3.474 | no                  |
| cgs_v3_b100 |          3.979 | yes                 |
| cgs_v3_b050 |          4.675 | yes                 |
| cgs_v3_b025 |          5.495 | yes                 |
| heft_ins    |          6.121 | yes                 |
| la_heft     |          6.923 | yes                 |
| cgs_v3_fan  |          7.6   | yes                 |

### Wilcoxon signed-rank: cgs vs each algorithm (Holm-corrected, two-sided)

gain = (other - cgs) / other; positive = cgs has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus      |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95    |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:------------|----:|-------:|-------:|---------:|----------------:|------------------:|:---------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| cgs_v3      | 712 |      0 |    712 |        0 |           0     |             0     | [0.000, 0.000] |      0     |  1        |      409 | 1        | no            |         1        | no       |
| cgs_v3_fan  | 712 |    600 |      9 |      103 |           8.622 |             7.839 | [7.415, 8.316] |      0.802 |  8.39e-76 |      409 | 5.87e-75 | yes, better   |         1e-51    | yes      |
| cgs_v3_b025 | 712 |    510 |     15 |      187 |           3.759 |             2.912 | [2.407, 3.447] |      0.547 |  6.59e-36 |      409 | 3.29e-35 | yes, better   |         1.36e-30 | yes      |
| cgs_v3_b050 | 712 |    472 |     21 |      219 |           2.235 |             1.443 | [0.989, 2.133] |      0.416 |  2.63e-21 |      409 | 1.05e-20 | yes, better   |         1.18e-18 | yes      |
| cgs_v3_b100 | 712 |    442 |     28 |      242 |           0.895 |             0.819 | [0.557, 1.210] |      0.302 |  7.48e-12 |      409 | 2.24e-11 | yes, better   |         1.28e-10 | yes      |
| cgs_v3_b200 | 712 |    380 |     51 |      281 |           0.035 |             0.145 | [0.000, 0.336] |      0.14  |  0.00188  |      409 | 0.00377  | yes, better   |         0.0268   | yes      |
| la_heft     | 712 |    582 |      4 |      126 |           6.707 |             5.791 | [4.824, 6.483] |      0.73  |  1.69e-63 |      409 | 1.01e-62 | yes, better   |         6.82e-48 | yes      |
| heft_ins    | 712 |    602 |      6 |      104 |           5.523 |             3.549 | [3.210, 3.934] |      0.83  |  2.26e-81 |      409 | 1.8e-80  | yes, better   |         9.89e-54 | yes      |
