#!/usr/bin/env python3
"""Decision-level examples (Section 6, Table 2).
  python3 paper/examples.py <dsla_var_binary> <evaluate_binary> <out.json>
Runs the rank build (lambda 0.5, R 3) with the conditional (state=1) and static (state=0) start
estimate on both example instances, validates every schedule, and records the processor scores at
the first decision where the two variants differ (from the independent Python port, port.py)."""
import json, os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from port import dsla, read_d3

exe, tool, out_path = sys.argv[1:4]
here = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'examples')
res = {}
for ex in ('example1', 'example2'):
    d3 = os.path.join(here, ex + '.d3')
    rec = {}
    for state in (1, 0):
        with tempfile.NamedTemporaryFile('w', suffix='.out', delete=False) as o:
            subprocess.run([exe, 'mode=0', f'state={state}'], stdin=open(d3), stdout=o, check=True)
        chk = json.loads(subprocess.run([tool, 'check', d3, o.name], capture_output=True, text=True, check=True).stdout)
        os.remove(o.name)
        trace = []
        port_mk = dsla(*read_d3(open(d3).read()), mode=0, state=bool(state), trace=trace)[0]
        rec['state' if state else 'static'] = dict(makespan=chk['makespan'], valid=bool(chk['valid']),
                                                   port_makespan=port_mk, trace=trace)
    a, b = rec['state']['trace'], rec['static']['trace']
    k = next(i for i, (x, y) in enumerate(zip(a, b)) if x['choice'] != y['choice'])
    res[ex] = dict(task=a[k]['task'] + 1,
                   dsla=dict(scores=a[k]['score'], processor=a[k]['choice'] + 1, makespan=rec['state']['makespan']),
                   static=dict(scores=b[k]['score'], processor=b[k]['choice'] + 1, makespan=rec['static']['makespan']),
                   all_valid=rec['state']['valid'] and rec['static']['valid'],
                   port_agrees=rec['state']['port_makespan'] == rec['state']['makespan'] and
                               rec['static']['port_makespan'] == rec['static']['makespan'])
json.dump(res, open(out_path, 'w'), indent=1)
for ex, r in res.items():
    print(ex, 'T%d' % r['task'], 'DSLA', r['dsla'], 'static', r['static'])
