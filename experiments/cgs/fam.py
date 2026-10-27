import csv,sys
from collections import defaultdict
cand=sys.argv[1]; refs=sys.argv[2].split(","); files=sys.argv[3:]
mk=defaultdict(dict)
for f in files:
    for r in csv.DictReader(open(f)): mk[r["algo"]][r["path"]]=float(r["makespan"])
fam=lambda p:p.split("/")[0]
fams=sorted({fam(p) for p in mk[cand]})
print(f"{cand} paired gain % vs:\n{'family':15}{'n':>4}"+"".join(f"{r:>10}" for r in refs))
for F in fams+["ALL"]:
    ps=[p for p in mk[cand] if F=="ALL" or fam(p)==F]
    print(f"{F:15}{len(ps):>4}"+"".join(f"{sum(100*(mk[r][p]-mk[cand][p])/mk[r][p] for p in ps)/len(ps):>10.2f}" for r in refs))
