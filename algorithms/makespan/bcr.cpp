// BCR v1: bounded bottleneck-closure repair. Experimental, not a novelty claim.
// Seeds reproduce main@3741b6d SCCT and DSLA; no baseline source is modified.
// Fixed topological task order; all processor-order arcs respect that order.
// A trial jointly reassigns a small group to one processor, propagates all
// affected precedence/resource timings exactly, and is accepted only if the
// pair (makespan, sum of task finish times) decreases lexicographically.
// Two phases: single-task repair, then single + critical-parent/chain closures.
// Arguments: scct|dsla, seed|single|bundle, strict, verify, budget=N, rounds=N.
// Default: dsla bundle, 4096 exact trial evaluations, 8 rounds per phase.
// These are engineering budgets fixed before development results, not weights.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = numeric_limits<ll>::max()/8;
struct Edge { int v; ll c; };
struct Schedule {
    vector<int> order, pr, prev, next;
    vector<ll> st, ft;
    ll mk=0, total=0;
};
struct Graph {
    int n=0,p=0;
    vector<vector<ll>> w;
    vector<vector<Edge>> pred,succ;
    vector<int> topo;
    void read() {
        if (!(cin>>n>>p) || n<=0 || p<=0) throw runtime_error("invalid dimensions");
        w.assign(n,vector<ll>(p)); pred.resize(n); succ.resize(n);
        ll biggest=0;
        for(auto &r:w) for(auto &x:r) {
            if(!(cin>>x)||x<0) throw runtime_error("invalid computation cost");
            biggest=max(biggest,x);
        }
        for(int i=0;i<n;i++) for(int j=0;j<n;j++) {
            ll c; if(!(cin>>c)||c< -1) throw runtime_error("invalid edge cost");
            if(c>=0) { succ[i].push_back({j,c}); pred[j].push_back({i,c}); biggest=max(biggest,c); }
        }
        if(biggest > INF / max(1LL,8LL*n*n)) throw runtime_error("cost range too large");
        vector<int> d(n); deque<int> q;
        for(int t=0;t<n;t++) { d[t]=pred[t].size(); if(!d[t])q.push_back(t); }
        while(!q.empty()) { int t=q.front();q.pop_front();topo.push_back(t);
            for(auto e:succ[t]) if(--d[e.v]==0)q.push_back(e.v); }
        if((int)topo.size()!=n) throw runtime_error("cyclic input");
    }
    void finish(Schedule &s) const {
        s.mk=*max_element(s.ft.begin(),s.ft.end());
        s.total=accumulate(s.ft.begin(),s.ft.end(),0LL);
        s.prev.assign(n,-1); s.next.assign(n,-1); vector<int> last(p,-1);
        for(int t:s.order) { int k=s.pr[t];s.prev[t]=last[k];
            if(last[k]>=0) s.next[last[k]]=t;
            last[k]=t; }
    }
    Schedule replay(const vector<int>& order,const vector<int>& pr) const {
        Schedule s;s.order=order;s.pr=pr;s.st.assign(n,0);s.ft.assign(n,0);
        vector<ll> avail(p,0);
        for(int t:order) { int k=pr[t];ll st=avail[k];
            for(auto e:pred[t])st=max(st,s.ft[e.v]+(pr[e.v]==k?0:e.c));
            s.st[t]=st;s.ft[t]=st+w[t][k];avail[k]=s.ft[t]; }
        finish(s);return s;
    }
    Schedule scct() const {
        vector<vector<ll>> T(n,vector<ll>(p));
        vector<vector<double>> G(n,vector<double>(p));
        vector<double> rank(n,0);vector<ll> busy(p);
        for(int ii=n-1;ii>=0;ii--) { int t=topo[ii];
            for(int k=0;k<p;k++) { vector<pair<ll,int>> order;
                for(int ci=0;ci<(int)succ[t].size();ci++) { auto e=succ[t][ci];ll b=INF;
                    for(int q=0;q<p;q++)b=min(b,T[e.v][q]+(q==k?0:e.c));
                    order.push_back({-b,ci}); }
                sort(order.begin(),order.end());fill(busy.begin(),busy.end(),0);ll worst=0;
                for(auto [nb,ci]:order) { auto e=succ[t][ci];ll best=INF;int bq=0;
                    for(int q=0;q<p;q++) {ll f=max(busy[q],q==k?0LL:e.c)+T[e.v][q];
                        if(f<best){best=f;bq=q;} }
                    busy[bq]=max(busy[bq],bq==k?0LL:e.c)+w[e.v][bq];worst=max(worst,best); }
                T[t][k]=w[t][k]+worst;
                double tail=0;
                for(auto e:succ[t]) {double b=1e300;
                    for(int q=0;q<p;q++)b=min(b,G[e.v][q]+2.0*w[e.v][q]+(q==k?0.0:(double)e.c));
                    tail=max(tail,b); }
                G[t][k]=w[t][k]+tail;
            }
            rank[t]=accumulate(G[t].begin(),G[t].end(),0.0)/p;
        }
        vector<int> prio(n),pos(n),d(n);iota(prio.begin(),prio.end(),0);
        stable_sort(prio.begin(),prio.end(),[&](int a,int b){return rank[a]>rank[b];});
        for(int i=0;i<n;i++)pos[prio[i]]=i;
        priority_queue<int,vector<int>,greater<int>> ready;
        for(int t=0;t<n;t++){d[t]=pred[t].size();if(!d[t])ready.push(pos[t]);}
        Schedule s;s.pr.assign(n,-1);s.st.assign(n,0);s.ft.assign(n,0);vector<ll> avail(p,0);
        while(!ready.empty()){int t=prio[ready.top()];ready.pop();s.order.push_back(t);
            ll best=INF;int bk=0;ll bst=0,bft=0;
            for(int k=0;k<p;k++){ll st=avail[k];
                for(auto e:pred[t])st=max(st,s.ft[e.v]+(s.pr[e.v]==k?0:e.c));
                ll ft=st+w[t][k],score=ft+T[t][k];
                if(score<best){best=score;bk=k;bst=st;bft=ft;}}
            s.pr[t]=bk;s.st[t]=bst;s.ft[t]=bft;avail[bk]=bft;
            for(auto e:succ[t])if(--d[e.v]==0)ready.push(pos[e.v]);
        }
        finish(s);return s;
    }
    Schedule dsla() const {
        vector<vector<ll>> ioct(n,vector<ll>(p));vector<ll> mn(n);
        vector<double> rank(n,0),mio(n,0);
        for(int ii=n-1;ii>=0;ii--){int t=topo[ii];
            for(int k=0;k<p;k++){ll mx=0;for(auto e:succ[t])mx=max(mx,min(ioct[e.v][k],mn[e.v]+e.c));
                ioct[t][k]=mx+w[t][k];mio[t]+=ioct[t][k];}
            mn[t]=*min_element(ioct[t].begin(),ioct[t].end());
            double wb=0;for(int k=0;k<p;k++)wb+=w[t][k];wb/=p;
            double mx=0;for(auto e:succ[t])mx=max(mx,e.c+rank[e.v]);rank[t]=wb+mx;
        }
        vector<int> prio(n),pos(n);iota(prio.begin(),prio.end(),0);
        stable_sort(prio.begin(),prio.end(),[&](int a,int b){return rank[a]>rank[b];});
        for(int i=0;i<n;i++)pos[prio[i]]=i;
        vector<vector<Edge>> crit=succ;
        for(auto &ch:crit){stable_sort(ch.begin(),ch.end(),[&](Edge a,Edge b){return mio[a.v]>mio[b.v];});if(ch.size()>3)ch.resize(3);}
        auto build=[&](bool eftPolicy,double lam){
            Schedule s;s.pr.assign(n,-1);s.st.assign(n,0);s.ft.assign(n,0);
            vector<ll> avail(p,0),eft(p);vector<vector<ll>> drt(n,vector<ll>(p,0));
            vector<int>d(n),ready;vector<double> score(p);
            for(int t=0;t<n;t++){d[t]=pred[t].size();if(!d[t])ready.push_back(t);}
            while(!ready.empty()){int bi=0;
                if(!eftPolicy){for(int i=1;i<(int)ready.size();i++)if(pos[ready[i]]<pos[ready[bi]])bi=i;}
                else{ll be=INF;for(int i=0;i<(int)ready.size();i++){int t=ready[i];ll m=INF;
                    for(int k=0;k<p;k++)m=min(m,max(avail[k],drt[t][k])+w[t][k]);
                    if(m<be||(m==be&&pos[t]<pos[ready[bi]])){be=m;bi=i;}}}
                int t=ready[bi];ready.erase(ready.begin()+bi);s.order.push_back(t);
                for(int k=0;k<p;k++)eft[k]=max(avail[k],drt[t][k])+w[t][k];
                int best=0;
                if(succ[t].empty()){for(int k=1;k<p;k++)if(eft[k]<eft[best])best=k;}
                else{for(int k=0;k<p;k++){ll lk=0;
                    for(auto e:crit[t]){ll L=INF;for(int q=0;q<p;q++){
                        ll av=q==k?eft[k]:avail[q];ll start=max(max(av,eft[k]+(q==k?0:e.c)),drt[e.v][q]);
                        L=min(L,start+ioct[e.v][q]);}lk=max(lk,L);}
                    score[k]=(1-lam)*eft[k]+lam*(double)(lk+w[t][k]);}
                    for(int k=1;k<p;k++)if(score[k]<score[best]-1e-9||(fabs(score[k]-score[best])<=1e-9&&eft[k]<eft[best]))best=k;}
                s.pr[t]=best;s.ft[t]=eft[best];s.st[t]=s.ft[t]-w[t][best];avail[best]=s.ft[t];
                for(auto e:succ[t]){for(int q=0;q<p;q++)drt[e.v][q]=max(drt[e.v][q],s.ft[t]+(q==best?0:e.c));
                    if(--d[e.v]==0)ready.push_back(e.v);}
            }
            finish(s);return s;
        };
        Schedule a=build(false,0.5),b=build(true,0.1);return b.mk<a.mk?b:a;
    }
};
struct Repair {
    const Graph &g;int budget=4096,rounds=8;bool bundles=true,strict=false,verify=false;
    int trials=0,accepted=0,bundleAccepted=0;ll propagated=0;
    Repair(const Graph& gg):g(gg){}
    bool better(const Schedule&a,const Schedule&b)const{
        return a.mk<b.mk||(!strict&&a.mk==b.mk&&a.total<b.total);
    }
    // Exact causal propagation. An O(n) scan creates changed resource arcs;
    // only affected nodes require predecessor scans. Worst case is O(n+e+n log n).
    bool trial(const Schedule&s,const vector<int>&group,int q,Schedule&best) {
        vector<int> pr=s.pr;bool moved=false;
        for(int t:group){moved|=pr[t]!=q;pr[t]=q;}if(!moved)return false;
        trials++;int n=g.n;
        vector<int> prev(n,-1),next(n,-1),last(g.p,-1),pos(n);
        for(int i=0;i<n;i++)pos[s.order[i]]=i;
        for(int t:s.order){prev[t]=last[pr[t]];if(prev[t]>=0)next[prev[t]]=t;last[pr[t]]=t;}
        priority_queue<int,vector<int>,greater<int>> todo;vector<char>marked(n,0);
        auto push=[&](int t){if(t>=0&&!marked[t]){marked[t]=1;todo.push(pos[t]);}};
        for(int t=0;t<n;t++)if(prev[t]!=s.prev[t])push(t);
        for(int t:group)if(pr[t]!=s.pr[t]){push(t);for(auto e:g.succ[t])push(e.v);}
        vector<ll> ft=s.ft;bool reject=false;
        while(!todo.empty()){int t=s.order[todo.top()];todo.pop();propagated++;
            ll st=prev[t]<0?0:ft[prev[t]];
            for(auto e:g.pred[t])st=max(st,ft[e.v]+(pr[e.v]==pr[t]?0:e.c));
            ll f=st+g.w[t][pr[t]];
            if(f>best.mk){reject=true;break;}
            if(f!=ft[t]){ft[t]=f;for(auto e:g.succ[t])push(e.v);push(next[t]);}
        }
        if(verify){Schedule ref=g.replay(s.order,pr);
            if(reject){if(ref.mk<=best.mk)throw runtime_error("incorrect early rejection");}
            else if(ref.ft!=ft)throw runtime_error("incremental replay mismatch");}
        if(reject)return false;
        Schedule cand;cand.order=s.order;cand.pr=move(pr);cand.ft=move(ft);
        cand.mk=*max_element(cand.ft.begin(),cand.ft.end());cand.total=accumulate(cand.ft.begin(),cand.ft.end(),0LL);
        if(!better(cand,best))return false;
        cand.prev=move(prev);cand.next=move(next);cand.st.resize(n);
        for(int t=0;t<n;t++)cand.st[t]=cand.ft[t]-g.w[t][cand.pr[t]];
        best=move(cand);return true;
    }
    Schedule run(Schedule s){
        const int n=g.n,p=g.p;vector<int>pos(n);for(int i=0;i<n;i++)pos[s.order[i]]=i;
        const int half=budget/2;
        for(int phase=0;phase<2;phase++){
            int stop=phase==0?half:budget;
            for(int round=0;round<rounds&&trials<stop;round++){
                vector<ll>tail(n,0),priority(n,0);vector<int>focus;
                for(int ii=n-1;ii>=0;ii--){int t=s.order[ii];
                    if(s.next[t]>=0){int v=s.next[t];tail[t]=g.w[v][s.pr[v]]+tail[v];}
                    for(auto e:g.succ[t])tail[t]=max(tail[t],(s.pr[t]==s.pr[e.v]?0:e.c)+g.w[e.v][s.pr[e.v]]+tail[e.v]);
                    if(s.ft[t]+tail[t]==s.mk){
                        ll score=g.w[t][s.pr[t]]-*min_element(g.w[t].begin(),g.w[t].end());
                        for(auto e:g.pred[t])if(s.pr[e.v]!=s.pr[t])score=max(score,e.c);
                        for(auto e:g.succ[t])if(s.pr[e.v]!=s.pr[t])score=max(score,e.c);
                        priority[t]=score;focus.push_back(t);
                    }
                }
                stable_sort(focus.begin(),focus.end(),[&](int a,int b){
                    if(priority[a]!=priority[b]) return priority[a]>priority[b];
                    return pos[a]<pos[b];});
                if(focus.size()>24)focus.resize(24);
                vector<vector<ll>>prefix(n,vector<ll>(p));vector<ll>avail(p,0);
                for(int t:s.order){prefix[t]=avail;avail[s.pr[t]]=s.ft[t];}
                vector<vector<int>>groups;set<vector<int>>seen;
                auto add=[&](vector<int>gr){sort(gr.begin(),gr.end(),[&](int a,int b){return pos[a]<pos[b];});
                    gr.erase(unique(gr.begin(),gr.end()),gr.end());if(seen.insert(gr).second)groups.push_back(move(gr));};
                for(int t:focus){add({t});
                    if(phase==1&&bundles){
                        vector<Edge>parents=g.pred[t];stable_sort(parents.begin(),parents.end(),[&](Edge a,Edge b){
                            ll aa=s.ft[a.v]+(s.pr[a.v]==s.pr[t]?0:a.c),bb=s.ft[b.v]+(s.pr[b.v]==s.pr[t]?0:b.c);
                            return aa!=bb?aa>bb:pos[a.v]<pos[b.v];});
                        vector<int>gr{t};for(int i=0;i<min(3,(int)parents.size());i++){gr.push_back(parents[i].v);add(gr);}
                        gr={t};int u=t;
                        for(int depth=0;depth<3&&!g.pred[u].empty();depth++){
                            int par=-1;ll arrival=-1;
                            for(auto e:g.pred[u]){ll a=s.ft[e.v]+(s.pr[e.v]==s.pr[u]?0:e.c);
                                if(a>arrival||(a==arrival&&(par<0||pos[e.v]<pos[par]))){arrival=a;par=e.v;}}
                            gr.push_back(par);add(gr);u=par;
                        }
                    }
                }
                Schedule best=s;int bestGroup=0;
                for(const auto&gr:groups){if(trials>=stop)break;
                    vector<pair<ll,int>>scores;vector<int>in(n,-1);for(int i=0;i<(int)gr.size();i++)in[gr[i]]=i;
                    for(int q=0;q<p;q++){
                        ll busy=prefix[gr[0]][q],val=0;vector<ll>gf(gr.size(),0);
                        for(int i=0;i<(int)gr.size();i++){int t=gr[i];ll st=busy;
                            for(auto e:g.pred[t])st=max(st,in[e.v]>=0?gf[in[e.v]]:s.ft[e.v]+(s.pr[e.v]==q?0:e.c));
                            gf[i]=st+g.w[t][q];busy=gf[i];val=max(val,busy+tail[t]);}
                        scores.push_back({val,q});
                    }
                    sort(scores.begin(),scores.end());vector<int>targets;
                    for(int i=0;i<min(4,p);i++)targets.push_back(scores[i].second);
                    for(int t:gr)if(find(targets.begin(),targets.end(),s.pr[t])==targets.end())targets.push_back(s.pr[t]);
                    for(int q:targets){if(trials>=stop)break;if(trial(s,gr,q,best))bestGroup=gr.size();}
                }
                if(!better(best,s))break;
                if(best.mk>s.mk)throw runtime_error("nonmonotone repair");
                s=move(best);accepted++;bundleAccepted+=(bestGroup>1);
                if(verify){Schedule full=g.replay(s.order,s.pr);if(full.ft!=s.ft)throw runtime_error("committed replay mismatch");}
            }
        }
        return s;
    }
};
#ifndef BCR_TEST
int main(int argc,char**argv){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    try{
        Graph g;g.read();string seed="dsla",mode="bundle";Repair r(g);
        for(int i=1;i<argc;i++){string a=argv[i];
            if(a=="scct"||a=="dsla")seed=a;
            else if(a=="seed"||a=="single"||a=="bundle")mode=a;
            else if(a=="strict")r.strict=true;else if(a=="verify")r.verify=true;
            else if(a.rfind("budget=",0)==0)r.budget=stoi(a.substr(7));
            else if(a.rfind("rounds=",0)==0)r.rounds=stoi(a.substr(7));
            else throw runtime_error("unknown argument: "+a);
        }
        if(r.budget<0||r.budget>100000||r.rounds<0||r.rounds>1000)throw runtime_error("invalid budget");
        Schedule s=seed=="scct"?g.scct():g.dsla();ll original=s.mk;
        if(mode!="seed"){r.bundles=mode=="bundle";s=r.run(move(s));}
        if(s.mk>original)throw runtime_error("seed guarantee violated");
        cout<<"TASK ST FT PRO\n";for(int t=0;t<g.n;t++)cout<<t+1<<' '<<s.st[t]<<' '<<s.ft[t]<<' '<<s.pr[t]+1<<'\n';
        cerr<<"seed="<<seed<<" initial="<<original<<" final="<<s.mk<<" trials="<<r.trials
            <<" accepted="<<r.accepted<<" bundle_accepted="<<r.bundleAccepted<<" propagated="<<r.propagated<<'\n';
    }catch(const exception&e){cerr<<e.what()<<'\n';return 2;}
}
#endif
