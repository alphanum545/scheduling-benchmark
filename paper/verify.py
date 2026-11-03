#!/usr/bin/env python3
"""verify.py: check that a fresh run reproduces the CGS evaluation exactly.
  python3 paper/verify.py <outputs_dir>
Checks (exit status 1 on any failure):
  1. the instance index has the paper's 2814 instances (712 dev / 2102 test);
  2. every algorithm in algorithms.toml produced a valid schedule on every instance;
  3. each insertion baseline in append mode gives the repository original's schedule on every instance
     (HEFT: on 2811, the 3 known rank-tie differences listed in paper/expected/append_check.json);
  4. every generated table body is byte-identical to paper/expected/tables;
  5. every number in results.json, describe.json, why_random/why_random.json and
     why_contention/why_contention.json equals paper/expected (relative tolerance 1e-9); why_random.py
     fails unless the closure probe reproduces CGS's schedule on every test instance, and why_contention.py
     fails unless every re-run schedule's makespan and contention makespan equal runs.csv;
  6. the worked examples give the expected makespans (all schedules valid), including the results
     published by the HEFT/AEFT papers (classic DAG: HEFT 80, AEFT 76) and the Lookahead HEFT paper
     (its Fig. 1: HEFT 260, Lookahead HEFT 184).
Scheduling times (timing.json) depend on hardware and are not compared."""
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

ap = json.load(open(os.path.join(OUT, 'append_check.json')))
check(ap == json.load(open(os.path.join(EXP, 'append_check.json')))
      and all(v['identical'] == v['instances'] == 2814 for b, v in ap.items() if b != 'heft') and ap['heft']['identical'] == 2811,
      'append fidelity: ' + ', '.join(f"{b} {v['identical']}/{v['instances']}" for b, v in ap.items())
      + ' (heft_ins is a rewrite with floating-point ranks; the repository HEFT ranks in fixed point, so 3 rank ties break differently)')

for f in sorted(os.listdir(os.path.join(EXP, 'tables'))):
    a = open(os.path.join(EXP, 'tables', f), 'rb').read()
    p = os.path.join(OUT, 'tables', f)
    b = open(p, 'rb').read() if os.path.exists(p) else None
    check(a == b, f'table {f} identical to paper/expected')


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


for name in ('results.json', 'describe.json', 'why_random/why_random.json', 'why_contention/why_contention.json'):
    bad = []
    p = os.path.join(OUT, name)
    if not os.path.exists(p):
        check(False, f'{name} missing'); continue
    compare(json.load(open(os.path.join(EXP, os.path.basename(name)))), json.load(open(p)), name, bad)
    check(not bad, f'{name} matches paper/expected' + ('' if not bad else f' ({len(bad)} differences, first: {bad[0]})'))

ex = json.load(open(os.path.join(OUT, 'example', 'example.json')))
c, l = ex['classic_topcuoglu'], ex['lookahead_heft_fig1']
check(c['heft_ins'] == 80 and c['aeft_ins'] == 76 and l['heft_ins'] == 260 and l['la_heft'] == 184,
      f"published example results: classic HEFT {c['heft_ins']} (80), AEFT {c['aeft_ins']} (76); "
      f"Lookahead example HEFT {l['heft_ins']} (260), Lookahead HEFT {l['la_heft']} (184)")
check(ex == json.load(open(os.path.join(EXP, 'example.json'))), f"worked example makespans {ex}")

print('\nREPRODUCED: all checks passed' if not fails else f'\nNOT REPRODUCED: {len(fails)} check(s) failed')
sys.exit(1 if fails else 0)
