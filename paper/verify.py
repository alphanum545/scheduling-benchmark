#!/usr/bin/env python3
"""verify.py: check that a fresh run reproduces the manuscript exactly.
  python3 paper/verify.py <outputs_dir>
Checks (exit status 1 on any failure):
  1. the instance index has the paper's 2814 instances (712 dev / 2102 test);
  2. every algorithm in algorithms.toml produced a valid schedule on every instance;
  3. every generated table body is byte-identical to paper/expected/tables (the manuscript's tables);
  4. every number in results.json and describe.json equals paper/expected (relative tolerance 1e-9);
  5. the Section 6 worked example gives the published makespans, with all schedules valid.
Construction times (timing.json, Fig. 4) depend on hardware and are not compared."""
import csv, hashlib, json, math, os, sys, tomllib

OUT = sys.argv[1]
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXP = os.path.join(ROOT, 'paper', 'expected')
fails = []


def check(cond, msg):
    print(('PASS  ' if cond else 'FAIL  ') + msg)
    if not cond:
        fails.append(msg)


rows = list(csv.DictReader(open(os.path.join(ROOT, 'data_index.csv'))))
dev = sum(int(hashlib.sha256(r['path'].encode()).hexdigest()[:8], 16) % 4 == 0 for r in rows)
check(len(rows) == 2814 and dev == 712, f'instance index: {len(rows)} instances, {dev} dev, {len(rows) - dev} test')

algos = set(tomllib.load(open(os.path.join(ROOT, 'algorithms.toml'), 'rb')))
runs = list(csv.DictReader(open(os.path.join(OUT, 'runs.csv'))))
ok = {(r['algo'], r['path']) for r in runs if r['status'] == 'ok'}
check(len(ok) == len(algos) * 2814 and len(runs) == len(ok),
      f'runs: {len(ok)} valid of {len(runs)} ({len(algos)} algorithms x 2814 expected)')

for f in sorted(os.listdir(os.path.join(EXP, 'tables'))):
    a = open(os.path.join(EXP, 'tables', f), 'rb').read()
    p = os.path.join(OUT, 'tables', f)
    b = open(p, 'rb').read() if os.path.exists(p) else None
    check(a == b, f'table {f} identical to the manuscript')


def compare(x, y, path, bad):
    if isinstance(x, dict):
        if set(x) != set(y): bad.append(f'{path}: keys differ'); return
        for k in x: compare(x[k], y[k], f'{path}.{k}', bad)
    elif isinstance(x, list):
        if len(x) != len(y): bad.append(f'{path}: length differs'); return
        for i, (u, v) in enumerate(zip(x, y)): compare(u, v, f'{path}[{i}]', bad)
    elif isinstance(x, float) or isinstance(y, float):
        if not math.isclose(x, y, rel_tol=1e-9, abs_tol=1e-12): bad.append(f'{path}: {y} != {x}')
    elif x != y:
        bad.append(f'{path}: {y} != {x}')


for name in ('results.json', 'describe.json'):
    bad = []
    compare(json.load(open(os.path.join(EXP, name))), json.load(open(os.path.join(OUT, name))), name, bad)
    check(not bad, f'{name} matches the manuscript' + ('' if not bad else f' ({len(bad)} differences, first: {bad[0]})'))

ex = json.load(open(os.path.join(OUT, 'example', 'example.json')))
want = dict(heft=78, peft=77, ippts=93, aeft=89, pmms=89, bppts=85, dsla=68)
check(ex['makespans'] == want and ex['dsla_builds']['rank'] == 68 and ex['dsla_builds']['eft'] == 73,
      f"worked example makespans {ex['makespans']} (valid schedules)")

print('\nREPRODUCED: all checks passed' if not fails else f'\nNOT REPRODUCED: {len(fails)} check(s) failed')
sys.exit(1 if fails else 0)
