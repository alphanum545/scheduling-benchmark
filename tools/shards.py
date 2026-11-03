"""shards.py: split per-instance work across CI jobs and merge it back exactly.

A script that computes one result per instance (a path in data_index.csv) and then aggregates can run in
three modes, chosen by its trailing arguments:

  (none)                        compute every instance, then aggregate (the local, single-machine default)
  --shard <i> <n> <out.json>    compute only shard i of n and write {path: result} to out.json; no aggregate
  --merge <file or glob> ...    read all shard files, put the results back in the original instance order,
                                then aggregate exactly as in the default mode (everything after --merge is
                                taken as files or globs, so a pattern the shell has already expanded works)

Instances are assigned to shards by greedy longest-processing-time balancing on the cost n^2 (1 + p/16)
(the harness's own cost model), so the assignment depends only on data_index.csv. Results are stored as
JSON, which round-trips Python floats exactly, and merging restores the original order, so the aggregate
is bit-identical to an unsharded run.
"""
import csv, glob, json, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def parse(argv):
    """Strip a trailing --shard/--merge clause from argv; return (rest, mode)."""
    if '--shard' in argv:
        k = argv.index('--shard')
        i, n, out = int(argv[k + 1]), int(argv[k + 2]), argv[k + 3]
        if not 0 <= i < n:
            sys.exit(f'bad shard {i} of {n}')
        return argv[:k] + argv[k + 4:], ('shard', i, n, out)
    if '--merge' in argv:
        k = argv.index('--merge')
        if k == len(argv) - 1:
            sys.exit('--merge needs at least one file or glob')
        return argv[:k], ('merge', argv[k + 1:])
    return argv, None


def _cost(row):
    return int(row['n']) ** 2 * (1 + int(row['p']) / 16)


def assignment(n, index=os.path.join(ROOT, 'data_index.csv')):
    """path -> shard, balanced on the cost model; ties broken by index order."""
    rows = list(csv.DictReader(open(index)))
    order = sorted(range(len(rows)), key=lambda k: (-_cost(rows[k]), k))
    load, where = [0.0] * n, {}
    for k in order:
        s = min(range(n), key=lambda j: (load[j], j))
        load[s] += _cost(rows[k])
        where[rows[k]['path']] = s
    return where


def mine(paths, mode):
    """The paths this invocation computes: all of them, or this shard's, in their original order."""
    if mode is None:
        return list(paths)
    if mode[0] == 'merge':
        return []
    where = assignment(mode[2])
    return [p for p in paths if where[p] == mode[1]]


def save(results, mode):
    """Write {path: result} for a shard run."""
    os.makedirs(os.path.dirname(os.path.abspath(mode[3])), exist_ok=True)
    with open(mode[3], 'w') as f:
        json.dump(results, f)
    print(f'shard {mode[1]}/{mode[2]}: {len(results)} instances -> {mode[3]}')


def load(paths, mode):
    """Results of all shards, as a list aligned with paths; fails if any path is missing or duplicated."""
    got = {}
    files = sorted({f for pat in mode[1] for f in glob.glob(pat)})
    if not files:
        sys.exit(f'no shard files match {" ".join(mode[1])}')
    for fn in files:
        for p, r in json.load(open(fn)).items():
            if p in got:
                sys.exit(f'{p} appears in more than one shard file')
            got[p] = r
    missing = [p for p in paths if p not in got]
    extra = set(got) - set(paths)
    if missing or extra:
        sys.exit(f'shard files do not cover the instances: {len(missing)} missing, {len(extra)} unexpected')
    return [got[p] for p in paths]
