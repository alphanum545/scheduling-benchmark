#!/usr/bin/env python3
"""bench.py: plan, run and record scheduling-algorithm benchmarks.

Commands
  index   Scan a data folder once: detect each file's format (D1/D2), size, lower bound.
          Writes data_index.csv, which planning uses (so planning needs no data download).
  plan    Select instances and algorithms, skip runs already in results/store.csv,
          and split the remaining work into balanced shards.
  run     Execute one shard (or everything) in parallel on all CPU cores.
  merge   Fold shard result files into results/store.csv.

Data layout: <data>/<Family>/<N>Nodes/<P>PROCESSORS/<i>_<P>.txt  (any extra folder level above
<Family> is ignored). The file format is detected from its exact token count, so D1 (6 header
vectors) and D2 (5 header vectors) files are both handled by the same algorithm source.

Local use
  python3 tools/bench.py index --data data
  python3 tools/bench.py plan  --algos heft,aeft --families GE,LA --shards 1
  python3 tools/bench.py run   --data data
  python3 tools/bench.py merge
  python3 tools/report.py --algos heft,aeft --highlight aeft

Configure algorithms in algorithms.toml. Needs Python 3.11+ (stdlib only) and g++.
"""
import argparse, csv, glob, hashlib, json, os, re, shutil, subprocess, sys, tempfile, time, tomllib
from concurrent.futures import ThreadPoolExecutor, as_completed

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = os.path.join(ROOT, ".work")
BUILD = os.path.join(WORK, "build")
STORE = os.path.join(ROOT, "results", "store.csv")
INDEX = os.path.join(ROOT, "data_index.csv")
PATH_RE = re.compile(r"(?:^|/)(?P<family>[^/]+)/(?P<nodes>\d+)Nodes/(?P<procs>\d+)PROCESSORS/(?P<inst>\d+)_\d+\.txt$")

FIELDS = ["dataset", "family", "nodes", "n", "p", "instance", "path", "algo", "algo_hash", "status",
          "makespan", "lb", "seq", "slr", "speedup", "efficiency", "runtime_s", "violations", "first_violation"]
INDEX_FIELDS = ["path", "family", "asset", "dataset", "nodes", "n", "p", "instance", "edges", "lb", "seq", "size"]
VALID = ("ok", "unchecked")
RETRY = ("missing_file", "check_error")  # infrastructure problems: never treated as done

# ------------------------------------------------------------------ config
def load_algos():
    with open(os.path.join(ROOT, "algorithms.toml"), "rb") as f:
        cfg = tomllib.load(f)
    algos = {}
    for name, a in cfg.items():
        a = dict(a)
        a.setdefault("input", "d3"); a.setdefault("output", "schedule"); a.setdefault("args", [])
        a.setdefault("duplication", False); a.setdefault("enabled", True); a.setdefault("timeout", 900)
        a.setdefault("flags", "-O2 -std=c++17")
        if a["input"] not in ("d3", "d2", "raw"):
            sys.exit(f"{name}: input must be d3, d2 or raw")
        src = os.path.join(ROOT, a["source"])
        h = hashlib.sha256(open(src, "rb").read())
        h.update(json.dumps([a["input"], a["output"], a["args"], a["duplication"], a["flags"]]).encode())
        a["hash"] = h.hexdigest()[:12]
        algos[name] = a
    return algos

def pick_algos(spec, algos):
    if spec in (None, "", "all"):
        return [k for k, a in algos.items() if a["enabled"]]
    names = [s.strip() for s in spec.split(",") if s.strip()]
    missing = [s for s in names if s not in algos]
    if missing:
        sys.exit(f"unknown algorithm(s): {missing}. Known: {list(algos)}")
    return names

def csvset(s, cast=str):
    return None if s in (None, "", "all") else {cast(x.strip()) for x in str(s).split(",") if x.strip()}

# ------------------------------------------------------------------ tools
def build_tool():
    os.makedirs(BUILD, exist_ok=True)
    tool = os.path.join(BUILD, "evaluate")
    src = os.path.join(ROOT, "tools", "evaluate.cpp")
    if not os.path.exists(tool) or os.path.getmtime(tool) < os.path.getmtime(src):
        subprocess.run(["g++", "-O2", "-std=c++17", "-o", tool, src], check=True)
    return tool

def shim_dir():
    shim = os.path.join(BUILD, "shim")
    hdr = os.path.join(shim, "bits", "stdc++.h")
    if not os.path.exists(hdr):
        os.makedirs(os.path.dirname(hdr), exist_ok=True)
        names = ["algorithm", "array", "bitset", "cassert", "cctype", "cfloat", "climits", "cmath", "cstdio",
                 "cstdlib", "cstring", "ctime", "deque", "fstream", "functional", "iomanip", "iostream",
                 "iterator", "limits", "list", "map", "numeric", "queue", "random", "set", "sstream", "stack",
                 "string", "tuple", "unordered_map", "unordered_set", "utility", "vector", "chrono"]
        open(hdr, "w").write("".join(f"#include <{h}>\n" for h in names))
    return shim

def build_algos(names, algos):
    os.makedirs(BUILD, exist_ok=True)
    shim = shim_dir()
    def one(n):
        a = algos[n]
        exe = os.path.join(BUILD, f"{n}-{a['hash']}")
        if os.path.exists(exe):
            return None
        cmd = ["g++", *a["flags"].split(), "-w", "-o", exe, os.path.join(ROOT, a["source"])]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode:  # compilers without bits/stdc++.h (macOS clang)
            r = subprocess.run(cmd[:1] + ["-I", shim] + cmd[1:], capture_output=True, text=True)
        return f"build failed for {n}:\n{r.stderr[:2000]}" if r.returncode else None
    with ThreadPoolExecutor(os.cpu_count() or 1) as ex:
        errs = [e for e in ex.map(one, names) if e]
    if errs:
        sys.exit("\n".join(errs))

# ------------------------------------------------------------------ index
def cmd_index(a):
    tool = build_tool()
    files = []
    for path in glob.glob(os.path.join(a.data, "**", "*.txt"), recursive=True):
        rel = os.path.relpath(path, a.data).replace(os.sep, "/")
        if "__MACOSX" in rel or os.path.basename(rel).startswith("._"):
            continue
        m = PATH_RE.search(rel)
        if m:
            files.append((rel, m))
    if not files:
        sys.exit(f"no instance files found under {a.data}")
    def scan(item):
        rel, m = item
        r = subprocess.run([tool, "strip", os.path.join(a.data, rel), "-"], capture_output=True, text=True)
        if r.returncode:
            return rel, None, r.stderr.strip()
        meta = json.loads(r.stdout)
        d = m.groupdict()
        return rel, dict(path=rel, family=d["family"], dataset=meta["format"], nodes=int(d["nodes"]),
                         n=meta["n"], p=meta["p"], instance=int(d["inst"]), edges=meta["edges"],
                         lb=meta["lb"], seq=meta["seq"], size=os.path.getsize(os.path.join(a.data, rel))), ""
    mpath = os.path.join(a.data, ".assets.json")
    assets = json.load(open(mpath)) if os.path.exists(mpath) else {}
    rows, bad = [], []
    with ThreadPoolExecutor(os.cpu_count() or 1) as ex:
        for rel, row, err in ex.map(scan, files):
            if row is None:
                bad.append((rel, err))
            else:
                if int(PATH_RE.search(rel).group("procs")) != row["p"]:
                    bad.append((rel, f"folder says {PATH_RE.search(rel).group('procs')} processors, file says {row['p']}"))
                row["asset"] = assets.get(row["family"], "")
                rows.append(row)
    rows.sort(key=lambda r: (r["family"], r["nodes"], r["p"], r["instance"]))
    with open(a.out, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=INDEX_FIELDS); w.writeheader(); w.writerows(rows)
    fams = {}
    for r in rows:
        fams.setdefault((r["family"], r["dataset"]), 0); fams[(r["family"], r["dataset"])] += 1
    print(f"indexed {len(rows)} instances -> {os.path.relpath(a.out, ROOT)}")
    for (fam, ds), k in sorted(fams.items()):
        print(f"  {fam:12s} {ds}  {k} files")
    if bad:
        print(f"{len(bad)} problem files:")
        for rel, err in bad[:50]:
            print(f"  ! {rel}: {err}")
        if a.strict:
            sys.exit(1)

def read_index():
    if not os.path.exists(INDEX):
        sys.exit("data_index.csv not found: run `bench.py index --data <folder>` first")
    with open(INDEX, newline="") as f:
        return list(csv.DictReader(f))

# ------------------------------------------------------------------ store
def read_store():
    if not os.path.exists(STORE):
        return {}
    with open(STORE, newline="") as f:
        return {(r["algo"], r["algo_hash"], r["path"]): r for r in csv.DictReader(f)}

def done_keys():
    return {k for k, r in read_store().items() if r["status"] not in RETRY}

def write_rows(path, rows, fields=FIELDS):
    os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields); w.writeheader()
        for r in rows:
            w.writerow({k: r.get(k, "") for k in fields})

# ------------------------------------------------------------------ plan
def select(index, a):
    sel = {k: csvset(getattr(a, k), int if k in ("nodes", "procs", "instances") else str)
           for k in ("datasets", "families", "nodes", "procs", "instances")}
    out = []
    for r in index:
        if sel["datasets"] and r["dataset"] not in sel["datasets"]: continue
        if sel["families"] and r["family"] not in sel["families"]: continue
        if sel["nodes"] and int(r["nodes"]) not in sel["nodes"]: continue
        if sel["procs"] and int(r["p"]) not in sel["procs"]: continue
        if sel["instances"] and int(r["instance"]) not in sel["instances"]: continue
        out.append(r)
    return out

def cmd_plan(a):
    algos = load_algos()
    names = pick_algos(a.algos, algos)
    insts = select(read_index(), a)
    if not insts:
        sys.exit("no instances match the selection")
    store = set() if a.force else done_keys()
    groups, cached = [], 0
    for r in insts:
        todo = [n for n in names if (n, algos[n]["hash"], r["path"]) not in store]
        cached += len(names) - len(todo)
        if todo:
            # cost model: reading/processing is ~n^2, processor loops add a factor of p
            unit = int(r["n"]) ** 2 * (1 + int(r["p"]) / 16)
            # small instances: all algorithms in one group (instance prepared once);
            # large instances: one group per algorithm so they spread across cores and shards
            parts = [todo] if int(r["n"]) < a.split_nodes else [[n] for n in todo]
            for part in parts:
                groups.append(dict(path=r["path"], family=r["family"], algos=part, cost=unit * len(part)))
    k = max(1, min(a.shards, len(groups)))
    shards = [dict(cost=0, groups=[]) for _ in range(k)]
    for g in sorted(groups, key=lambda g: -g["cost"]):
        s = min(shards, key=lambda s: s["cost"]); s["groups"].append(g); s["cost"] += g["cost"]
    plan = dict(algos=names, shards=[s["groups"] for s in shards],
                runs_total=len(insts) * len(names), runs_cached=cached,
                runs_todo=sum(len(g["algos"]) for g in groups))
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, "w") as f:
        json.dump(plan, f)
    print(f"instances: {len(insts)}  algorithms: {len(names)}  runs: {plan['runs_total']}  "
          f"already done: {cached}  to run: {plan['runs_todo']}  shards: {k if groups else 0}")
    if a.github_output:
        matrix = [dict(shard=i, families=" ".join(sorted({g["family"] for g in s["groups"]})))
                  for i, s in enumerate(shards) if s["groups"]]
        with open(a.github_output, "a") as f:
            f.write(f"matrix={json.dumps(matrix)}\n")
            f.write(f"todo={plan['runs_todo']}\n")

# ------------------------------------------------------------------ run
def run_group(g, data, algos, tool, tmp):
    raw = os.path.join(data, g["path"])
    d = PATH_RE.search(g["path"]).groupdict()
    base = dict(family=d["family"], nodes=int(d["nodes"]), instance=int(d["inst"]), path=g["path"])
    key = hashlib.md5((g["path"] + "|" + ",".join(g["algos"])).encode()).hexdigest()
    d3, d2 = os.path.join(tmp, key + ".d3"), os.path.join(tmp, key + ".d2")
    need_d2 = any(algos[n]["input"] == "d2" for n in g["algos"])
    if not os.path.exists(raw):
        return [dict(base, algo=n, algo_hash=algos[n]["hash"], status="missing_file") for n in g["algos"]]
    r = subprocess.run([tool, "strip", raw, d3] + ([d2] if need_d2 else []), capture_output=True, text=True)
    if r.returncode:
        return [dict(base, algo=n, algo_hash=algos[n]["hash"], status="bad_instance",
                     first_violation=r.stderr.strip()[:300]) for n in g["algos"]]
    meta = json.loads(r.stdout)
    base.update(dataset=meta["format"], n=meta["n"], p=meta["p"], lb=meta["lb"], seq=meta["seq"])
    rows = []
    for n in g["algos"]:
        a = algos[n]
        row = dict(base, algo=n, algo_hash=a["hash"])
        exe = os.path.join(BUILD, f"{n}-{a['hash']}")
        inp = {"d3": d3, "d2": d2, "raw": raw}[a["input"]]
        outf = d3 + f".{n}.out"
        t0 = time.perf_counter()
        try:
            with open(inp, "rb") as fi, open(outf, "wb") as fo:
                p = subprocess.run([exe, *map(str, a["args"])], stdin=fi, stdout=fo,
                                   stderr=subprocess.DEVNULL, timeout=a["timeout"])
            row["runtime_s"] = round(time.perf_counter() - t0, 4)
        except subprocess.TimeoutExpired:
            row.update(status="timeout", runtime_s=a["timeout"]); rows.append(row); continue
        if p.returncode:
            row["status"] = f"crash({p.returncode})"; rows.append(row); os.remove(outf); continue
        if a["output"] == "schedule":
            cmd = [tool, "check", d3, outf] + (["--dup-entry"] if a["duplication"] else [])
            cr = subprocess.run(cmd, capture_output=True, text=True)
            try:
                c = json.loads(cr.stdout)
            except json.JSONDecodeError:
                row.update(status="check_error", first_violation=(cr.stderr or cr.stdout).strip()[:300])
                rows.append(row); os.remove(outf); continue
            mk = c["makespan"]
            row.update(makespan=mk, violations=c["violations"], first_violation=c["first"],
                       status="ok" if c["valid"] else "invalid")
        else:  # makespan-only output: the last number printed
            nums = re.findall(r"-?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?", open(outf).read())
            os.remove(outf)
            if not nums:
                row["status"] = "no_output"; rows.append(row); continue
            mk = float(nums[-1])
            low = mk < meta["lb"]
            row.update(makespan=mk, status="invalid" if low else "unchecked",
                       first_violation="makespan below the critical-path lower bound" if low else "")
        if os.path.exists(outf):
            os.remove(outf)
        if mk:
            row.update(slr=round(mk / meta["lb"], 6) if meta["lb"] else "",
                       speedup=round(meta["seq"] / mk, 6), efficiency=round(meta["seq"] / mk / meta["p"], 6))
        rows.append(row)
    for f in (d3, d2):
        if os.path.exists(f):
            os.remove(f)
    return rows

def cmd_run(a):
    with open(a.plan) as f:
        plan = json.load(f)
    algos = load_algos()
    groups = plan["shards"][a.shard] if a.shard is not None else [g for s in plan["shards"] for g in s]
    if a.out is None:
        a.out = os.path.join(WORK, f"shard-{a.shard if a.shard is not None else 'all'}.csv")
    if not groups:
        print("nothing to run"); write_rows(a.out, []); return
    tool = build_tool()
    build_algos(sorted({n for g in groups for n in g["algos"]}), algos)
    workers = a.workers or os.cpu_count() or 1
    tmp = tempfile.mkdtemp(prefix="bench-")
    rows, t0, done = [], time.time(), 0
    total = sum(len(g["algos"]) for g in groups)
    step = max(1, total // 20)
    with ThreadPoolExecutor(workers) as ex:
        futs = [ex.submit(run_group, g, a.data, algos, tool, tmp)
                for g in sorted(groups, key=lambda g: -g["cost"])]  # big instances first
        nxt = step
        for fu in as_completed(futs):
            rs = fu.result(); rows += rs; done += len(rs)
            for r in rs:
                if r["status"] not in VALID:
                    print(f"  ! {r['algo']:12s} {r['path']}: {r['status']} {r.get('first_violation', '')}")
            if done >= nxt or done == total:
                print(f"[{done}/{total}] {time.time() - t0:.0f}s", flush=True); nxt = done + step
    shutil.rmtree(tmp, ignore_errors=True)
    write_rows(a.out, rows)
    ok = sum(r["status"] in VALID for r in rows)
    print(f"done: {len(rows)} runs, {ok} valid, {len(rows) - ok} flagged, "
          f"{time.time() - t0:.0f}s on {workers} workers")

# ------------------------------------------------------------------ merge
def cmd_merge(a):
    store = read_store()
    # Drop rows for instances that are no longer in the index (e.g. a family was
    # re-generated and a file was dropped, or a malformed file was excluded).
    # The store is cumulative, so without this those rows would keep showing up
    # in the Validity sheet forever.
    if os.path.exists(INDEX):
        live = {r["path"] for r in csv.DictReader(open(INDEX))}
        stale = [k for k in store if k[2] not in live]
        for k in stale:
            del store[k]
        if stale:
            print(f"pruned {len(stale)} stored row(s) for instances no longer in the index")
    n = 0
    for path in sorted(glob.glob(a.inputs)):
        with open(path, newline="") as f:
            for r in csv.DictReader(f):
                store[(r["algo"], r["algo_hash"], r["path"])] = r; n += 1
    write_rows(STORE, sorted(store.values(), key=lambda r: (r["path"], r["algo"], r["algo_hash"])))
    print(f"merged {n} rows; store now has {len(store)} rows")

# ------------------------------------------------------------------ cli
def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest="cmd", required=True)
    i = sp.add_parser("index")
    i.add_argument("--data", default=os.path.join(ROOT, "data"))
    i.add_argument("--out", default=INDEX)
    i.add_argument("--strict", action="store_true", help="fail if any file is unreadable")
    p = sp.add_parser("plan")
    p.add_argument("--algos", default="all")
    for k in ("datasets", "families", "nodes", "procs", "instances"):
        p.add_argument(f"--{k}", default="all")
    p.add_argument("--shards", type=int, default=1)
    p.add_argument("--split-nodes", type=int, default=500, help="instances with at least this many tasks run each algorithm as a separate job")
    p.add_argument("--force", action="store_true", help="ignore stored results and rerun everything")
    p.add_argument("--out", default=os.path.join(WORK, "plan.json"))
    p.add_argument("--github-output", default=None)
    r = sp.add_parser("run")
    r.add_argument("--data", default=os.path.join(ROOT, "data"))
    r.add_argument("--plan", default=os.path.join(WORK, "plan.json"))
    r.add_argument("--shard", type=int, default=None)
    r.add_argument("--workers", type=int, default=0)
    r.add_argument("--out", default=None)
    m = sp.add_parser("merge")
    m.add_argument("--inputs", default=os.path.join(WORK, "shard-*.csv"))
    a = ap.parse_args()
    {"index": cmd_index, "plan": cmd_plan, "run": cmd_run, "merge": cmd_merge}[a.cmd](a)

if __name__ == "__main__":
    main()
