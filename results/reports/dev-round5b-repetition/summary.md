# Benchmark report: dev-round5b-repetition

Algorithms: g_ins_g0, g_ins_g25, g_ins_g50, g_ins_g100, g_ins_g0_net, g_ins_g0_b50, g_ins_g0_net_b50, g_g0, g_g0_net, l4_oppts, l4_hr_b50_ins, l4_heft_ins, oppts, dsla, left, heft  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo             |   ok | example_problem   |
|:-----------------|-----:|:------------------|
| g_ins_g0         |  712 |                   |
| g_ins_g25        |  712 |                   |
| g_ins_g50        |  712 |                   |
| g_ins_g100       |  712 |                   |
| g_ins_g0_net     |  712 |                   |
| g_ins_g0_b50     |  712 |                   |
| g_ins_g0_net_b50 |  712 |                   |
| g_g0             |  712 |                   |
| g_g0_net         |  712 |                   |
| l4_oppts         |  712 |                   |
| l4_hr_b50_ins    |  712 |                   |
| l4_heft_ins      |  712 |                   |
| oppts            |  712 |                   |
| dsla             |  712 |                   |
| left             |  712 |                   |
| heft             |  712 |                   |

## Overall averages
| algo             |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:-----------------|-----------:|--------:|----------:|-------------:|------------:|
| g_ins_g0         |    4357.33 |  8.9385 |   22.9195 |       1.3997 |      0.032  |
| g_ins_g25        |    4372.09 |  8.9429 |   22.9766 |       1.3948 |      0.0316 |
| g_ins_g50        |    4377.29 |  8.9446 |   22.9651 |       1.3921 |      0.0316 |
| g_ins_g100       |    4387.46 |  8.9742 |   22.9354 |       1.3858 |      0.0331 |
| g_ins_g0_net     |    4625.99 |  9.6637 |   21.3212 |       1.2548 |      0.0321 |
| g_ins_g0_b50     |    4367.15 |  8.8952 |   23.4274 |       1.4208 |      0.0314 |
| g_ins_g0_net_b50 |    4491.71 |  9.1915 |   22.8534 |       1.3533 |      0.0321 |
| g_g0             |    4677.11 |  9.423  |   21.0306 |       1.2934 |      0.0331 |
| g_g0_net         |    4947.04 | 10.1304 |   19.8151 |       1.1773 |      0.0314 |
| l4_oppts         |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0315 |
| l4_hr_b50_ins    |    4377.89 |  8.9055 |   23.4975 |       1.4169 |      0.0316 |
| l4_heft_ins      |    4479.08 |  9.453  |   21.9582 |       1.3623 |      0.0338 |
| oppts            |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0973 |
| dsla             |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.0366 |
| left             |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.033  |
| heft             |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0868 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo             |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:-----------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| g_ins_g0         |       0.6875 |      2.4823 |        0.5298 |      1.0313 |   0.0182 |   53273.4 |      11.7864 |
| g_ins_g25        |       0.6817 |      2.4254 |        0.5314 |      1.0311 |   0.0181 |   53052.8 |      11.7648 |
| g_ins_g50        |       0.676  |      2.4282 |        0.5324 |      1.0308 |   0.0184 |   53201.8 |      11.7308 |
| g_ins_g100       |       0.6716 |      2.4149 |        0.5319 |      1.0307 |   0.0184 |   53315.9 |      11.6817 |
| g_ins_g0_net     |       0.658  |      2.4893 |        0.5664 |      1.0309 |   0.0177 |   53267   |      11.1072 |
| g_ins_g0_b50     |       0.6961 |      2.3985 |        0.5313 |      1.0323 |   0.0182 |   52513.8 |      11.9485 |
| g_ins_g0_net_b50 |       0.6806 |      2.4094 |        0.553  |      1.0327 |   0.0178 |   52846.3 |      11.7033 |
| g_g0             |       0.7194 |      2.1578 |        0.5056 |      1.0259 |   0.0184 |   42880.1 |      10.7123 |
| g_g0_net         |       0.685  |      2.1673 |        0.5506 |      1.0263 |   0.0175 |   42733.6 |      10.0884 |
| l4_oppts         |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| l4_hr_b50_ins    |       0.6875 |      2.389  |        0.5306 |      1.0321 |   0.0182 |   52797.5 |      11.925  |
| l4_heft_ins      |       0.7372 |      2.3664 |        0.529  |      1.0326 |   0.0184 |   51608.3 |      11.5766 |
| oppts            |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| dsla             |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |
| left             |       0.7182 |      2.3751 |        0.4519 |      1.0318 |   0.0189 |   51806.8 |      10.8583 |
| heft             |       0.7694 |      2.2526 |        0.5204 |      1.0288 |   0.0183 |   44319.5 |      11.1233 |

## g_ins_g0 versus each algorithm
| versus           |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:-----------------|-------------:|------------:|------------:|---------------:|
| g_ins_g25        |        50.14 |       14.19 |       35.67 |          0.014 |
| g_ins_g50        |        52.67 |       10.11 |       37.22 |         -0.099 |
| g_ins_g100       |        56.18 |        5.34 |       38.48 |          0.153 |
| g_ins_g0_net     |        73.6  |        1.83 |       24.58 |          5.635 |
| g_ins_g0_b50     |        42.42 |        5.06 |       52.53 |         -0.929 |
| g_ins_g0_net_b50 |        57.58 |        4.63 |       37.78 |          1.543 |
| g_g0             |        78.51 |        8.43 |       13.06 |          5.687 |
| g_g0_net         |        85.11 |        1.12 |       13.76 |         10.393 |
| l4_oppts         |        79.35 |        3.23 |       17.42 |          4.925 |
| l4_hr_b50_ins    |        46.07 |        2.81 |       51.12 |         -0.988 |
| l4_heft_ins      |        72.61 |        0.7  |       26.69 |          3.739 |
| oppts            |        79.35 |        3.23 |       17.42 |          4.925 |
| dsla             |        48.31 |        2.81 |       48.88 |         -0.488 |
| left             |        84.41 |        0.28 |       15.31 |          8.057 |
| heft             |        83.85 |        0.42 |       15.73 |          5.901 |

## Statistical significance

Friedman test over 16 algorithms on 712 instances: chi2 = 3223.1, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.864 ranks.

| algo             |   average_rank | differs_from_best   |
|:-----------------|---------------:|:--------------------|
| g_ins_g0_b50     |          5.353 | —                   |
| l4_hr_b50_ins    |          5.48  | no                  |
| g_ins_g0         |          5.777 | no                  |
| dsla             |          5.822 | no                  |
| g_ins_g25        |          6.12  | no                  |
| g_ins_g50        |          6.188 | no                  |
| g_ins_g100       |          6.53  | yes                 |
| g_ins_g0_net_b50 |          8.004 | yes                 |
| l4_heft_ins      |          9.499 | yes                 |
| l4_oppts         |         10.229 | yes                 |
| oppts            |         10.229 | yes                 |
| g_ins_g0_net     |         10.31  | yes                 |
| g_g0             |         10.43  | yes                 |
| heft             |         11.21  | yes                 |
| left             |         11.813 | yes                 |
| g_g0_net         |         13.006 | yes                 |

### Wilcoxon signed-rank: g_ins_g0 vs each algorithm (Holm-corrected, two-sided)

gain = (other - g_ins_g0) / other; positive = g_ins_g0 has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus           |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95     |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:-----------------|----:|-------:|-------:|---------:|----------------:|------------------:|:----------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| g_ins_g25        | 712 |    357 |    101 |      254 |           0.014 |             0.024 | [0.000, 0.166]  |      0.158 |  0.000701 |      409 | 0.0014   | yes, better   |         0.0328   | yes      |
| g_ins_g50        | 712 |    375 |     72 |      265 |          -0.099 |             0.117 | [0.000, 0.323]  |      0.171 |  0.00018  |      409 | 0.000539 | yes, better   |         0.0278   | yes      |
| g_ins_g100       | 712 |    400 |     38 |      274 |           0.153 |             0.424 | [0.160, 0.645]  |      0.206 |  3.69e-06 |      409 | 1.85e-05 | yes, better   |         0.00252  | yes      |
| g_ins_g0_net     | 712 |    524 |     13 |      175 |           5.635 |             4.386 | [3.671, 5.110]  |      0.675 |  7.49e-54 |      409 | 6.74e-53 | yes, better   |         1.94e-35 | yes      |
| g_ins_g0_b50     | 712 |    302 |     36 |      374 |          -0.929 |            -0.16  | [-0.356, 0.000] |     -0.224 |  4.6e-07  |      409 | 2.76e-06 | yes, worse    |         4.17e-08 | yes      |
| g_ins_g0_net_b50 | 712 |    410 |     33 |      269 |           1.543 |             0.636 | [0.293, 1.336]  |      0.35  |  2.59e-15 |      409 | 1.82e-14 | yes, better   |         4.19e-12 | yes      |
| g_g0             | 712 |    559 |     60 |       93 |           5.687 |             3.672 | [3.045, 4.332]  |      0.873 |  4.58e-83 |      409 | 5.95e-82 | yes, better   |         1.39e-53 | yes      |
| g_g0_net         | 712 |    606 |      8 |       98 |          10.393 |             9.365 | [8.556, 10.210] |      0.87  |  5.29e-89 |      409 | 7.93e-88 | yes, better   |         1.24e-55 | yes      |
| l4_oppts         | 712 |    565 |     23 |      124 |           4.925 |             3.733 | [3.133, 4.341]  |      0.767 |  3.93e-68 |      409 | 4.33e-67 | yes, better   |         1.01e-46 | yes      |
| l4_hr_b50_ins    | 712 |    328 |     20 |      364 |          -0.988 |            -0.116 | [-0.324, 0.000] |     -0.192 |  1.26e-05 |      409 | 5.05e-05 | yes, worse    |         0.000151 | yes      |
| l4_heft_ins      | 712 |    517 |      5 |      190 |           3.739 |             2.798 | [2.205, 3.336]  |      0.563 |  1.95e-38 |      409 | 1.56e-37 | yes, better   |         6.43e-29 | yes      |
| oppts            | 712 |    565 |     23 |      124 |           4.925 |             3.733 | [3.133, 4.341]  |      0.767 |  3.93e-68 |      409 | 4.33e-67 | yes, better   |         1.01e-46 | yes      |
| dsla             | 712 |    344 |     20 |      348 |          -0.488 |             0     | [-0.204, 0.129] |     -0.051 |  0.245    |      409 | 0.245    | no            |         0.273    | no       |
| left             | 712 |    601 |      2 |      109 |           8.057 |             6.461 | [5.571, 7.583]  |      0.86  |  1.08e-87 |      409 | 1.51e-86 | yes, better   |         4.54e-58 | yes      |
| heft             | 712 |    597 |      3 |      112 |           5.901 |             4.694 | [4.225, 5.147]  |      0.805 |  5.61e-77 |      409 | 6.73e-76 | yes, better   |         8.39e-55 | yes      |
