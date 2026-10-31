"""Independent Python port of dsla.cpp used to verify the constructed examples and print score traces."""
import math
def read_d3(txt):
    t=list(map(int,txt.split())); n,p=t[0],t[1]; i=2
    W=[t[i+r*p:i+(r+1)*p] for r in range(n)]; i+=n*p
    C=[t[i+r*n:i+(r+1)*n] for r in range(n)]
    return n,p,W,C
def dsla(n,p,W,C,R=3,lr=0.5,le=0.1,state=True,own=1.0,mode=2,trace=None):
    succ=[[(j,C[i][j]) for j in range(n) if C[i][j]!=-1] for i in range(n)]
    pred=[[(i,C[i][j]) for i in range(n) if C[i][j]!=-1] for j in range(n)]
    indeg=[len(x) for x in pred]; topo=[]; q=[i for i in range(n) if not indeg[i]]
    while q:
        t=q.pop(0); topo.append(t)
        for s,_ in succ[t]:
            indeg[s]-=1
            if not indeg[s]: q.append(s)
    I=[[0]*p for _ in range(n)]; mn=[0]*n
    for t in reversed(topo):
        for k in range(p):
            mx=0
            for c,cc in succ[t]: mx=max(mx,min(I[c][k],mn[c]+cc))
            I[t][k]=mx+W[t][k]
        mn[t]=min(I[t])
    rnk=[0.0]*n
    for t in reversed(topo):
        rnk[t]=sum(W[t])/p+max([cc+rnk[c] for c,cc in succ[t]],default=0)
    prio=sorted(range(n),key=lambda a:-rnk[a]); pos=[0]*n
    for i,t in enumerate(prio): pos[t]=i
    mio=[sum(I[t]) for t in range(n)]
    crit=[sorted(succ[t],key=lambda x:-mio[x[0]])[:R] for t in range(n)]
    def build(eftp,lam):
        av=[0]*p; drt=[[0]*p for _ in range(n)]; deg=[len(x) for x in pred]
        ready=[i for i in range(n) if not deg[i]]; FT=[0]*n; PR=[-1]*n; ST=[0]*n
        while ready:
            if not eftp: bi=min(range(len(ready)),key=lambda i:pos[ready[i]])
            else:
                bi=0;be=math.inf
                for i,t in enumerate(ready):
                    m=min(max(av[k],drt[t][k])+W[t][k] for k in range(p))
                    if m<be or (m==be and pos[t]<pos[ready[bi]]): be=m;bi=i
            t=ready.pop(bi)
            eft=[max(av[k],drt[t][k])+W[t][k] for k in range(p)]
            if not succ[t] or lam==0:
                best=min(range(p),key=lambda k:(eft[k],k)); sc=None
            else:
                sc=[]
                for k in range(p):
                    lk=0
                    for c,cc in crit[t]:
                        L=math.inf
                        for qq in range(p):
                            if state:
                                a=eft[k] if qq==k else av[qq]
                                st=max(a,eft[k]+(0 if qq==k else cc),drt[c][qq])
                            else: st=eft[k]+(0 if qq==k else cc)
                            L=min(L,st+I[c][qq])
                        lk=max(lk,L)
                    sc.append((1-lam)*eft[k]+lam*(lk+own*W[t][k]))
                best=0
                for k in range(1,p):
                    if sc[k]<sc[best]-1e-9 or (abs(sc[k]-sc[best])<=1e-9 and eft[k]<eft[best]): best=k
            if trace is not None: trace.append(dict(task=t,avail=list(av),eft=eft,score=sc,choice=best))
            PR[t]=best; FT[t]=eft[best]; ST[t]=FT[t]-W[t][best]; av[best]=FT[t]
            for c,cc in succ[t]:
                for qq in range(p): drt[c][qq]=max(drt[c][qq],FT[t]+(0 if qq==best else cc))
                deg[c]-=1
                if not deg[c]: ready.append(c)
        return max(FT) if n else 0,PR,ST,FT
    if mode==1: return build(True,le)
    S=build(False,lr)
    if mode==2:
        B=build(True,le)
        if B[0]<S[0]: S=B
    return S
