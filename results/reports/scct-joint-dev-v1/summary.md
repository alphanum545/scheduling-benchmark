# Benchmark report: scct-joint-dev-v1

Algorithms: scct_joint_diamond, scct, scct_joint_base, scct_joint, scct_diamond, oppts, heft, left, dsla, aeft, peft  
Instances compared: 712 of 712 (only those where every algorithm is valid)

## Validity
| algo               |   ok | example_problem   |
|:-------------------|-----:|:------------------|
| scct_joint_diamond |  712 |                   |
| scct               |  712 |                   |
| scct_joint_base    |  712 |                   |
| scct_joint         |  712 |                   |
| scct_diamond       |  712 |                   |
| oppts              |  712 |                   |
| heft               |  712 |                   |
| left               |  712 |                   |
| dsla               |  712 |                   |
| aeft               |  712 |                   |
| peft               |  712 |                   |

## Overall averages
| algo               |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:-------------------|-----------:|--------:|----------:|-------------:|------------:|
| scct_joint_diamond |    4605.78 |  9.2589 |   21.703  |       1.33   |      0.7558 |
| scct               |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0333 |
| scct_joint_base    |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0399 |
| scct_joint         |    4605.78 |  9.2589 |   21.703  |       1.33   |      0.7093 |
| scct_diamond       |    4610.77 |  9.2719 |   21.6664 |       1.3272 |      0.0403 |
| oppts              |    4671.13 |  9.387  |   21.2845 |       1.3025 |      0.0973 |
| heft               |    4668.99 |  9.6197 |   21.5457 |       1.3318 |      0.0868 |
| left               |    4493.97 |  9.5985 |   20.3849 |       1.2754 |      0.033  |
| dsla               |    4340.09 |  8.8686 |   23.0137 |       1.4021 |      0.0366 |
| aeft               |    4688.64 |  9.4375 |   21.0459 |       1.2909 |      0.1008 |
| peft               |    4905.46 | 10.0312 |   20.127  |       1.1942 |      0.1022 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo               |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:-------------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| scct_joint_diamond |       0.7215 |      2.0258 |        0.5118 |      1.0271 |   0.0182 |   41384.8 |      11.0277 |
| scct               |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| scct_joint_base    |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| scct_joint         |       0.7215 |      2.0258 |        0.5118 |      1.0271 |   0.0182 |   41384.8 |      11.0277 |
| scct_diamond       |       0.7216 |      2.023  |        0.5115 |      1.0268 |   0.0183 |   41296.6 |      11.0108 |
| oppts              |       0.7049 |      2.1294 |        0.5139 |      1.0258 |   0.0183 |   44197.4 |      10.8181 |
| heft               |       0.7694 |      2.2526 |        0.5204 |      1.0288 |   0.0183 |   44319.5 |      11.1233 |
| left               |       0.7182 |      2.3751 |        0.4519 |      1.0318 |   0.0189 |   51806.8 |      10.8583 |
| dsla               |       0.7191 |      2.3336 |        0.5085 |      1.0317 |   0.018  |   52468.7 |      12.0789 |
| aeft               |       0.7216 |      2.1409 |        0.5063 |      1.026  |   0.0184 |   42171.4 |      10.7195 |
| peft               |       0.6899 |      2.1941 |        0.5505 |      1.026  |   0.0176 |   45086.3 |      10.4542 |

## scct_joint_diamond versus each algorithm
| versus          |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:----------------|-------------:|------------:|------------:|---------------:|
| scct            |        39.19 |       29.78 |       31.04 |          0.094 |
| scct_joint_base |        39.19 |       29.78 |       31.04 |          0.094 |
| scct_joint      |         0    |      100    |        0    |          0     |
| scct_diamond    |        39.19 |       29.78 |       31.04 |          0.094 |
| oppts           |        65.87 |        3.65 |       30.48 |          1.076 |
| heft            |        61.8  |        0.98 |       37.22 |          1.812 |
| left            |        68.82 |        0.42 |       30.76 |          4.05  |
| dsla            |        24.02 |        1.97 |       74.02 |         -4.898 |
| aeft            |        70.79 |        5.48 |       23.74 |          2.084 |
| peft            |        80.76 |        0.98 |       18.26 |          6.067 |

## Statistical significance

Friedman test over 11 algorithms on 712 instances: chi2 = 1689.9, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.566 ranks.

| algo               |   average_rank | differs_from_best   |
|:-------------------|---------------:|:--------------------|
| dsla               |          2.909 | —                   |
| scct_joint_diamond |          5.09  | yes                 |
| scct_joint         |          5.09  | yes                 |
| scct               |          5.343 | yes                 |
| scct_diamond       |          5.343 | yes                 |
| scct_joint_base    |          5.343 | yes                 |
| oppts              |          6.635 | yes                 |
| heft               |          6.711 | yes                 |
| aeft               |          7.22  | yes                 |
| left               |          7.671 | yes                 |
| peft               |          8.647 | yes                 |

### Wilcoxon signed-rank: scct_joint_diamond vs each algorithm (Holm-corrected, two-sided)

gain = (other - scct_joint_diamond) / other; positive = scct_joint_diamond has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus          |   n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |   p_holm | significant   |   p_per_dag_holm | robust   |
|:----------------|----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|---------:|:--------------|-----------------:|:---------|
| scct            | 712 |    279 |    212 |      221 |           0.094 |             0     | [0.000, 0.000]   |      0.126 |  0.0147   |      409 | 0.0586   | no            |         0.776    | no       |
| scct_joint_base | 712 |    279 |    212 |      221 |           0.094 |             0     | [0.000, 0.000]   |      0.126 |  0.0147   |      409 | 0.0586   | no            |         0.776    | no       |
| scct_joint      | 712 |      0 |    712 |        0 |           0     |             0     | [0.000, 0.000]   |      0     |  1        |      409 | 1        | no            |         1        | no       |
| scct_diamond    | 712 |    279 |    212 |      221 |           0.094 |             0     | [0.000, 0.000]   |      0.126 |  0.0147   |      409 | 0.0586   | no            |         0.776    | no       |
| oppts           | 712 |    469 |     26 |      217 |           1.076 |             1.191 | [0.951, 1.582]   |      0.432 |  1.15e-22 |      409 | 6.88e-22 | yes, better   |         6.29e-15 | yes      |
| heft            | 712 |    440 |      7 |      265 |           1.812 |             1.634 | [0.956, 2.283]   |      0.309 |  1.21e-12 |      409 | 6.03e-12 | yes, better   |         3.73e-06 | yes      |
| left            | 712 |    490 |      3 |      219 |           4.05  |             3.603 | [2.705, 4.317]   |      0.466 |  6.16e-27 |      409 | 4.31e-26 | yes, better   |         3.96e-16 | yes      |
| dsla            | 712 |    171 |     14 |      527 |          -4.898 |            -2.41  | [-2.911, -1.914] |     -0.698 |  2.01e-57 |      409 | 1.81e-56 | yes, worse    |         2.5e-43  | yes      |
| aeft            | 712 |    504 |     39 |      169 |           2.084 |             1.621 | [1.303, 1.917]   |      0.658 |  1.54e-49 |      409 | 1.23e-48 | yes, better   |         2.23e-34 | yes      |
| peft            | 712 |    575 |      7 |      130 |           6.067 |             5.573 | [4.502, 6.099]   |      0.746 |  4.98e-66 |      409 | 4.98e-65 | yes, better   |         6.91e-42 | yes      |
