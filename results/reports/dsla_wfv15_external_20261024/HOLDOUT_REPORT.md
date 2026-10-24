# Outcome-blind WfInstances external-family sensitivity

Original application DAG structure and individual task runtimes came from WfInstances Pegasus execution logs, but processor heterogeneous costs, bandwidth, and no-contention schedule completion are MODELLED. Families were selected before outcomes; no performance-based exclusion.

Input sources: 92; eligible traces: 89; predeclared exclusions: 3; primary modeled cases (p=2,4,8,16): 712.

| Comparator | Equal-family mean gain (%) | 95% trace-group CI | Win/tie/loss | Worst gain (%) |
|---|---:|---|---:|---:|
| heft_insertion_reimplementation | 0.809 | [0.672, 0.940] | 522/38/152 | -6.089 |
| peft_insertion_reimplementation | 5.094 | [4.615, 5.601] | 643/9/60 | -12.649 |
| portfolio_heft_peft | 0.630 | [0.479, 0.772] | 495/30/187 | -12.649 |
| portfolio_static | 0.029 | [-0.001, 0.058] | 176/394/142 | -1.788 |
| portfolio_no_offset | 0.374 | [0.291, 0.456] | 474/60/178 | -6.089 |
| portfolio_all_children | 0.001 | [-0.000, 0.002] | 4/706/2 | -0.013 |

All finite cases were retained. Negative improvements and intervals spanning zero are not superiority evidence.
Predeclared five-source-family stratified bootstrap resamples each trace as the unit and gives every family equal headline weight.

## Reference-processor efficiency (modelled processor speeds, not homogeneous real hardware)
- p=2: mean speedup 2.410; mean efficiency 1.205; below 0.8 in 0.0% of cases
- p=4: mean speedup 5.297; mean efficiency 1.324; below 0.8 in 0.0% of cases
- p=8: mean speedup 10.915; mean efficiency 1.364; below 0.8 in 6.7% of cases
- p=16: mean speedup 21.927; mean efficiency 1.370; below 0.8 in 13.5% of cases
