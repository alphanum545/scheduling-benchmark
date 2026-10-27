import csv,subprocess,json,hashlib,tempfile,re,os,sys
from concurrent.futures import ProcessPoolExecutor
R="/home/claude/scheduling-benchmark"; T=os.environ.get("RCIS_BIN","/tmp/rcis_gated")
def split_of(p): return int(hashlib.sha256(p.encode()).hexdigest()[:8],16)%4==0
def one(path):
    with tempfile.TemporaryDirectory() as t:
        d3=f"{t}/x"; subprocess.run([os.environ.get("EVALUATE","build/evaluate"),"strip",f"{R}/data/{path}",d3],capture_output=True)
        out={}
        for tag,args in [("all",[]),("g2par",["--gate=2","--commit=1"])]:
            e=subprocess.run([T,*args],stdin=open(d3),capture_output=True,text=True).stderr
            out[tag]=dict(re.findall(r"(\w+)=(\d+)",e))
        return path,out
idx=[r["path"] for r in csv.DictReader(open(f"{R}/data_index.csv")) if split_of(r["path"])]
with ProcessPoolExecutor(2) as ex: res=list(ex.map(one,idx))
json.dump(res,open("trace.json","w"))
