# Benchmark report: latest

Algorithms: oppts, heft, cpop, peft, ipeft, mpeft, lstd, ppts, ippts, aeft  
Instances compared: 2094 of 2100 (only those where every algorithm is valid)

## Validity
| algo   |   invalid |   ok | example_problem                   |
|:-------|----------:|-----:|:----------------------------------|
| oppts  |         6 | 2094 | no schedule table found in output |
| heft   |         6 | 2094 | no schedule table found in output |
| cpop   |         6 | 2094 | no schedule table found in output |
| peft   |         6 | 2094 | no schedule table found in output |
| ipeft  |         6 | 2094 | no schedule table found in output |
| mpeft  |         6 | 2094 | no schedule table found in output |
| lstd   |         6 | 2094 | no schedule table found in output |
| ppts   |         6 | 2094 | no schedule table found in output |
| ippts  |         6 | 2094 | no schedule table found in output |
| aeft   |         6 | 2094 | no schedule table found in output |

## Overall averages
| algo   |   makespan |     slr |   speedup |   efficiency |   runtime_s |
|:-------|-----------:|--------:|----------:|-------------:|------------:|
| oppts  |    3279.18 | 10.9548 |   25.9273 |       1.5647 |      0.1151 |
| heft   |    3354.47 | 11.2881 |   26.2309 |       1.6069 |      0.1019 |
| cpop   |    4927.25 | 16.7617 |   15.9932 |       0.943  |      0.2078 |
| peft   |    3584.14 | 11.8045 |   24.413  |       1.4199 |      0.1187 |
| ipeft  |    3527.3  | 11.6043 |   25.092  |       1.47   |      0.1192 |
| mpeft  |    3684.75 | 11.7521 |   24.7336 |       1.4326 |      0.1168 |
| lstd   |    3375.64 | 11.3248 |   26.0275 |       1.5876 |      0.0997 |
| ppts   |    3418.96 | 11.4024 |   24.8318 |       1.4791 |      0.1006 |
| ippts  |    4419.98 | 15.5148 |   17.0815 |       0.9983 |      0.1178 |
| aeft   |    3270.98 | 10.9992 |   25.5733 |       1.5508 |      0.1117 |

## oppts versus each algorithm
| versus   |   better_pct |   equal_pct |   worse_pct |   avg_gain_pct |
|:---------|-------------:|------------:|------------:|---------------:|
| heft     |        61.17 |        0.19 |       38.63 |          1.014 |
| cpop     |        99.04 |        0.1  |        0.86 |         33.359 |
| peft     |        78.8  |        1.34 |       19.87 |          6.524 |
| ipeft    |        76.6  |        1.15 |       22.25 |          4.757 |
| mpeft    |        75.64 |        0.91 |       23.45 |          6.049 |
| lstd     |        63.94 |        0.33 |       35.72 |          1.671 |
| ppts     |        80.42 |        1.05 |       18.53 |          3.912 |
| ippts    |        97.8  |        0.19 |        2.01 |         28.089 |
| aeft     |        54.3  |        2.24 |       43.46 |          0.868 |
