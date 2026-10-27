// RCIS: Ready-Closure Insertion Scheduling (standalone experimental scheduler).
// Independent from DSLA, SCCT, and BCR. Static nonpreemptive heterogeneous DAGs.
// D3 input: n p; n*p computation costs; n*n communication costs (-1: no edge).
// Explicit predecessor-complete ready closures; bounded joint calendar insertion.
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
using Time=long long;
constexpr Time LIMIT=std::numeric_limits<Time>::max()/8;
struct Slot {int task,proc; Time start,finish;};
struct Graph {
    int n=0,p=0;
    std::vector<std::vector<Time>> w,c;
    std::vector<std::vector<int>> pred,succ,topo_extra;
    std::vector<int> topo;
    void validate(){
        if(n<0||p<=0||(int)w.size()!=n||(int)c.size()!=n)throw std::runtime_error("invalid dimensions");
        pred.assign(n,{});succ.assign(n,{});topo.clear();Time largest=0;
        for(int i=0;i<n;i++){
            if((int)w[i].size()!=p||(int)c[i].size()!=n)throw std::runtime_error("invalid matrix shape");
            for(Time t:w[i]){if(t<0)throw std::runtime_error("negative task time");largest=std::max(largest,t);}
            for(int j=0;j<n;j++){Time t=c[i][j];if(t < -1)throw std::runtime_error("negative communication cost");
                if(t>=0){succ[i].push_back(j);pred[j].push_back(i);largest=std::max(largest,t);}}
        }
        if(largest>LIMIT/std::max(1LL,8LL*n))throw std::runtime_error("cost range too large");
        std::vector<int> deg(n);std::queue<int> q;
        for(int i=0;i<n;i++){deg[i]=(int)pred[i].size();if(!deg[i])q.push(i);}
        while(!q.empty()){int u=q.front();q.pop();topo.push_back(u);
            for(int v:succ[u])if(--deg[v]==0)q.push(v);}
        if((int)topo.size()!=n)throw std::runtime_error("cyclic graph");
    }
    static Graph read(std::istream& in){
        Graph g;if(!(in>>g.n>>g.p)||g.n<0||g.p<=0)throw std::runtime_error("invalid dimensions");
        if(g.n>100000||g.p>100000||1LL*g.n*g.n>100000000LL||1LL*g.n*g.p>100000000LL)
            throw std::runtime_error("input exceeds dense matrix safety limit");
        g.w.assign(g.n,std::vector<Time>(g.p));g.c.assign(g.n,std::vector<Time>(g.n));
        for(auto& row:g.w)for(auto& x:row)if(!(in>>x))throw std::runtime_error("truncated computation matrix");
        for(auto& row:g.c)for(auto& x:row)if(!(in>>x))throw std::runtime_error("truncated communication matrix");
        g.validate();return g;
    }
};
struct Options {int parents=2,shortlist=2;bool bundles=true,insertion=true,common_target=true;int commit=0;int gate=0;bool fallback=false;};
// fallback: when the gate rejects a closure, still use it as lookahead for the popped task only
// commit: 0=all closure tasks, 1=parents only (join deferred), 2=popped task only (closure as lookahead)
// gate: 0=none, 1=no outside ready task above lowest closure parent, 2=no outside ready task above the join
struct Result{std::vector<Slot>tasks;std::vector<int>order;Time makespan=0;int bundles=0;long long trials=0,certificates=0,inversions=0,gated=0;};
class RCIS {
    const Graph&g;Options o;Result r;
    std::vector<Time> h,tail;
    std::vector<std::vector<Time>>drt;
    std::vector<std::vector<Slot>>calendar;
    std::vector<int>left,ready_parents;
    std::vector<char>done,ready;
    std::priority_queue<std::pair<Time,int>> pq;
    using Key=std::tuple<Time,Time,Time,Time>;
    static bool earlier(const Slot&a,const Slot&b){
        return std::tie(a.start,a.finish,a.task)<std::tie(b.start,b.finish,b.task);
    }
    void make_ready(int u){ready[u]=true;pq.push({h[u],-u});for(int v:g.succ[u])ready_parents[v]++;}
    Slot slot(int u,int q,const std::vector<Slot>&local)const{
        Time release=drt[u][q],duration=g.w[u][q];
        for(const Slot&s:local)if(g.c[s.task][u]>=0)
            release=std::max(release,s.finish+(s.proc==q?0:g.c[s.task][u]));
        if(duration==0)return {u,q,release,release};
        if(!o.insertion){
            Time st=release;
            for(const Slot&s:calendar[q])st=std::max(st,s.finish);
            for(const Slot&s:local)if(s.proc==q)st=std::max(st,s.finish);
            return {u,q,st,st+duration};
        }
        std::vector<Slot>extra;
        for(const Slot&s:local)if(s.proc==q&&s.finish>s.start)extra.push_back(s);
        std::sort(extra.begin(),extra.end(),earlier);
        size_t i=0,j=0;Time st=release;
        while(i<calendar[q].size()||j<extra.size()){
            Slot busy;
            if(j==extra.size()||(i<calendar[q].size()&&earlier(calendar[q][i],extra[j])))
                busy=calendar[q][i++];
            else busy=extra[j++];
            if(busy.finish<=st)continue;
            if(st+duration<=busy.start)break;
            st=std::max(st,busy.finish);
        }
        return {u,q,st,st+duration};
    }
    Slot best_eft(int u,const std::vector<Slot>&local)const{
        Slot best=slot(u,0,local);
        for(int q=1;q<g.p;q++){Slot s=slot(u,q,local);
            if(std::tie(s.finish,s.proc)<std::tie(best.finish,best.proc))best=s;}
        return best;
    }
    Key key(const std::vector<Slot>&plan,int join)const{
        Time horizon=0,jfinish=0,sum=0,work=0;
        for(const Slot&s:plan){horizon=std::max(horizon,s.finish+tail[s.task]);
            sum+=s.finish;work+=g.w[s.task][s.proc];
            if(s.task==join)jfinish=s.finish;}
        return {horizon,jfinish,sum,work};
    }
    std::vector<Slot> closure(int join,std::vector<int>parents){
        // First compute the deterministic sequential EFT reference.
        std::sort(parents.begin(),parents.end(),[&](int a,int b){return h[a]!=h[b]?h[a]>h[b]:a<b;});
        std::vector<Slot>best;
        for(int u:parents)best.push_back(best_eft(u,best));
        best.push_back(best_eft(join,best));
        const Key reference=key(best,join);Key bestkey=reference;
        std::vector<std::vector<int>>shortlists(g.n);
        for(int u:parents){
            std::vector<std::pair<Time,int>>v;
            for(int q=0;q<g.p;q++)v.push_back({slot(u,q,{}).finish,q});
            std::partial_sort(v.begin(),v.begin()+std::min(o.shortlist,g.p),v.end());
            for(int k=0;k<std::min(o.shortlist,g.p);k++)shortlists[u].push_back(v[k].second);
        }
        std::sort(parents.begin(),parents.end());
        do{
            for(int q=0;q<g.p;q++){
                std::vector<Slot>local;
                std::function<void(size_t)>visit=[&](size_t depth){
                    if(depth==parents.size()){
                        local.push_back(slot(join,q,local));r.trials++;
                        Key k=key(local,join);
                        if(k<bestkey){bestkey=k;best=local;}
                        local.pop_back();return;
                    }
                    int u=parents[depth];auto choices=shortlists[u];
                    if(o.common_target&&std::find(choices.begin(),choices.end(),q)==choices.end())choices.push_back(q);
                    for(int k:choices){local.push_back(slot(u,k,local));visit(depth+1);local.pop_back();}
                };
                visit(0);
            }
        }while(std::next_permutation(parents.begin(),parents.end()));
        if(bestkey>reference)throw std::runtime_error("local reference guarantee failed");
        r.certificates++;return best;
    }
    void commit(const Slot&s){
        int u=s.task;
        if(done[u]||!ready[u]||left[u]!=0)throw std::runtime_error("unsafe commitment");
        for(int v:g.pred[u])
            if(!done[v]||s.start<r.tasks[v].finish+(r.tasks[v].proc==s.proc?0:g.c[v][u]))
                throw std::runtime_error("precedence violation");
        r.tasks[u]=s;r.order.push_back(u);
        r.makespan=std::max(r.makespan,s.finish);
        if(s.finish>s.start){auto&cal=calendar[s.proc];cal.insert(std::lower_bound(cal.begin(),cal.end(),s,earlier),s);}
        done[u]=true;ready[u]=false;
        for(int v:g.succ[u]){
            ready_parents[v]--;left[v]--;
            for(int q=0;q<g.p;q++)
                drt[v][q]=std::max(drt[v][q],s.finish+(s.proc==q?0:g.c[u][v]));
            if(left[v]==0)make_ready(v);
        }
    }
public:
    RCIS(const Graph&gg,Options oo={}):g(gg),o(oo){
        if(o.parents<1||o.parents>3||o.shortlist<1||o.shortlist>3)
            throw std::runtime_error("unsupported search cap");
        r.tasks.assign(g.n,{-1,-1,0,0});
        h.assign(g.n,0);tail.assign(g.n,0);
        drt.assign(g.n,std::vector<Time>(g.p));
        calendar.resize(g.p);left.resize(g.n);
        ready_parents.assign(g.n,0);done.assign(g.n,0);ready.assign(g.n,0);
        for(auto it=g.topo.rbegin();it!=g.topo.rend();++it){
            int u=*it;
            for(int v:g.succ[u])tail[u]=std::max(tail[u],h[v]);
            h[u]=*std::min_element(g.w[u].begin(),g.w[u].end())+tail[u];
        }
        for(int u=0;u<g.n;u++)left[u]=g.pred[u].size();
        for(int u=0;u<g.n;u++)if(!left[u])make_ready(u);
    }
    Result run(){
        while((int)r.order.size()<g.n){
            while(!pq.empty()&&done[-pq.top().second])pq.pop();
            if(pq.empty())throw std::runtime_error("ready queue exhausted");
            int u=-pq.top().second;pq.pop();int join=-1;
            if(o.bundles)
                for(int v:g.succ[u]){
                    if(done[v]||left[v]<1||left[v]>o.parents||ready_parents[v]!=left[v])continue;
                    if(join<0||h[v]>h[join]||(h[v]==h[join]&&v<join))join=v;
                }
            std::vector<int>parents;
            if(join>=0){
                for(int v:g.pred[join])if(!done[v])parents.push_back(v);
                // highest-priority ready task outside the closure
                Time top=-1;
                for(int x=0;x<g.n;x++)if(ready[x]&&!done[x]&&x!=u&&std::find(parents.begin(),parents.end(),x)==parents.end())top=std::max(top,h[x]);
                Time lowpar=LIMIT;for(int v:parents)lowpar=std::min(lowpar,h[v]);
                bool rej=(o.gate==1&&top>lowpar)||(o.gate==2&&top>h[join]);
                if(rej&&o.fallback){r.gated++;r.bundles++;
                    auto plan=closure(join,parents);
                    for(const Slot&s:plan)if(s.task==u){commit(slot(u,s.proc,{}));break;}
                    continue;}
                if(rej){join=-1;r.gated++;}
                else{ // count tasks committed ahead of a higher-priority outside ready task
                    for(int v:parents)if(v!=u&&top>h[v]&&o.commit!=2)r.inversions++;
                    if(top>h[join]&&o.commit==0)r.inversions++;
                }
            }
            if(join<0)commit(best_eft(u,{}));
            else{
                auto plan=closure(join,parents);
                if(o.commit==0){for(const Slot&s:plan)commit(s);}
                else if(o.commit==1){for(const Slot&s:plan)if(s.task!=join)commit(s);}
                else{for(const Slot&s:plan)if(s.task==u){commit(slot(u,s.proc,{}));break;}}
                r.bundles++;
            }
        }
        return r;
    }
};
#ifndef RCIS_TEST
int main(int argc,char**argv){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    try{
        Options o;
        for(int i=1;i<argc;i++){std::string a=argv[i];
            if(a=="--single")o.bundles=false;
            else if(a=="--append")o.insertion=false;
            else if(a=="--no-common-target")o.common_target=false;
            else if(a.rfind("--commit=",0)==0)o.commit=std::stoi(a.substr(9));
            else if(a=="--fallback")o.fallback=true;
            else if(a.rfind("--gate=",0)==0)o.gate=std::stoi(a.substr(7));
            else if(a.rfind("--parents=",0)==0)o.parents=std::stoi(a.substr(10));
            else if(a.rfind("--shortlist=",0)==0)o.shortlist=std::stoi(a.substr(12));
            else throw std::runtime_error("unknown argument: "+a);
        }
        Graph g=Graph::read(std::cin);Result r=RCIS(g,o).run();
        std::cout<<"TASK ST FT PRO\n";
        for(const Slot&s:r.tasks)
            std::cout<<s.task+1<<' '<<s.start<<' '<<s.finish<<' '<<s.proc+1<<'\n';
        std::cerr<<"method=rcis makespan="<<r.makespan<<" closures="<<r.bundles
                 <<" trials="<<r.trials<<" local_certificates="<<r.certificates<<" inversions="<<r.inversions<<" gated="<<r.gated<<'\n';
    }catch(const std::exception&e){std::cerr<<"error: "<<e.what()<<'\n';return 2;}
}
#endif
