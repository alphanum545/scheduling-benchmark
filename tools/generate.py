#!/usr/bin/env python3
"""Generate random layered DAG instances in the same file format and folder layout as D1/D2.

  python3 tools/generate.py --out data/R --nodes 50,100 --procs 2,4,8 --ccr 0.1,1,10 --beta 0.5 --count 10

Writes  <out>/<family>/<N>Nodes/<P>PROCESSORS/<i>_<P>.txt  where family = <prefix>ccr<C>-b<B>.
Use --format D1 or D2 (default D2). Energy parameter vectors are filled with plausible random values.
"""
import argparse, os, random

def random_dag(n, p, ccr, beta, fat, density, rnd, wdag=50):
    inner = max(n - 2, 1)
    width = max(1, int(round((inner ** 0.5) * fat * 2)))
    levels, left = [], inner
    while left > 0:
        w = min(left, rnd.randint(max(1, width // 2), width)); levels.append(w); left -= w
    ids, nxt = [], 1
    for w in levels:
        ids.append(list(range(nxt, nxt + w))); nxt += w
    N = nxt + 1
    comm = [[-1] * N for _ in range(N)]
    wbar = [rnd.uniform(1, 2 * wdag) for _ in range(N)]
    comp = [[max(1, round(rnd.uniform(wbar[i] * (1 - beta / 2), wbar[i] * (1 + beta / 2)))) for _ in range(p)]
            for i in range(N)]
    def edge(u, v):
        if comm[u][v] == -1:
            comm[u][v] = round(rnd.uniform(0, 2 * wdag * ccr))
    for t in ids[0]: edge(0, t)
    for L in range(1, len(ids)):
        for t in ids[L]:
            cands = [u for LL in range(max(0, L - 2), L) for u in ids[LL]]
            k = max(1, round(density * len(ids[L - 1])))
            for u in rnd.sample(cands, min(k, len(cands))): edge(u, t)
    for i in range(1, N - 1):
        if all(c == -1 for c in comm[i]): edge(i, N - 1)
        if all(comm[j][i] == -1 for j in range(N)): edge(0, i)
    return comp, comm

def write(path, comp, comm, fmt, rnd):
    n, p = len(comp), len(comp[0])
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        f.write(f"{n} {p}\n")
        if fmt == "D1":
            f.write(" ".join(f"{rnd.uniform(0.1, 0.4):.4f}" for _ in range(p)) + "\n")
        for lo, hi in ((0.03, 0.07), (0.8, 1.2), (2.5, 3.0), (0.02, 0.99)):
            f.write(" ".join(f"{rnd.uniform(lo, hi):.4f}" for _ in range(p)) + "\n")
        f.write(" ".join("1.0" for _ in range(p)) + "\n")
        for r in comp: f.write(" ".join(map(str, r)) + "\n")
        for r in comm: f.write(" ".join(map(str, r)) + "\n")

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", required=True)
    ap.add_argument("--nodes", default="50,100")
    ap.add_argument("--procs", default="2,4,8")
    ap.add_argument("--ccr", default="1")
    ap.add_argument("--beta", default="0.5")
    ap.add_argument("--fat", type=float, default=0.5)
    ap.add_argument("--density", type=float, default=0.5)
    ap.add_argument("--count", type=int, default=5)
    ap.add_argument("--format", default="D2", choices=["D1", "D2"])
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--prefix", default="R-", help="family name prefix")
    a = ap.parse_args()
    fl = lambda s: [float(x) for x in s.split(",")]
    k = 0
    for ccr in fl(a.ccr):
        for beta in fl(a.beta):
            fam = f"{a.prefix}ccr{ccr:g}-b{beta:g}"
            for n in map(int, a.nodes.split(",")):
                for i in range(1, a.count + 1):
                    for p in map(int, a.procs.split(",")):
                        rnd = random.Random(f"{a.seed}-{fam}-{n}-{i}-{p}")
                        comp, comm = random_dag(n, p, ccr, beta, a.fat, a.density, rnd)
                        write(os.path.join(a.out, fam, f"{n}Nodes", f"{p}PROCESSORS", f"{i}_{p}.txt"),
                              comp, comm, a.format, rnd)
                        k += 1
    print(f"wrote {k} instances to {a.out}")

if __name__ == "__main__":
    main()
