# What the contention replay measures (test split)

Mean difference from AEFT (%, positive = larger than AEFT). Inflation = mean contention makespan / planned makespan.

| Method | Inflation (workflows) | Planned | Remote volume | Busiest port | Contention makespan | Remote volume, random CCR 10 | Contention makespan, random CCR 10 |
|:--|--:|--:|--:|--:|--:|--:|--:|
| CGS | 11.93 | −2.95 | 2.45 | 2.49 | 1.72 | −10.01 | −6.52 |
| CGS without joint placement | 11.76 | −2.04 | 3.06 | 2.19 | 1.53 | 1.10 | −2.36 |
| quad selection weight 1 | 11.52 | −1.00 | 0.64 | 0.91 | 1.08 | 5.36 | −2.02 |
| quad selection weight 2 | 10.90 | 3.22 | −0.62 | −0.30 | 1.16 | 10.22 | −1.29 |
| CGS with ungated closures | 11.44 | 2.99 | −3.76 | −1.15 | 0.38 | −20.09 | −7.67 |
| AEFT | 11.23 | --- | --- | --- | --- | --- | --- |
| PMMS | 11.25 | 1.41 | 2.86 | 2.32 | 1.61 | −1.60 | 1.76 |
| HEFT | 11.20 | 4.93 | 11.96 | 5.40 | 4.88 | −3.39 | −1.31 |
| PEFT | 10.61 | 7.85 | −5.02 | −0.71 | −0.69 | −7.61 | −0.35 |
| Lookahead HEFT | 10.88 | 6.24 | −3.15 | −1.79 | −0.86 | −57.77 | −32.16 |
| BPPTS | 9.66 | 15.97 | −10.44 | −4.94 | −1.53 | 2.58 | 2.29 |

| Group | Instances | Inflation range (×) | corr(contention, port) | corr(contention, planned) | CGS losses to AEFT | % of losses with heavier port | P(lose | heavier) % | P(lose | lighter) % |
|:--|--:|--:|--:|--:|--:|--:|--:|--:|
| workflow | 1563 | 9.7–11.9 | 0.69 | 0.06 | 903 | 74.4 | 75.4 | 34.4 |
| FFT | 50 | 1.9–2.4 | 0.71 | 0.05 | 28 | 78.6 | 73.3 | 30.0 |
| GE | 44 | 3.1–3.4 | 0.56 | 0.37 | 23 | 73.9 | 68.0 | 31.6 |
| Genome | 442 | 3.9–5.0 | 0.83 | -0.01 | 234 | 81.2 | 73.4 | 24.0 |
| LA | 47 | 3.8–4.2 | 0.46 | 0.23 | 27 | 70.4 | 73.1 | 38.1 |
| LIGO | 504 | 6.2–8.2 | 0.63 | -0.05 | 309 | 71.8 | 73.5 | 43.1 |
| Montage | 476 | 20.7–24.9 | 0.62 | 0.17 | 282 | 71.6 | 81.1 | 35.2 |
| ccr0.1 | 172 | 1.2–1.2 | 0.02 | 0.69 | 80 | 53.8 | 51.2 | 42.0 |
| ccr1 | 180 | 5.7–6.0 | 0.51 | 0.37 | 68 | 57.4 | 58.2 | 25.7 |
| ccr10 | 187 | 15.3–20.6 | 0.83 | 0.07 | 78 | 43.6 | 60.7 | 33.6 |
