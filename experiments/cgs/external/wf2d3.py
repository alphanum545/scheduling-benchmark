#!/usr/bin/env python3
"""wf2d3.py - convert a WfCommons WfFormat (schema 1.6) workflow trace into a heterogeneous d3 instance.

usage: wf2d3.py TRACE.json P CCR BETA OUT.d3        (writes OUT.d3 and prints a JSON summary)

Model (fixed before any CGS run on these instances; see PROTOCOL.md):
  * Tasks and edges come from workflow.specification (children/parents, union); runtimes from
    workflow.execution.tasks[].runtimeInSeconds (missing runtime -> 0, as recorded in the trace).
  * Edge data d(i,j) = total size of the files that i writes and j reads (outputFiles of i that are
    inputFiles of j). If no edge of the workflow carries file data, every edge gets d = 1.
  * Base cost: runtimes scaled so that the mean over tasks with a positive runtime is 100.
  * Heterogeneity (Topcuoglu et al., TPDS 2002): w(i,k) = round(base_i * U(1 - BETA/2, 1 + BETA/2)),
    drawn independently per task and processor; tasks with a positive runtime get w >= 1.
  * Communication: c(i,j) = round(K * d(i,j)) with K chosen so that mean_edges(c) / mean(w) = CCR
    (before rounding); the same cost on every link (uniform bandwidth), 0 on the same processor.
  * Randomness: numpy Generator seeded by SHA-256 of "<trace file name>|P|BETA", so the computation
    matrix is identical across CCR values and across machines.
"""
import hashlib, json, sys
import numpy as np


def load(path):
    d = json.load(open(path))
    w = d["workflow"]; sp = w["specification"]
    tasks = sp["tasks"]; ids = [t["id"] for t in tasks]; idx = {t: i for i, t in enumerate(ids)}
    size = {f["id"]: int(f.get("sizeInBytes") or 0) for f in sp.get("files", [])}
    rt = {t["id"]: float(t.get("runtimeInSeconds") or 0.0) for t in w.get("execution", {}).get("tasks", [])}
    edges = set()
    for t in tasks:
        for c in t.get("children", []):
            if c in idx and c != t["id"]: edges.add((idx[t["id"]], idx[c]))
        for q in t.get("parents", []):
            if q in idx and q != t["id"]: edges.add((idx[q], idx[t["id"]]))
    data = {}
    for (a, b) in edges:
        out = set(tasks[a].get("outputFiles", [])); inp = set(tasks[b].get("inputFiles", []))
        data[(a, b)] = sum(size.get(f, 0) for f in out & inp)
    if edges and all(v == 0 for v in data.values()):
        data = {e: 1 for e in edges}
    r = np.array([max(0.0, rt.get(t, 0.0)) for t in ids])
    return ids, r, data


def topo_ok(n, edges):
    indeg = [0] * n; succ = [[] for _ in range(n)]
    for a, b in edges: succ[a].append(b); indeg[b] += 1
    q = [i for i in range(n) if indeg[i] == 0]; seen = 0
    while q:
        x = q.pop(); seen += 1
        for y in succ[x]:
            indeg[y] -= 1
            if indeg[y] == 0: q.append(y)
    return seen == n


def convert(path, p, ccr, beta):
    ids, r, data = load(path)
    n = len(ids)
    if not topo_ok(n, data.keys()): raise SystemExit(f"{path}: not a DAG")
    pos = r[r > 0]
    base = r * (100.0 / pos.mean()) if len(pos) else np.full(n, 100.0)
    name = path.replace("\\", "/").split("/")[-1]
    seed = int(hashlib.sha256(f"{name}|{p}|{beta}".encode()).hexdigest()[:16], 16)
    rng = np.random.default_rng(seed)
    f = rng.uniform(1 - beta / 2, 1 + beta / 2, size=(n, p))
    W = np.rint(base[:, None] * f).astype(np.int64)
    W[(base[:, None] > 0) & (W < 1)] = 1
    C = np.full((n, n), -1, dtype=np.int64)
    if data:
        dm = np.mean(list(data.values()))
        K = ccr * W.mean() / dm if dm > 0 else 0.0
        for (a, b), v in data.items(): C[a, b] = int(round(K * v))
    return n, W, C, len(data)


def write(out, n, p, W, C):
    with open(out, "w") as fo:
        fo.write(f"{n} {p}\n")
        for row in W: fo.write(" ".join(map(str, row)) + "\n")
        for row in C: fo.write(" ".join(map(str, row)) + "\n")


if __name__ == "__main__":
    path, p, ccr, beta, out = sys.argv[1], int(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4]), sys.argv[5]
    n, W, C, e = convert(path, p, ccr, beta)
    write(out, n, p, W, C)
    cm = C[C >= 0]
    print(json.dumps({"n": n, "p": p, "edges": e, "mean_w": float(W.mean()),
                      "ccr_realised": float(cm.mean() / W.mean()) if e else 0.0}))
