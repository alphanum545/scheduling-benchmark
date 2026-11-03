# Why CGS gains less on the random DAGs (test split)

| Group | Test instances | Parents per task | Tasks with 1–2 parents % | Join ready time set earlier % | Ready time added by closure % | Joint placement gain % | Table-in-selection gain % | Top-3 baseline spread % (median) | CGS vs AEFT % |
|:--|--:|--:|--:|--:|--:|--:|--:|--:|--:|
| Workflows | 1563 | 1.9 | 97.5 | 24.4 | 64.6 | 0.89 | 3.71 | 6.94 | 2.95 |
| Random, CCR 0.1 | 172 | 7.3 | 7.8 | 68.8 | 5.3 | −0.01 | 0.80 | 0.82 | 0.41 |
| Random, CCR 1 | 180 | 7.3 | 8.5 | 74.9 | 4.1 | 0.00 | 0.62 | 2.34 | 1.71 |
| Random, CCR 10 | 187 | 7.2 | 8.1 | 65.5 | 4.2 | 2.00 | −0.09 | 2.53 | 3.70 |

Per workflow family:

| Family | Test instances | Parents per task | Tasks with 1–2 parents % | Join ready time set earlier % | Ready time added by closure % | Joint placement gain % | Table-in-selection gain % | Top-3 baseline spread % (median) | CGS vs AEFT % |
|:--|--:|--:|--:|--:|--:|--:|--:|--:|--:|
| FFT | 50 | 1.8 | 99.1 | 17.6 | 64.0 | 0.37 | 3.56 | 5.88 | 0.75 |
| GE | 44 | 1.7 | 100.0 | 24.8 | 53.5 | 1.98 | 5.38 | 8.19 | 3.58 |
| Genome | 442 | 1.2 | 97.4 | 19.0 | 74.7 | 1.20 | 5.60 | 7.86 | 2.91 |
| LA | 47 | 1.9 | 100.0 | 36.2 | 37.1 | 1.73 | 4.49 | 6.90 | 1.81 |
| LIGO | 504 | 1.6 | 95.6 | 39.2 | 42.2 | 0.04 | 3.73 | 6.48 | 2.88 |
| Montage | 476 | 2.9 | 98.9 | 24.1 | 67.6 | 1.36 | 1.74 | 7.06 | 3.34 |

Headroom (bound ignores communication; meaningful at CCR 0.1 only): median gap of AEFT / best baseline to the bound, Workflows 53.7 / 50.6 %; Random, CCR 0.1 9.3 / 9.0 %; Random, CCR 1 39.7 / 38.4 %; Random, CCR 10 85.8 / 84.9 %.
