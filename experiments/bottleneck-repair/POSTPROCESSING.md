# Supplemental audit provenance

Measurement workflow: 40312108945; source checkpoint e508503637b660bb9395222f9d78c464bf637cf9. All chosen algorithms were forced to execute in that run. The subsequent invocation uses force=false with exactly the same algorithms and source hashes; it should schedule zero new measurements and only rebuild report outputs.

The existing tools/report.py is preserved byte-for-byte as tools/report_bcr_original.py in the same directory. A small wrapper invokes it unchanged, then runs experiments/bottleneck-repair/analyze.py only for report bcr-dev-v1. Workflow YAML, schedulers, dataset index, split rules and original metric calculations are unchanged. New exports provide exact seed checks, paired family results, scheduler-runtime distributions, and family-stratified named-DAG-cluster bootstrap intervals. No observations are selected or dropped; missing, duplicate or invalid comparisons fail the audit.

analyze.py was tested locally on clearly marked artificial rows before commit; those rows are not benchmark data and are not committed to the research results. Bootstrap settings and filename grouping are recorded in the generated manifest. Filename-derived groups have not been structurally certified as independent DAGs.
