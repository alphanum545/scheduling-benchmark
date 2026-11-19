// D3 insertion-based HEFT, PEFT and depth-one maximum-EFT Lookahead HEFT.
#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
using Time = long long;
struct Slot { Time start, finish; int cpu; };
struct State {
    vector<vector<pair<Time,Time>>> busy;
    vector<Slot> assigned;
    void place(int task, Slot slot) {
        assigned[task] = slot;
        if (slot.start == slot.finish) return;
        auto &line = busy[slot.cpu];
        auto interval = make_pair(slot.start, slot.finish);
        line.insert(lower_bound(line.begin(), line.end(), interval), interval);
    }
};
int main(int argc, char **argv) {
    if (argc != 2) return 2;
    string method = argv[1];
    bool peft = method == "peft", look = method == "lookahead";
    if (!peft && !look && method != "heft") return 2;
    int n,p;
    if (!(cin >> n >> p) || n<0 || p<=0) return 2;
    vector<vector<Time>> w(n,vector<Time>(p)), oct=w;
    vector<vector<pair<int,Time>>> parents(n), children(n);
    vector<int> degree(n), topo;
    for (auto &row:w) for (auto &v:row) if (!(cin>>v) || v<0) return 2;
    for (int i=0;i<n;++i) for (int j=0;j<n;++j) {
        Time c;
        if (!(cin>>c) || c < -1) return 2;
        if (c<0) continue;
        parents[j].emplace_back(i,c); children[i].emplace_back(j,c); ++degree[j];
    }
    auto pending=degree;
    for (int i=0;i<n;++i) if (!pending[i]) topo.push_back(i);
    for (size_t i=0;i<topo.size();++i)
        for (auto [j,c]:children[topo[i]]) if (!--pending[j]) topo.push_back(j);
    if (int(topo.size())!=n) return 2;
    // Scale ranks by p to compare exact sums on integer benchmark inputs.
    vector<Time> rank(n);
    for (auto it=topo.rbegin();it!=topo.rend();++it) {
        int t=*it;
        for (auto [j,c]:children[t]) {
            if (!peft) { rank[t]=max(rank[t],rank[j]+p*c); continue; }
            Time remote=oct[j][0]+w[j][0];
            for (int k=1;k<p;++k) remote=min(remote,oct[j][k]+w[j][k]);
            for (int k=0;k<p;++k)
                oct[t][k]=max(oct[t][k],min(oct[j][k]+w[j][k],remote+c));
        }
        for (int k=0;k<p;++k) rank[t]+=peft ? oct[t][k] : w[t][k];
    }
    auto earlier=[&](int a,int b){return rank[a]!=rank[b]?rank[a]>rank[b]:a<b;};
    for (auto &list:children) stable_sort(list.begin(),list.end(),
        [&](auto a,auto b){return earlier(a.first,b.first);});
    auto slot=[&](const State &s,int t,int k) {
        Time start=0;
        for (auto [j,c]:parents[t]) {
            auto parent=s.assigned[j];
            if (parent.cpu>=0) start=max(start,parent.finish+(parent.cpu==k?0:c));
        }
        if (w[t][k]) for (auto [a,b]:s.busy[k]) {
            if (start+w[t][k]<=a) break;
            start=max(start,b);
        }
        return Slot{start,start+w[t][k],k};
    };
    State state{vector<vector<pair<Time,Time>>>(p),vector<Slot>(n,{0,0,-1})};
    priority_queue<pair<Time,int>> ready;
    for (int t:topo) if (!degree[t]) ready.emplace(rank[t],-t);
    while (!ready.empty()) {
        int t=-ready.top().second; ready.pop();
        Slot chosen{};
        tuple<Time,Time,int> best;
        for (int k=0;k<p;++k) {
            auto candidate=slot(state,t,k);
            Time score=candidate.finish+oct[t][k];
            if (look && !children[t].empty()) {
                State trial=state;
                trial.place(t,candidate);
                score=0;
                for (auto [j,c]:children[t]) {
                    auto child=slot(trial,j,0);
                    for (int q=1;q<p;++q) {
                        auto alternative=slot(trial,j,q);
                        if (alternative.finish<child.finish) child=alternative;
                    }
                    trial.place(j,child);
                    score=max(score,child.finish);
                }
            }
            auto key=make_tuple(score,candidate.finish,k);
            if (!k || key<best) { best=key; chosen=candidate; }
        }
        state.place(t,chosen);
        cout<<t+1<<' ';
        for (auto [j,c]:children[t]) if (!--degree[j]) ready.emplace(rank[j],-j);
    }
    cout<<"\nTASK\tST\tFT\tPRO\n";
    for (int t=0;t<n;++t) {
        auto s=state.assigned[t];
        cout<<t+1<<'\t'<<s.start<<'\t'<<s.finish<<'\t'<<s.cpu+1<<'\n';
    }
}
