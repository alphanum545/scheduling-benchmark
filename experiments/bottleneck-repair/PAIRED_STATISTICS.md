# BCR paired statistics: scientific-notation display

Source: results/reports/bcr-dev-v1/bcr_pairwise.csv, measurement run 40312108945. The fixed-decimal p-value display in the first supplemental bcr_analysis.md is rounded, not exact zero. This corrected display changes no tests, observations, inclusion criteria or source code.

| Comparator | Wins / ties / losses | Mean paired gain, percent | Holm p | Named-DAG grouped Holm p |
|:--|:--|--:|:--|:--|
| SCCT | 637 / 7 / 68 | 7.4090 | 2.711e-103 | 1.342e-63 |
| DSLA | 642 / 70 / 0 | 3.2274 | 5.847e-106 | 1.746e-64 |
| HEFT | 690 / 4 / 18 | 9.2600 | 2.953e-114 | 6.422e-67 |
| LEFT | 679 / 3 / 30 | 11.3254 | 4.999e-113 | 7.344e-67 |
| OPPTS | 658 / 7 / 47 | 8.3621 | 4.126e-109 | 1.674e-64 |
| AEFT | 670 / 6 / 36 | 9.2421 | 9.630e-112 | 3.765e-66 |
| PEFT | 699 / 1 / 12 | 13.0064 | 1.261e-115 | 2.379e-67 |
| SCCT seed control | 637 / 7 / 68 | 7.4090 | 2.711e-103 | 1.342e-63 |
| DSLA seed control | 642 / 70 / 0 | 3.2274 | 5.847e-106 | 1.746e-64 |
| SCCT-seeded BCR | 491 / 27 / 194 | 2.8425 | 3.135e-39 | 3.493e-31 |
| DSLA-seeded single-task repair | 376 / 326 / 10 | 1.7580 | 9.424e-62 | 4.177e-46 |
| Strict-makespan DSLA-seeded BCR | 313 / 370 / 29 | 0.5715 | 1.320e-44 | 4.493e-33 |

All tests are exploratory development comparisons. Grouping by filename fields is not proof of independent DAGs. The non-worsening acceptance rule partly enforces the one-sided pattern against the included seed; effect sizes, runtime and matched repair baselines remain essential.
