# Public benchmark setup: manual-only, no execution authorized yet

All future DSLA evaluation must run in **alphanum545/scheduling-benchmark**, never the private research-paper repository. Do not overwrite historical results, source algorithms, cache or current benchmark workflows.

The verified code and documentation are in experiments/dsla-wfinstances-v15. Review the frozen PROTOCOL/PLAN and run the two local engineering-only tests before later executing a real workflow. The Actions template is stored in compressed form as workflow/dsla-wfinstances.yml.gz; decompress to review.

This migration DOES NOT launch GitHub Actions. For a later run, the workflow definition must be registered in .github/workflows/dsla-wfinstances.yml with GitHub workflow-write authority. It is **workflow_dispatch only**, sharing bench-store concurrency with the current benchmark workflow, 20 public ubuntu-latest jobs maximum (five families x four shards). GitHub requires manual-dispatch workflows to exist on default main. Register via a reviewed workflow-only change/PR to main, with the exact template also present on the dedicated study branch. Installing/merging a manual-only workflow does not start jobs.

When the repository owner says runners are free, select the dedicated research branch in the public repository Actions UI and manually dispatch. The frozen original DSLA at the pinned historic public commit, the relocated public comparison engine and the public WfInstances v1.5 release are used. All trace files are included according to the precommitted eligibility rule; any rejected input or failed schedule must be retained and disclosed. Require all twenty artifacts, source identity hashes, complete schedules, and the successful consolidated analysis. The public report must not imply actual cloud execution.

**Status at migration: no WfInstances execution was run.** The repository owner's busy GitHub-hosted runners were not used.
