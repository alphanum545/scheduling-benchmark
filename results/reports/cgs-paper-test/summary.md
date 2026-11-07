# Benchmark report: cgs-paper-test

Algorithms: cgs, cgs_single, cgs_credit0, heft_ins, peft, aeft, bppts, left, la_heft  
Instances compared: 2102 of 2102 (only those where every algorithm is valid)

## Validity
| algo        |   ok | example_problem   |
|:------------|-----:|:------------------|
| cgs         | 2102 |                   |
| cgs_single  | 2102 |                   |
| cgs_credit0 | 2102 |                   |
| heft_ins    | 2102 |                   |
| peft        | 2102 |                   |
| aeft        | 2102 |                   |
| bppts       | 2102 |                   |
| left        | 2102 |                   |
| la_heft     | 2102 |                   |

## Overall averages
| algo        |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:------------|-----------:|--------:|----------:|-------------:|------------:|
| cgs         |    4526.84 |  8.7498 |   22.5276 |       1.4091 |      0.0409 |
| cgs_single  |    4549.28 |  8.8147 |   22.3327 |       1.3999 |      0.0393 |
| cgs_credit0 |    4542.94 |  8.7658 |   22.5206 |       1.4087 |      0.0397 |
| heft_ins    |    4659.04 |  9.364  |   20.9127 |       1.3428 |      0.0334 |
| peft        |    5129.37 |  9.9635 |   19.1376 |       1.1671 |      0.1037 |
| aeft        |    4930.7  |  9.3865 |   20.0047 |       1.2663 |      0.104  |
| bppts       |    5718.62 | 11.4907 |   15.8972 |       0.9563 |      0.032  |
| left        |    4673.3  |  9.4997 |   19.3367 |       1.2525 |      0.032  |
| la_heft     |    5000.17 |  9.5291 |   21.065  |       1.2583 |      0.1646 |

## Secondary metrics (means over the compared instances)
comm_ratio = communication paid / total; imbalance = max / mean processor busy time; utilization = busy / (p x makespan); rob_ratio = mean makespan under +-20% cost noise / makespan; rob_cv = coefficient of variation of the perturbed makespans; cont_mk = makespan of the schedule replayed under network contention (one-port model), cont_ratio = cont_mk / makespan.

| algo        |   comm_ratio |   imbalance |   utilization |   rob_ratio |   rob_cv |   cont_mk |   cont_ratio |
|:------------|-------------:|------------:|--------------:|------------:|---------:|----------:|-------------:|
| cgs         |       0.686  |      2.4159 |        0.5384 |      1.0325 |   0.0177 |   55008.7 |      11.2329 |
| cgs_single  |       0.6923 |      2.2898 |        0.535  |      1.0324 |   0.0178 |   54203.5 |      11.083  |
| cgs_credit0 |       0.6907 |      2.3262 |        0.5378 |      1.0326 |   0.0177 |   54280.4 |      11.2093 |
| heft_ins    |       0.7298 |      2.4417 |        0.5335 |      1.0323 |   0.0178 |   55870.8 |      10.7085 |
| peft        |       0.6811 |      2.2206 |        0.5519 |      1.0261 |   0.0175 |   49972.5 |       9.6354 |
| aeft        |       0.7143 |      2.1565 |        0.5087 |      1.0264 |   0.018  |   47223.8 |       9.877  |
| bppts       |       0.6344 |      2.1414 |        0.505  |      1.0233 |   0.0172 |   46416.8 |       7.7657 |
| left        |       0.7082 |      2.423  |        0.456  |      1.0317 |   0.0188 |   56485.2 |      10.1021 |
| la_heft     |       0.6377 |      2.7142 |        0.5603 |      1.0278 |   0.0172 |   55153   |      10.0353 |

## cgs versus each algorithm
| versus      |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:------------|-------------:|------------:|------------:|---------------:|
| cgs_single  |        39.3  |       47.62 |       13.08 |          0.836 |
| cgs_credit0 |        16.08 |       73.5  |       10.42 |          0.221 |
| heft_ins    |        84.68 |        1.24 |       14.08 |          5.419 |
| peft        |        92.34 |        0.81 |        6.85 |         11.567 |
| aeft        |        85.73 |        1.24 |       13.04 |          7.8   |
| bppts       |        96.91 |        0.24 |        2.85 |         22.351 |
| left        |        86.44 |        0.24 |       13.32 |          9.567 |
| la_heft     |        79.54 |        0.62 |       19.84 |          6.604 |

## Statistical significance

Friedman test over 9 algorithms on 2102 instances: chi2 = 8482.2, p = 0.00e+00; Nemenyi critical difference (alpha 0.05) = 0.262 ranks.

| algo        |   average_rank | differs_from_best   |
|:------------|---------------:|:--------------------|
| cgs         |          2.562 | —                   |
| cgs_credit0 |          2.694 | no                  |
| cgs_single  |          3.146 | yes                 |
| heft_ins    |          5.044 | yes                 |
| la_heft     |          5.127 | yes                 |
| aeft        |          5.432 | yes                 |
| left        |          6.143 | yes                 |
| peft        |          6.529 | yes                 |
| bppts       |          8.323 | yes                 |

### Wilcoxon signed-rank: cgs vs each algorithm (Holm-corrected, two-sided)

gain = (other - cgs) / other; positive = cgs has the shorter makespan. p_per_dag_holm repeats the test with one observation per DAG (gains averaged over processor counts); 'robust' = significant under both.

| versus      |    n |   wins |   ties |   losses |   mean_gain_pct |   median_gain_pct | median_ci95      |   effect_r |   p_value |   n_dags |    p_holm | significant   |   p_per_dag_holm | robust   |
|:------------|-----:|-------:|-------:|---------:|----------------:|------------------:|:-----------------|-----------:|----------:|---------:|----------:|:--------------|-----------------:|:---------|
| cgs_single  | 2102 |    826 |   1001 |      275 |           0.836 |             0     | [0.000, 0.000]   |      0.584 | 2.86e-63  |      529 | 5.72e-63  | yes, better   |         5.56e-37 | yes      |
| cgs_credit0 | 2102 |    338 |   1545 |      219 |           0.221 |             0     | [0.000, 0.000]   |      0.259 | 1.13e-07  |      529 | 1.13e-07  | yes, better   |         8.43e-06 | yes      |
| heft_ins    | 2102 |   1780 |     26 |      296 |           5.419 |             3.627 | [3.413, 3.836]   |      0.835 | 2.16e-238 |      529 | 8.62e-238 | yes, better   |         4.43e-83 | yes      |
| peft        | 2102 |   1941 |     17 |      144 |          11.567 |             9.218 | [8.857, 9.606]   |      0.949 | 2.68e-308 |      529 | 1.87e-307 | yes, better   |         2.99e-86 | yes      |
| aeft        | 2102 |   1802 |     26 |      274 |           7.8   |             5.918 | [5.447, 6.373]   |      0.896 | 4.6e-274  |      529 | 2.3e-273  | yes, better   |         8.81e-87 | yes      |
| bppts       | 2102 |   2037 |      5 |       60 |          22.351 |            19.059 | [18.154, 19.969] |      0.991 | 0         |      529 | 0         | yes, better   |         2.69e-87 | yes      |
| left        | 2102 |   1817 |      5 |      280 |           9.567 |             8.465 | [8.045, 9.016]   |      0.909 | 1.91e-284 |      529 | 1.14e-283 | yes, better   |         7.49e-87 | yes      |
| la_heft     | 2102 |   1672 |     13 |      417 |           6.604 |             5.551 | [5.094, 5.998]   |      0.692 | 2.03e-165 |      529 | 6.1e-165  | yes, better   |         4.76e-71 | yes      |
