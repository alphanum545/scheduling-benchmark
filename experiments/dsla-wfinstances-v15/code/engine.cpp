// Controlled DSLA mechanism engine. Original method is unchanged in its upstream repository.
// Outputs full schedules for independent validation. Internal timers exclude input/output.
#include <algorithm>
#include <chrono>
#include <climits>
#include <tuple>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace std;
using ll = long long;
using Clock = chrono::steady_clock;
struct Graph { int n,p; vector<vector<ll>> w; vector<vector<pair<int,ll>>> su,pr; vector<int> topo,pos,octpos; vector<vector<ll>> io,oct; vector<vector<pair<int,ll>>> crit; double prep=0,topo_us=0,io_us=0,oct_us=0,rank_us=0,kids_us=0; };
struct Config { string id; bool eft=false,A=true,D=true,offset=true,own=true; int weight=5,R=3; bool index=false,insert=false,peft=false; };
struct Schedule { vector<int> order,proc; vector<ll> st,ft; ll mk=0; double build_us=0,total_us=0; long long child_evals=0; };
static double us(Clock::time_point t){return chrono::duration<double,micro>(Clock::now()-t).count();}
static ll gap(const vector<pair<ll,ll>>& slots,ll ready,ll dur){if(!dur)return ready; ll t=ready;for(auto [s,f]:slots){if(t+dur<=s)break;if(t<f&&t+dur>s)t=f;}return t;}
Graph read_graph(){
 Graph g; if(!(cin>>g.n>>g.p)||g.n<1||g.p<1||g.n>10000||g.p>4096)throw runtime_error("dimensions");
 g.w.assign(g.n,vector<ll>(g.p));g.su.resize(g.n);g.pr.resize(g.n);
 for(auto &r:g.w)for(auto &x:r)if(!(cin>>x)||x<0||x>2147483647LL)throw runtime_error("cost");
 for(int i=0;i<g.n;i++)for(int j=0;j<g.n;j++){ll c;if(!(cin>>c)||c< -1||c>2147483647LL)throw runtime_error("edge");if(c>=0){g.su[i].push_back({j,c});g.pr[j].push_back({i,c});}}
 string extra;if(cin>>extra)throw runtime_error("trailing input");
 auto start=Clock::now();vector<int> d(g.n);for(int i=0;i<g.n;i++){d[i]=g.pr[i].size();if(!d[i])g.topo.push_back(i);}for(size_t i=0;i<g.topo.size();i++)for(auto [s,c]:g.su[g.topo[i]])if(!--d[s])g.topo.push_back(s);if((int)g.topo.size()!=g.n)throw runtime_error("cycle");
 g.topo_us=us(start);auto tick=Clock::now();
 g.io.assign(g.n,vector<ll>(g.p));vector<ll> mn(g.n);vector<double> meanio(g.n);
 for(int z=g.n-1;z>=0;z--){int t=g.topo[z];for(int k=0;k<g.p;k++){ll mx=0;for(auto [c,cc]:g.su[t])mx=max(mx,min(g.io[c][k],mn[c]+cc));g.io[t][k]=mx+g.w[t][k];}mn[t]=*min_element(g.io[t].begin(),g.io[t].end());}g.io_us=us(tick);tick=Clock::now();
 g.oct.assign(g.n,vector<ll>(g.p));vector<ll> mno(g.n);vector<double> meano(g.n);
 for(int z=g.n-1;z>=0;z--){int t=g.topo[z];mno[t]=LLONG_MAX;for(int k=0;k<g.p;k++){ll mx=0;for(auto [c,cc]:g.su[t])mx=max(mx,min(g.oct[c][k]+g.w[c][k],mno[c]+cc));g.oct[t][k]=mx;mno[t]=min(mno[t],mx+g.w[t][k]);meano[t]+=mx;}}
 vector<int> ord(g.n);iota(ord.begin(),ord.end(),0);stable_sort(ord.begin(),ord.end(),[&](int a,int b){return meano[a]>meano[b];});g.octpos.resize(g.n);for(int i=0;i<g.n;i++)g.octpos[ord[i]]=i;g.oct_us=us(tick);tick=Clock::now();
 vector<double> rank(g.n);for(int z=g.n-1;z>=0;z--){int t=g.topo[z];double avg=0,mx=0;for(int k=0;k<g.p;k++)avg+=g.w[t][k];for(auto [c,cc]:g.su[t])mx=max(mx,cc+rank[c]);rank[t]=avg/g.p+mx;}
 iota(ord.begin(),ord.end(),0);stable_sort(ord.begin(),ord.end(),[&](int a,int b){return rank[a]>rank[b];});g.pos.resize(g.n);for(int i=0;i<g.n;i++)g.pos[ord[i]]=i;g.rank_us=us(tick);tick=Clock::now();
 for(int t=0;t<g.n;t++)for(int k=0;k<g.p;k++)meanio[t]+=g.io[t][k];g.crit=g.su;for(auto &ch:g.crit)stable_sort(ch.begin(),ch.end(),[&](auto a,auto b){return meanio[a.first]>meanio[b.first];});g.kids_us=us(tick);g.prep=g.topo_us+g.io_us+g.rank_us+g.kids_us;
 return g;
}
Schedule build(const Graph& g,const Config& cfg){
 auto begin=Clock::now();int n=g.n,p=g.p;Schedule s;s.st.assign(n,0);s.ft.assign(n,0);s.proc.assign(n,-1);vector<ll> avail(p),eft(p),est(p);vector<vector<ll>> drt(n,vector<ll>(p));vector<vector<pair<ll,ll>>> slots(p);vector<int> deg(n),ready;for(int t=0;t<n;t++){deg[t]=g.pr[t].size();if(!deg[t])ready.push_back(t);}const auto &pos=cfg.peft?g.octpos:g.pos;double lam=cfg.weight/10.0;vector<double> score(p);
 while(!ready.empty()){
  int bi=0;ll be=LLONG_MAX;for(int i=0;i<(int)ready.size();i++){int t=ready[i];if(cfg.eft){ll m=LLONG_MAX;for(int k=0;k<p;k++)m=min(m,max(avail[k],drt[t][k])+g.w[t][k]);if(m<be||(m==be&&pos[t]<pos[ready[bi]])){be=m;bi=i;}}else if(pos[t]<pos[ready[bi]])bi=i;}
  int t=ready[bi];ready.erase(ready.begin()+bi);s.order.push_back(t);
  for(int k=0;k<p;k++){est[k]=cfg.insert?gap(slots[k],drt[t][k],g.w[t][k]):max(avail[k],drt[t][k]);eft[k]=est[k]+g.w[t][k];}
  int best=0;if(cfg.peft){for(int k=0;k<p;k++)score[k]=(double)(eft[k]+g.oct[t][k]);for(int k=1;k<p;k++)if(score[k]<score[best])best=k;}
  else if(g.su[t].empty()||cfg.weight==0){for(int k=1;k<p;k++)if(eft[k]<eft[best])best=k;}
  else {const auto &children=cfg.index?g.su[t]:g.crit[t];int count=cfg.R<0?(int)children.size():min(cfg.R,(int)children.size());
   for(int k=0;k<p;k++){ll lk=0;for(int j=0;j<count;j++){auto [c,cc]=children[j];ll L=LLONG_MAX;for(int q=0;q<p;q++){ll av=cfg.A?((cfg.own&&q==k)?eft[k]:avail[q]):0;ll dt=cfg.D?drt[c][q]:0;ll st=max(max(av,eft[k]+(q==k?0:cc)),dt);L=min(L,st+g.io[c][q]);}lk=max(lk,L);s.child_evals++;}score[k]=(1-lam)*eft[k]+lam*(double)(lk+(cfg.offset?g.w[t][k]:0));}
   for(int k=1;k<p;k++)if(score[k]<score[best]-1e-9||(fabs(score[k]-score[best])<=1e-9&&eft[k]<eft[best]))best=k;
  }
  s.proc[t]=best;s.st[t]=est[best];s.ft[t]=eft[best];avail[best]=max(avail[best],eft[best]);if(cfg.insert&&s.ft[t]>s.st[t]){auto v=make_pair(s.st[t],s.ft[t]);slots[best].insert(lower_bound(slots[best].begin(),slots[best].end(),v),v);}for(auto [c,cc]:g.su[t]){for(int q=0;q<p;q++)drt[c][q]=max(drt[c][q],s.ft[t]+(q==best?0:cc));if(!--deg[c])ready.push_back(c);}
 }
 s.mk=*max_element(s.ft.begin(),s.ft.end());s.build_us=us(begin);s.total_us=(cfg.peft?g.topo_us+g.oct_us:(cfg.weight==0?g.topo_us+g.rank_us:g.prep))+s.build_us;return s;
}
bool valid(const Graph& g,const Schedule& s){
 vector<int> seen(g.n),pos(g.n);if((int)s.order.size()!=g.n)return false;for(int i=0;i<g.n;i++){int t=s.order[i];if(t<0||t>=g.n||seen[t]++)return false;pos[t]=i;}
 vector<vector<pair<ll,ll>>> slots(g.p);for(int t=0;t<g.n;t++){int k=s.proc[t];if(k<0||k>=g.p||s.st[t]<0||s.ft[t]-s.st[t]!=g.w[t][k])return false;for(auto [u,c]:g.pr[t])if(pos[u]>=pos[t]||s.st[t]<s.ft[u]+(s.proc[u]==k?0:c))return false;if(s.ft[t]>s.st[t])slots[k].push_back({s.st[t],s.ft[t]});}for(auto &v:slots){sort(v.begin(),v.end());for(size_t i=1;i<v.size();i++)if(v[i].first<v[i-1].second)return false;}return s.mk==*max_element(s.ft.begin(),s.ft.end());
}
template<class T>void arr(const vector<T>& a){cout<<'[';for(size_t i=0;i<a.size();i++){if(i)cout<<',';cout<<a[i];}cout<<']';}
void emit(const string& id,const Schedule&s,bool feasible,const string& chosen=""){
 cout<<"{\"id\":\""<<id<<"\",\"makespan\":"<<s.mk<<",\"valid\":"<<(feasible?"true":"false")<<",\"build_us\":"<<s.build_us<<",\"total_us\":"<<s.total_us<<",\"child_evals\":"<<s.child_evals<<",\"selected\":\""<<chosen<<"\",\"order\":";arr(s.order);cout<<",\"proc\":";arr(s.proc);cout<<",\"start\":";arr(s.st);cout<<",\"finish\":";arr(s.ft);cout<<'}';
}
int main(int argc,char**argv){try{
 if(argc==2&&string(argv[1])=="--self-test"){vector<pair<ll,ll>> v{{0,3},{10,14}};if(gap(v,3,4)!=3||gap(v,8,3)!=14||gap(v,5,0)!=5)return 1;cout<<"gap-tests-pass\n";return 0;}
 ios::sync_with_stdio(false);cin.tie(nullptr);Graph g=read_graph();vector<Config> cfg;for(int A=0;A<2;A++)for(int D=0;D<2;D++)for(int P=0;P<2;P++)for(int W:{1,5}){Config c;c.A=A;c.D=D;c.eft=P;c.weight=W;c.id=string("a")+to_string(A)+"d"+to_string(D)+(P?"_eft":"_rank")+"_w"+to_string(W);cfg.push_back(c);}
 for(int P=0;P<2;P++){Config c;c.eft=P;c.weight=0;c.id=P?"eft_greedy":"rank_greedy";cfg.push_back(c);}
 for(int P=0;P<2;P++){Config c;c.eft=P;c.weight=P?1:5;string name=P?"eft":"rank";c.offset=false;c.id=name+"_no_offset";cfg.push_back(c);c.offset=true;c.own=false;c.id=name+"_own_old";cfg.push_back(c);c.own=true;c.R=1;c.id=name+"_r1";cfg.push_back(c);c.R=-1;c.id=name+"_all_children";cfg.push_back(c);c.R=3;c.index=true;c.id=name+"_index_children";cfg.push_back(c);}
 Config h;h.weight=0;h.insert=true;h.id="heft_insertion_reimplementation";cfg.push_back(h);Config p=h;p.peft=true;p.id="peft_insertion_reimplementation";cfg.push_back(p);
 vector<pair<string,Schedule>> results;for(auto &c:cfg)results.push_back({c.id,build(g,c)});
 auto get=[&](const string& x)->Schedule{for(auto &r:results)if(r.first==x)return r.second;throw runtime_error("unknown config");};
 vector<tuple<string,string,string>> pairs{{"dsla","a1d1_rank_w5","a1d1_eft_w1"},{"portfolio_rank_weights","a1d1_rank_w1","a1d1_rank_w5"},{"portfolio_eft_weights","a1d1_eft_w1","a1d1_eft_w5"},{"portfolio_policy_w1","a1d1_rank_w1","a1d1_eft_w1"},{"portfolio_policy_w5","a1d1_rank_w5","a1d1_eft_w5"},{"portfolio_static","a0d0_rank_w5","a0d0_eft_w1"},{"portfolio_no_offset","rank_no_offset","eft_no_offset"},{"portfolio_all_children","rank_all_children","eft_all_children"},{"portfolio_heft_peft","heft_insertion_reimplementation","peft_insertion_reimplementation"},{"portfolio_r1","rank_r1","eft_r1"},{"portfolio_index_children","rank_index_children","eft_index_children"},{"portfolio_greedy","rank_greedy","eft_greedy"},{"portfolio_availability_only","a1d0_rank_w5","a1d0_eft_w1"},{"portfolio_readiness_only","a0d1_rank_w5","a0d1_eft_w1"}};
 cout.precision(12);cout<<"{\"n\":"<<g.n<<",\"p\":"<<g.p<<",\"shared_prep_us\":"<<g.prep<<",\"runs\":[";bool first=true;for(auto &[id,s]:results){if(!first)cout<<',';first=false;emit(id,s,valid(g,s));}for(auto &[id,a,b]:pairs){auto x=get(a),y=get(b);string chosen=y.mk<x.mk?b:a;auto z=y.mk<x.mk?y:x;z.build_us=x.build_us+y.build_us;z.total_us=(id=="portfolio_heft_peft"?g.topo_us+g.rank_us+g.oct_us:(id=="portfolio_greedy"?g.topo_us+g.rank_us:g.prep))+z.build_us;z.child_evals=x.child_evals+y.child_evals;cout<<',';emit(id,z,valid(g,z),chosen);}cout<<"]}\n";return 0;
 }catch(const exception&e){cerr<<e.what()<<'\n';return 2;}}
