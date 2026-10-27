#!/usr/bin/env python3
"""run_ext.py - run schedulers on WfInstances-derived instances and check every schedule.

usage: run_ext.py WFINSTANCES_DIR OUT.csv name=src.cpp[:arg,arg] ...  [--limit K]

Instances (PROTOCOL.md): every WfFormat trace with 20 <= n <= 2000 tasks, crossed with
P in {4, 16}, CCR in {0.1, 1, 10}, BETA in {0.5, 1.0}; converted by wf2d3.py, checked by
tools/evaluate.cpp. Paths are relative to the repository root (R below).
"""
import csv, glob, hashlib, json, os, subprocess, sys, tempfile, time
from concurrent.futures import ProcessPoolExecutor

R = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
HERE = os.path.dirname(os.path.abspath(__file__))
B = os.path.join(tempfile.gettempdir(), "cgs_ext_build")
PS, CCRS, BETAS = (4, 16), (0.1, 1.0, 10.0), (0.5, 1.0)


def build(src):
    os.makedirs(B, exist_ok=True)
    exe = os.path.join(B, hashlib.md5(src.encode()).hexdigest()[:10])
    subprocess.run(["g++", "-O2", "-std=c++17", "-w", "-o", exe, src], check=True)
    return exe


def ntasks(path):
    return len(json.load(open(path))["workflow"]["specification"]["tasks"])


def one(job):
    trace, family, p, ccr, beta, algs, tool = job
    with tempfile.TemporaryDirectory() as t:
        d3 = f"{t}/x.d3"
        subprocess.run([sys.executable, "-I", f"{HERE}/wf2d3.py", trace, str(p), str(ccr), str(beta), d3],
                       check=True, capture_output=True)
        meta = json.loads(subprocess.run([tool, "strip", d3, f"{t}/y.d3"], capture_output=True, text=True).stdout)
        rows = []
        for name, exe, args in algs:
            out = f"{t}/o"; t0 = time.perf_counter()
            with open(d3, "rb") as fi, open(out, "wb") as fo:
                pr = subprocess.run([exe, *args], stdin=fi, stdout=fo, stderr=subprocess.DEVNULL, timeout=900)
            rt = time.perf_counter() - t0
            c = json.loads(subprocess.run([tool, "check", d3, out], capture_output=True, text=True).stdout or "{}") \
                if pr.returncode == 0 else {}
            rows.append(dict(workflow=os.path.basename(trace), family=family, p=p, ccr=ccr, beta=beta, algo=name,
                             makespan=c.get("makespan", ""), valid=c.get("valid", 0), lb=meta["lb"], n=meta["n"],
                             runtime=round(rt, 4)))
        return rows


if __name__ == "__main__":
    a = sys.argv[1:]; limit = None
    if "--limit" in a: i = a.index("--limit"); limit = int(a[i + 1]); del a[i:i + 2]
    root, out, specs = a[0], a[1], a[2:]
    tool = os.path.join(B, "evaluate"); os.makedirs(B, exist_ok=True)
    subprocess.run(["g++", "-O2", "-std=c++17", "-o", tool, f"{R}/tools/evaluate.cpp"], check=True)
    algs, built = [], {}
    for s in specs:
        name, rest = s.split("=", 1); src, _, args = rest.partition(":")
        src = src if os.path.isabs(src) else os.path.join(R, src)
        if src not in built: built[src] = build(src)
        algs.append((name, built[src], [x for x in args.split(",") if x]))
    traces = []
    for f in sorted(glob.glob(f"{root}/**/*.json", recursive=True)):
        n = ntasks(f)
        if 20 <= n <= 2000: traces.append((n, f, os.path.relpath(os.path.dirname(f), root)))
    traces.sort(key=lambda x: -x[0])
    if limit: traces = traces[-limit:]
    jobs = [(f, fam, p, c, b, algs, tool) for _, f, fam in traces for p in PS for c in CCRS for b in BETAS]
    rows = []
    with ProcessPoolExecutor(os.cpu_count()) as ex:
        for rs in ex.map(one, jobs): rows += rs
    w = csv.DictWriter(open(out, "w"), fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
    print(f"{len(traces)} workflows, {len(jobs)} instances, {len(rows)} rows -> {out}")
