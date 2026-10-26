# BCR development audit

Measurement run: 40312108945. Development split only.
Valid report rows: 9256; fully paired instances: 712; named-DAG groups: 409.
Seed makespan mismatches: {"scct": 0, "dsla": 0}.
Candidate source SHA-256: e1fb275b3131143d9c37eaac0bb61f081f7e36731263ac7363063134dc8f8281.

## All configurations

| algorithm       |   mean_gain_vs_dsla |   mean_gain_vs_scct |   mean_runtime_s |   median_runtime_s |   p95_runtime_s |   wins_vs_dsla |   ties_vs_dsla |   losses_vs_dsla |
|:----------------|--------------------:|--------------------:|-----------------:|-------------------:|----------------:|---------------:|---------------:|-----------------:|
| scct            |             -5.0154 |              0.0000 |           0.0366 |             0.0317 |          0.1153 |            165 |             14 |              533 |
| dsla            |              0.0000 |              4.2875 |           0.0360 |             0.0318 |          0.1149 |              0 |            712 |                0 |
| bcr_scct_seed   |             -5.0154 |              0.0000 |           0.0371 |             0.0318 |          0.1149 |            165 |             14 |              533 |
| bcr_dsla_seed   |              0.0000 |              4.2875 |           0.0361 |             0.0317 |          0.1150 |              0 |            712 |                0 |
| bcr_scct        |              0.0223 |              4.6726 |           0.0829 |             0.0641 |          0.2162 |            383 |              7 |              322 |
| bcr_dsla_single |              1.5019 |              5.7216 |           0.0444 |             0.0319 |          0.1157 |            569 |            143 |                0 |
| bcr_dsla_strict |              2.6730 |              6.8863 |           0.0594 |             0.0328 |          0.1662 |            632 |             80 |                0 |
| bcr_dsla        |              3.2274 |              7.4090 |           0.0712 |             0.0638 |          0.2152 |            642 |             70 |                0 |

## Full candidate versus comparators

| baseline        |   wins |   ties |   losses |   mean_gain_percent |   median_gain_percent |   mean_ci_low |   mean_ci_high |   p_holm |   p_group_holm |
|:----------------|-------:|-------:|---------:|--------------------:|----------------------:|--------------:|---------------:|---------:|---------------:|
| scct            |    637 |      7 |       68 |              7.4090 |                6.2961 |        6.9667 |         7.8613 |   0.0000 |         0.0000 |
| dsla            |    642 |     70 |        0 |              3.2274 |                1.7124 |        2.9506 |         3.5256 |   0.0000 |         0.0000 |
| heft            |    690 |      4 |       18 |              9.2600 |                7.3601 |        8.7011 |         9.8398 |   0.0000 |         0.0000 |
| left            |    679 |      3 |       30 |             11.3254 |               10.2145 |       10.6935 |        11.9586 |   0.0000 |         0.0000 |
| oppts           |    658 |      7 |       47 |              8.3621 |                7.1105 |        7.9211 |         8.8181 |   0.0000 |         0.0000 |
| aeft            |    670 |      6 |       36 |              9.2421 |                8.0735 |        8.7743 |         9.7364 |   0.0000 |         0.0000 |
| peft            |    699 |      1 |       12 |             13.0064 |               11.2395 |       12.3850 |        13.6233 |   0.0000 |         0.0000 |
| bcr_scct_seed   |    637 |      7 |       68 |              7.4090 |                6.2961 |        6.9667 |         7.8613 |   0.0000 |         0.0000 |
| bcr_dsla_seed   |    642 |     70 |        0 |              3.2274 |                1.7124 |        2.9506 |         3.5256 |   0.0000 |         0.0000 |
| bcr_scct        |    491 |     27 |      194 |              2.8425 |                1.5047 |        2.4734 |         3.2078 |   0.0000 |         0.0000 |
| bcr_dsla_single |    376 |    326 |       10 |              1.7580 |                0.0553 |        1.5210 |         2.0044 |   0.0000 |         0.0000 |
| bcr_dsla_strict |    313 |    370 |       29 |              0.5715 |                0.0000 |        0.4878 |         0.6592 |   0.0000 |         0.0000 |

Mean confidence intervals use family-stratified resampling of filename-derived DAG groups. They are sensitivity intervals, not a certificate of independent samples. Tests are two-sided, Holm-corrected across all comparisons shown.

## Family results

| family        |   n |   mean_gain_vs_dsla |   mean_gain_vs_scct |   mean_gain_vs_single |   wins_vs_dsla |   ties_vs_dsla |   losses_vs_dsla |   worst_gain_vs_dsla |   mean_runtime_s |
|:--------------|----:|--------------------:|--------------------:|----------------------:|---------------:|---------------:|-----------------:|---------------------:|-----------------:|
| FFT           |  10 |              1.5287 |              5.7642 |                0.3218 |              8 |              2 |                0 |               0.0000 |           0.0765 |
| GE            |  16 |              2.7336 |              4.9002 |                2.1751 |             12 |              4 |                0 |               0.0000 |           0.1066 |
| Genome        | 158 |              4.8434 |              9.3732 |                2.6895 |            149 |              9 |                0 |               0.0000 |           0.0572 |
| LA            |  13 |              3.8606 |             11.6054 |                2.7047 |             10 |              3 |                0 |               0.0000 |           0.0148 |
| LIGO          | 156 |              4.1068 |              9.6803 |                2.0848 |            146 |             10 |                0 |               0.0000 |           0.0571 |
| Montage       | 178 |              3.4530 |              5.4043 |                1.9170 |            166 |             12 |                0 |               0.0000 |           0.0605 |
| R-ccr0.1-b0.1 |  21 |              0.1080 |              0.5766 |                0.0733 |             12 |              9 |                0 |               0.0000 |           0.0817 |
| R-ccr0.1-b0.5 |  26 |              0.1457 |              0.3917 |                0.0239 |             18 |              8 |                0 |               0.0000 |           0.0667 |
| R-ccr0.1-b1   |  21 |              0.3781 |             -0.0194 |                0.0602 |             14 |              7 |                0 |               0.0000 |           0.0727 |
| R-ccr1-b0.1   |  17 |              1.1770 |              2.1157 |                0.3692 |             17 |              0 |                0 |               0.0629 |           0.1004 |
| R-ccr1-b0.5   |  27 |              1.7162 |              1.9862 |                0.8941 |             27 |              0 |                0 |               0.2169 |           0.1460 |
| R-ccr1-b1     |  16 |              1.0608 |              2.8965 |                0.2972 |             16 |              0 |                0 |               0.0911 |           0.1578 |
| R-ccr10-b0.1  |  19 |              0.6518 |             16.5690 |                0.5637 |             17 |              2 |                0 |               0.0000 |           0.1216 |
| R-ccr10-b0.5  |  18 |              1.6564 |             17.2859 |                0.9855 |             17 |              1 |                0 |               0.0000 |           0.1348 |
| R-ccr10-b1    |  16 |              1.7895 |             15.7169 |                1.2566 |             13 |              3 |                0 |               0.0000 |           0.0912 |

## Interpretation limits

All gains are arithmetic means of paired relative differences, in percent. The separately exported ratio-of-means measure is different. Runtime includes seed construction and is measured in the same public workflow, but the runs share hosts and are not isolated microbenchmarks. Source-hash matching and instance inclusion are recorded in bcr_analysis_manifest.json. Seed agreement here concerns makespans, not byte-identical full schedule traces. Full repair must be compared with single-task repair, not only with its seed. All candidates are development-exposed; statistical significance here is not confirmatory validation. No test-split results were computed by this analysis.
