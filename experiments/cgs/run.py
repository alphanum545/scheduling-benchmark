#!/usr/bin/env python3
"""Local runner: run algorithm variants on dev instances, check with tools/evaluate, write CSV.
usage: run.py OUT.csv name=src.cpp[:arg,arg] ...  [--split dev|test|all] [--maxn N]"""
import sys, os, csv, json, subprocess, hashlib, tempfile, time
from concurrent.futures import ProcessPoolExecutor
R = "/home/claude/scheduling-benchmark"; B = "/home/claude/work/build"
def split_of(p): return "dev" if int(hashlib.sha256(p.encode()).hexdigest()[:8], 16) % 4 == 0 else "test"
def build(name, src):
    os.makedirs(B, exist_ok=True); exe = f"{B}/{name}"
    subprocess.run(["g++", "-O2", "-std=c++17", "-w", "-o", exe, src], check=True); return exe
def one(job):
    path, algs = job; tool = f"{B}/evaluate"
    with tempfile.TemporaryDirectory() as t:
        d3 = f"{t}/x.d3"; r = subprocess.run([tool, "strip", f"{R}/data/{path}", d3], capture_output=True, text=True)
        meta = json.loads(r.stdout); rows = []
        for name, exe, args in algs:
            out = f"{t}/o"; t0 = time.perf_counter()
            with open(d3, "rb") as fi, open(out, "wb") as fo:
                p = subprocess.run([exe, *args], stdin=fi, stdout=fo, stderr=subprocess.DEVNULL, timeout=600)
            rt = time.perf_counter() - t0
            c = json.loads(subprocess.run([tool, "check", d3, out], capture_output=True, text=True).stdout or "{}") if p.returncode == 0 else {}
            rows.append(dict(path=path, algo=name, makespan=c.get("makespan", ""), valid=c.get("valid", False), lb=meta["lb"], n=meta["n"], p=meta["p"], runtime=round(rt, 4)))
        return rows
if __name__ == "__main__":
    a = sys.argv[1:]; split = "dev"; maxn = 10**9
    if "--split" in a: i = a.index("--split"); split = a[i+1]; del a[i:i+2]
    if "--maxn" in a: i = a.index("--maxn"); maxn = int(a[i+1]); del a[i:i+2]
    out, specs = a[0], a[1:]
    if not os.path.exists(f"{B}/evaluate"):
        os.makedirs(B, exist_ok=True); subprocess.run(["g++", "-O2", "-std=c++17", "-o", f"{B}/evaluate", f"{R}/tools/evaluate.cpp"], check=True)
    algs, built = [], {}
    for s in specs:
        name, rest = s.split("=", 1); src, _, args = rest.partition(":")
        if src not in built: built[src] = build(hashlib.md5(src.encode()).hexdigest()[:8], src)
        algs.append((name, built[src], [x for x in args.split(",") if x]))
    idx = [r for r in csv.DictReader(open(f"{R}/data_index.csv")) if (split == "all" or split_of(r["path"]) == split) and int(r["n"]) <= maxn]
    idx.sort(key=lambda r: -int(r["n"]))
    rows = []
    with ProcessPoolExecutor(os.cpu_count()) as ex:
        for rs in ex.map(one, [(r["path"], algs) for r in idx]): rows += rs
    w = csv.DictWriter(open(out, "w"), fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
    print(f"{len(idx)} instances, {len(rows)} rows -> {out}")
