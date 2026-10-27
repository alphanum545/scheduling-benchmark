# External benchmark (WfInstances): results, 27 October 2026

**Result: mixed.** On 3,444 instances built from 287 real workflow traces (17 families), the frozen CGS is clearly
better than HEFT, PEFT, BPPTS and IPPTS (all with insertion), modestly better than AEFT on average, and **worse than
Look-ahead HEFT on average (−1.01 %)**, although it beats Look-ahead HEFT on most instances and workflows. The
internal-benchmark ranking (CGS first) does not fully transfer. Protocol and analysis were committed before the run
(`1979aab`, [PROTOCOL.md](PROTOCOL.md)); no algorithm was changed. Every schedule (37,884) was valid.

## Main comparison

Gain = mean of 100 · (baseline − CGS) / baseline; positive favours CGS. Macro = each of the 17 families weighted
equally. *p*: two-sided Wilcoxon on the 287 per-workflow means, Holm-adjusted across the 10 comparisons.

| Baseline (insertion unless noted) | Gain | Macro gain | CGS wins / ties / losses | Workflows CGS better | Holm *p* |
|:--|--:|--:|:--|--:|--:|
| Look-ahead HEFT | **−1.01 %** | −0.81 % | 2293 / 435 / 716 | 193 / 287 | 2.0e-3 |
| SCCT | +1.16 % | +0.96 % | 1314 / 417 / 1713 | 156 / 287 | 8.3e-4 |
| AEFT | +1.65 % | +1.72 % | 1526 / 348 / 1570 | 182 / 287 | 2.5e-10 |
| HEFT | +3.45 % | +3.00 % | 2697 / 380 / 367 | 258 / 287 | 4.4e-35 |
| HEFT (non-insertion) | +4.80 % | +4.17 % | 2822 / 346 / 276 | — | 6.0e-43 |
| BPPTS | +5.77 % | +4.97 % | 2743 / 234 / 467 | — | 1.2e-38 |
| PEFT | +7.85 % | +6.38 % | 2994 / 93 / 357 | — | 9.4e-45 |
| IPPTS | +12.59 % | +11.46 % | 3030 / 74 / 340 | — | 2.3e-47 |

The Look-ahead HEFT and AEFT tests are significant but not one-directional: against Look-ahead HEFT the per-workflow
median favours CGS (+0.85 %) while the mean favours Look-ahead HEFT; against AEFT and SCCT the instance median is 0.00.

## Where CGS wins and loses

| CCR | vs Look-ahead HEFT: W / L, mean | vs AEFT: W / L, mean | vs HEFT: W / L, mean |
|:--|:--|:--|:--|
| 0.1 | 870 / 94, +1.50 % | 343 / 653, −0.26 % | 921 / 75, +1.77 % |
| 1 | 825 / 167, +0.85 % | 492 / 527, +0.78 % | 917 / 88, +3.28 % |
| 10 | 598 / 455, **−5.36 %** | 691 / 390, +4.43 % | 859 / 204, +5.30 % |

- **Look-ahead HEFT wins at high communication** on BLAST (−7.0 %), Epigenomics (−7.0 %), Cycles (−4.5 %),
  Montage (−4.2 %) and Sarek somatic (−4.1 %) (family means over all CCRs). Its full-children look-ahead captures
  fan-out costs that CGS's two-parent joint placement does not.
- **AEFT wins at low communication** (CCR 0.1: 653 losses vs 343 wins, −0.26 %), and on Sarek germline (−1.39 %),
  SRASearch (−1.78 %) and BWA (−0.95 %).
- CGS beats IPPTS in every family, and PEFT and BPPTS in every family but one each (PEFT on varlociraptor,
  −0.28 %; BPPTS on the single Nextflow ro-crate workflow, −1.66 %); it beats HEFT in 15 of 17 families (losses on
  SoyKB, −3.46 %, and Nextflow ro-crate, −2.97 %).

Per-family, per-P and per-β tables: [analysis.txt](analysis.txt).

## Components

| Ablation | Gain of full CGS | W / T / L | Holm *p* |
|:--|--:|:--|--:|
| Without joint placement (`cgs single`) | +0.59 % | 1129 / 1880 / 435 | 1.2e-7 |
| Plain rank gate (`cgs credit=0`) | +0.22 % | 348 / 2775 / 321 | 0.79 |

Joint placement of a join's parents transfers (+1.34 % at CCR 10, +3.45 % on Cycles, +2.89 % on Montage). The
communication credit in the gate does **not** transfer (not significant); it should not be claimed as a contribution.

## What this means for the paper

1. Do not claim CGS beats all published methods. Supported claim: CGS beats HEFT, PEFT, BPPTS and IPPTS on both
   benchmarks, beats AEFT on average on both, and against Look-ahead HEFT wins on the internal benchmark and on most
   external workflows but loses on average at CCR 10. Report Look-ahead HEFT's runtime (4.5× CGS's here) next to it.
2. The contribution is the gated joint placement; the communication credit is a minor, benchmark-specific detail.
3. These external results are now a held-out set. Any CGS change motivated by them (for example a wider look-ahead
   at high CCR) needs a fresh evaluation set.

Mean runtimes (2-core sandbox, two workers, not citable): CGS 0.031 s, Look-ahead HEFT 0.140 s, AEFT 0.027 s,
HEFT 0.027 s, PEFT 0.089 s, IPPTS 0.087 s, BPPTS 0.028 s.

## Reproduce

```sh
git clone https://github.com/wfcommons/WfInstances && git -C WfInstances checkout 4c3958e94f99e12c2663d4e093eb41731ca7496c
E=experiments/cgs; M=algorithms/makespan
python3 $E/external/run_ext.py WfInstances out.csv cgs=$E/cgs.cpp cgs_single=$E/cgs.cpp:single \
  cgs_credit0=$E/cgs.cpp:credit=0 heft_ins=$E/heft_ins.cpp aeft_ins=$E/aeft_ins.cpp peft_ins=$E/peft_ins.cpp \
  bppts_ins=$E/bppts_ins.cpp ippts_ins=$E/ippts_ins.cpp la_heft=$E/la_heft.cpp scct_ins=$E/scct_x.cpp:ins heft=$M/heft.cpp
python3 $E/external/analyze_ext.py out.csv
```

Raw results: `results_wfinstances.csv` (one row per instance and algorithm).
