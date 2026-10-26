
### OPPTS with its repetitions removed vs original OPPTS (all, n=2814)

| variant | mean gain vs OPPTS | shorter | longer |
|:--|--:|--:|--:|
| oppts_ft | -0.95% | 42.8% | 54.9% |
| oppts_fs | -8.28% | 18.9% | 78.9% |
| oppts_fb | -7.09% | 21.3% | 77.8% |

### SCCT vs baselines, test split (n=2102; SCCT strictly best of all 13 on 15.9%)

| baseline | mean gain | median gain | SCCT shorter | SCCT longer |
|:--|--:|--:|--:|--:|
| heft | +1.32% | +1.26% | 59.8% | 39.6% |
| peft | +5.86% | +4.65% | 77.9% | 21.3% |
| ipeft | +4.20% | +3.59% | 69.5% | 29.7% |
| mpeft | +5.46% | +3.66% | 73.2% | 25.8% |
| aeft | +1.84% | +1.54% | 71.0% | 24.4% |
| ppts | +3.91% | +3.29% | 78.0% | 21.1% |
| ippts | +24.29% | +20.65% | 96.1% | 3.8% |
| bppts | +17.45% | +14.76% | 93.8% | 5.9% |
| oppts | +0.85% | +0.97% | 62.7% | 34.8% |
| pmms | +2.08% | +1.75% | 71.0% | 28.0% |
| left | +3.31% | +3.19% | 66.9% | 32.8% |
| lstd | +2.16% | +2.47% | 64.9% | 34.4% |

### SCCT vs baselines, all split (n=2814; SCCT strictly best of all 13 on 16.3%)

| baseline | mean gain | median gain | SCCT shorter | SCCT longer |
|:--|--:|--:|--:|--:|
| heft | +1.41% | +1.29% | 60.2% | 39.1% |
| peft | +5.89% | +4.80% | 78.6% | 20.5% |
| ipeft | +4.19% | +3.64% | 69.7% | 29.5% |
| mpeft | +5.41% | +3.89% | 74.0% | 25.1% |
| aeft | +1.87% | +1.54% | 70.6% | 24.5% |
| ppts | +3.94% | +3.38% | 79.1% | 20.0% |
| ippts | +24.38% | +20.75% | 95.9% | 3.9% |
| bppts | +17.47% | +14.80% | 93.7% | 6.0% |
| oppts | +0.88% | +0.98% | 63.1% | 34.1% |
| pmms | +2.11% | +1.70% | 71.1% | 27.7% |
| left | +3.47% | +3.32% | 67.1% | 32.6% |
| lstd | +2.19% | +2.38% | 65.2% | 34.1% |

### SCCT by family (all instances)

| family | n | vs OPPTS | vs HEFT | baselines beaten | strongest baseline |
|:--|--:|--:|--:|--:|--:|
| FFT | 60 | -0.38% | +3.00% | 11/12 | oppts -0.38% |
| GE | 60 | -0.86% | +7.76% | 10/12 | oppts -0.86% |
| Genome | 600 | +0.69% | +1.98% | 12/12 | oppts +0.69% |
| LA | 60 | -0.96% | +4.70% | 11/12 | oppts -0.96% |
| LIGO | 660 | +1.47% | -1.53% | 11/12 | heft -1.53% |
| Montage | 654 | +1.21% | +5.35% | 12/12 | oppts +1.21% |
| R-ccr0.1-b0.1 | 80 | -0.30% | +0.21% | 7/12 | oppts -0.30% |
| R-ccr0.1-b0.5 | 80 | -0.41% | +0.46% | 11/12 | oppts -0.41% |
| R-ccr0.1-b1 | 80 | -0.33% | +0.88% | 11/12 | oppts -0.33% |
| R-ccr1-b0.1 | 80 | +0.80% | -0.63% | 11/12 | heft -0.63% |
| R-ccr1-b0.5 | 80 | +1.19% | +0.06% | 12/12 | heft +0.06% |
| R-ccr1-b1 | 80 | +0.57% | +0.18% | 12/12 | heft +0.18% |
| R-ccr10-b0.1 | 80 | +0.89% | -3.95% | 6/12 | left -18.19% |
| R-ccr10-b0.5 | 80 | +1.89% | -2.69% | 7/12 | left -16.34% |
| R-ccr10-b1 | 80 | +1.06% | -2.38% | 6/12 | left -14.89% |

### SCCT component ablations (all, n=2814)

| variant | change | SCCT gain over variant | variant vs OPPTS |
|:--|--:|--:|--:|
| scct_nocont | no sibling contention in the selection table | +0.18% | +0.67% |
| scct_noself | no extra weight on own cost in selection | +5.00% | -4.98% |
| scct_rankcont | rank by the contention table (first version) | +1.31% | -0.51% |
| scct_mu1 | OPPTS priority, mu = 1 | +0.31% | +0.55% |

### Priority sweeps on the dev split (n=712), gain vs OPPTS / HEFT

| variant | setting | vs OPPTS | workflows | random | vs HEFT |
|:--|--:|--:|--:|--:|--:|
| scct_mu0 | mu = 0 | -0.65% | -0.74% | -0.37% | +0.01% |
| scct_mu05 | mu = 0.5 | +0.24% | +0.19% | +0.38% | +0.91% |
| scct_mu1 | mu = 1 (OPPTS priority) | +0.70% | +0.75% | +0.56% | +1.39% |
| scct_mu15 | mu = 1.5 | +0.94% | +1.02% | +0.70% | +1.65% |
| scct | mu = 2 (chosen) | +0.97% | +1.04% | +0.74% | +1.70% |
| scct_mu3 | mu = 3 | +1.08% | +1.23% | +0.63% | +1.85% |
| scct_mu5 | mu = 5 | +0.92% | +1.06% | +0.51% | +1.69% |
| scct_mu10 | mu = 10 | +0.51% | +0.58% | +0.29% | +1.25% |
| scct_g025 | mu = 0, gamma = 0.25 | -2.07% | -2.54% | -0.67% | -1.46% |
| scct_g05 | mu = 0, gamma = 0.5 | -0.85% | -0.94% | -0.59% | -0.21% |
| scct_g075 | mu = 0, gamma = 0.75 | -0.50% | -0.51% | -0.48% | +0.16% |
| scct_rankcont | rank by contention table | -0.29% | -0.68% | +0.84% | +0.36% |
| scct_work | contention table + computation-only path | -0.24% | -0.47% | +0.44% | +0.56% |
| scct_cap1 | contention capped at p children (first-version ranking) | -0.25% | -0.59% | +0.77% | +0.41% |
