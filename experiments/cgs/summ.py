import csv,sys
from collections import defaultdict
ref=sys.argv[1]; files=sys.argv[2:]
mk=defaultdict(dict)
for f in files:
    for r in csv.DictReader(open(f)): mk[r["algo"]][r["path"]]=float(r["makespan"])
g=lambda a,ps:sum(100*(mk[ref][p]-mk[a][p])/mk[ref][p] for p in ps)/len(ps)
ps=list(mk[ref]); grp={"ALL":ps,"workflow":[p for p in ps if not p.startswith("R-")],"ccr0.1":[p for p in ps if "ccr0.1-" in p],"ccr1":[p for p in ps if "ccr1-" in p],"ccr10":[p for p in ps if "ccr10-" in p]}
print(f"gain % vs {ref}\n{'algo':14}"+"".join(f"{k:>10}" for k in grp))
for a in sorted(mk,key=lambda a:-g(a,ps)):
    print(f"{a:14}"+"".join(f"{g(a,v):>10.2f}" for v in grp.values()))
