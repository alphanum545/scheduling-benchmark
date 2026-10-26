#define BCR_TEST
#include "../../algorithms/makespan/bcr.cpp"
#include <cassert>
int main(){
    Graph g;g.n=3;g.p=3;g.w={{1,100,2},{100,1,2},{100,100,1}};
    g.succ={{{2,10}},{{2,10}}, {}};g.pred={{},{},{{0,10},{1,10}}};g.topo={0,1,2};
    auto s=g.replay({0,1,2},{0,1,2});assert(s.mk==12);
    Repair r(g);r.verify=true;
    for(int t=0;t<3;t++)for(int q=0;q<3;q++){
        Schedule b=s;bool improved=r.trial(s,{t},q,b);assert(!improved);assert(b.mk==12);
    }
    Schedule b=s;assert(r.trial(s,{0,1,2},2,b));assert(b.mk==5);
    auto bundle=r.run(s);assert(bundle.mk==5);
    cout<<"fan-in barrier: initial=12 single_best=12 joint=5\n";
    mt19937 rng(20261026);int checked=0,graphs=0,seeds=0;
    for(int rep=0;rep<100;rep++){
        Graph a;a.n=2+rng()%23;a.p=1+rng()%6;a.w.assign(a.n,vector<ll>(a.p));
        a.succ.resize(a.n);a.pred.resize(a.n);a.topo.resize(a.n);iota(a.topo.begin(),a.topo.end(),0);shuffle(a.topo.begin(),a.topo.end(),rng);
        for(auto& row:a.w)for(auto &v:row)v=rng()%51;
        for(int ii=0;ii<a.n;ii++)for(int jj=ii+1;jj<a.n;jj++)if(rng()%5==0){
            int u=a.topo[ii],v=a.topo[jj];ll c=rng()%151;a.succ[u].push_back({v,c});a.pred[v].push_back({u,c});
        }
        vector<int>pr(a.n);for(auto &q:pr)q=rng()%a.p;
        auto cur=a.replay(a.topo,pr);Repair cr(a);cr.verify=true;cr.budget=256;cr.rounds=3;
        for(int j=0;j<30;j++){
            vector<int>gr;for(int k=0;k<1+(int)(rng()%4);k++)gr.push_back(rng()%a.n);
            sort(gr.begin(),gr.end());gr.erase(unique(gr.begin(),gr.end()),gr.end());
            Schedule any=cur;any.mk=INF;any.total=INF;
            cr.trial(cur,gr,rng()%a.p,any);checked++;
            Schedule bounded=cur;cr.trial(cur,gr,rng()%a.p,bounded);checked++;
        }
        for(int ss=0;ss<2;ss++){
            auto x=ss?a.scct():a.dsla();auto ref=a.replay(x.order,x.pr);assert(x.ft==ref.ft);seeds++;
            Repair rr(a);rr.verify=true;rr.budget=256;rr.rounds=3;auto z=rr.run(x);
            assert(z.mk<=x.mk);assert(a.replay(z.order,z.pr).ft==z.ft);
        }
        graphs++;
    }
    cout<<"random graphs="<<graphs<<" full-vs-incremental trial checks="<<checked<<" seed replay checks="<<seeds<<"\n";
}
