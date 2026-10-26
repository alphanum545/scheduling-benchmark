# BCR development results and research decision

**Decision: retain BCR as a stronger measured candidate; novelty and unseen-corpus validation are not established.**

Repository: alphanum545/scheduling-benchmark. Branch: research/bottleneck-repair-20261026. Measurement workflow: 40312108945. Source commit: e508503637b660bb9395222f9d78c464bf637cf9. Supplemental audit: 40312825348. Baseline main remains 3741b6d4a61a9599284f78c23a826658b81f7500.

## What completed

The existing public Benchmark workflow executed 9,256 fresh measurements: 13 configurations on all 712 selected development instances, with 20 shards. Every selected algorithm returned a valid schedule on every selected instance. The later audit scheduled zero new measurements; results.csv retained Git blob 30d72a746fc0aff9dd074da7b00b7651b1a56afe. [Measurement plan log; audit plan log; results/reports/bcr-dev-v1/summary.md and bcr_analysis_manifest.json]

The SCCT and DSLA seed controls have zero per-instance makespan mismatches. This is makespan conformance on the measured inputs, not proof of byte-identical full schedules for all possible inputs. [bcr_analysis_manifest.json]

## Main results

All percentages below are mean paired relative makespan improvements, not the percentage reduction of a pooled average. Positive means BCR has shorter schedules. The full candidate uses DSLA as its seed. [bcr_pairwise.csv]

| Comparator | Mean gain, percent | BCR wins / ties / losses |
|:--|--:|:--|
| DSLA | 3.2 | 642 / 70 / 0 |
| SCCT | 7.4 | 637 / 7 / 68 |
| HEFT | 9.3 | 690 / 4 / 18 |
| LEFT | 11 | 679 / 3 / 30 |
| OPPTS | 8.4 | 658 / 7 / 47 |
| AEFT | 9.2 | 670 / 6 / 36 |
| PEFT | 13 | 699 / 1 / 12 |

The full candidate's mean paired improvement over DSLA is 3.2 percent; reduction in the raw arithmetic mean makespan is only 0.97 percent. Both measures must remain distinct. The DSLA comparison's family-stratified named-DAG bootstrap sensitivity interval is approximately 3.0 to 3.5 percent. Filename-derived groups have not been structurally certified as independent DAGs. [bcr_pairwise.csv; bcr_analysis_manifest.json]

The full candidate averages 0.071 seconds including construction of the seed, versus 0.036 seconds for DSLA, approximately a twofold runtime increase. All baseline timings were refreshed in the same public run; they are shared-runner measurements rather than isolated microbenchmarks. [bcr_configurations.csv]

## Component evidence

| Configuration | Mean paired gain versus DSLA, percent | Mean runtime, seconds |
|:--|--:|--:|
| Single-task repair | 1.5 | 0.044 |
| Joint repair, strict makespan decrease | 2.7 | 0.059 |
| Full joint repair, lexicographic acceptance | 3.2 | 0.071 |

Full repair improves over the single-task ablation by 1.8 percent in a direct paired comparison: 376 wins, 326 ties and 10 losses. The grouped Holm-adjusted Wilcoxon p-value is 4.18e-46. The strict-acceptance comparison gives a further 0.57 percent mean paired gain, with 313 wins, 370 ties and 29 losses. These gains are not additive. Evaluation caps are matched; realized wall-clock times are not. [bcr_pairwise.csv; bcr_configurations.csv]

Applying full repair to SCCT instead of DSLA improves SCCT by 4.7 percent. However, that SCCT-seeded configuration improves over original DSLA by only 0.022 percent on average and loses to DSLA on 322 instances. Do not attribute the full candidate's entire gain to a repaired SCCT-only algorithm. [bcr_configurations.csv]

## Family results and losses

The full candidate improves mean paired makespan over DSLA in every reported family. Its workflow gains are 1.5 percent for FFT, 2.7 for GE, 4.8 for Epigenomic (repository label Genome), 3.9 for LA, 4.1 for LIGO and 3.5 for Montage. [bcr_families.csv]

At CCR=10, gains over DSLA are 0.65, 1.7 and 1.8 percent for the three heterogeneity settings. The larger gains over SCCT there include the benefit of the DSLA seed. The low-CCR family R-ccr0.1-b1 still has a negative mean comparison with SCCT: -0.019 percent. Across the whole development corpus, the full candidate loses to SCCT on 68 cases, with a worst paired loss of approximately 11 percent. [bcr_families.csv; bcr_pairwise.csv]

## Mechanism and guarantee

BCR retains the seed topological order and moves bounded groups of bottleneck tasks jointly to a processor. It evaluates changed precedence and processor-order timings by exact causal replay. Acceptance requires a lexicographic decrease in makespan and then the sum of task finish times. Feasibility follows because all processor-order arcs respect the retained topological order. Accepted schedules cannot have worse makespan than their included seed under the same deterministic cost model. No optimality or universal dominance is claimed. [algorithms/makespan/bcr.cpp; PROTOCOL.md]

The engineering tests contain a specified fan-in incumbent with makespan 12, no improving single relocation, and a joint relocation producing makespan 5. That incumbent is constructed, not asserted to be DSLA's output. The sanitizer suite also checked 100 tiny random DAGs, 6,000 incremental/full trial calls and 200 seed replay checks without failure. These are correctness tests, not performance observations. [test_bcr.cpp; UNIT_TESTS.txt]

## Publication boundaries

Critical-path tabu search and variable-neighborhood methods for heterogeneous DAG scheduling already exist: Wong et al., DOI 10.1109/ICPADS.2009.127, and Selvi and Manimegalai, DOI 10.1080/08839514.2017.1300010. BCR cannot claim that local repair, critical-path guidance or neighborhood search is itself novel. The exact bounded joint-group design needs a full prior-art comparison and a strong runtime-matched repair baseline.

The repository's baseline implementations use non-insertion placement. Do not describe these experiments as certified canonical insertion-based HEFT/PEFT comparisons. No new test-split study was run; earlier SCCT test outcomes have been seen, and path-based splits do not establish graph disjointness. Fresh structurally disjoint external validation is required. All statistical results here are exploratory development evidence.

The first supplemental Markdown table rounded very small p-values to 0.0000. The scientific-notation CSV is authoritative; these are not exactly zero probabilities. See PAIRED_STATISTICS.md for the corrected display. The error affects display only, not observations or tests.

## Source of truth

Detailed outputs are in results/reports/bcr-dev-v1/: summary.md, results.csv, bcr_configurations.csv, bcr_pairwise.csv, bcr_families.csv and bcr_analysis_manifest.json. All loss rows and original measurements are retained.

Change log: implemented and tested BCR; completed fresh development benchmark and report-only audit; recorded both positive and negative evidence; left main and workflow YAML unchanged.
