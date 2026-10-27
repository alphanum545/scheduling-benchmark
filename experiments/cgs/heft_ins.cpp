// heft_ins.cpp - canonical HEFT (Topcuoglu, Hariri, Wu, IEEE TPDS 2002) with insertion-based placement.
// Upward rank with mean computation and the full communication cost c(i,j) (as the repository HEFT).
// Ties: lower index.
// Each task, in descending rank, goes to the processor giving the earliest finish time using the
// earliest idle gap that fits (insertion policy, Eq. 3 of the paper).
// Input (d3): n p, n x p computation, n x n communication (-1 = no edge).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(int argc, char **argv)
{
    bool append = argc > 1 && string(argv[1]) == "append";
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, p;
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    vector<vector<ll>> W(n, vector<ll>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    vector<vector<pair<int, ll>>> succ(n), pred(n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        ll c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s); } }
    double cf = 1;   // full c(i,j), as in algorithms/makespan/heft.cpp
    vector<double> rk(n, 0);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i]; double m = 0, w = 0;
        for (int k = 0; k < p; k++) w += (double)W[t][k] / p;
        for (auto [s, c] : succ[t]) m = max(m, cf * c + rk[s]);
        rk[t] = w + m;
    }
    vector<int> ord(n); iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) { return rk[a] > rk[b]; });
    vector<ll> ST(n), FT(n), avail(p, 0); vector<int> PR(n, -1);
    vector<vector<pair<ll, ll>>> cal(p);
    for (int t : ord) {
        ll bf = LLONG_MAX, bs = 0; int bk = 0;
        for (int k = 0; k < p; k++) {
            ll d = 0;
            for (auto [u, c] : pred[t]) d = max(d, FT[u] + (PR[u] == k ? 0 : c));
            ll st = d, dur = W[t][k];
            if (append) st = max(d, avail[k]);
            else if (dur > 0)
                for (auto [a, b] : cal[k]) { if (b <= st) continue; if (st + dur <= a) break; st = max(st, b); }
            if (st + dur < bf) { bf = st + dur; bs = st; bk = k; }
        }
        ST[t] = bs; FT[t] = bf; PR[t] = bk; avail[bk] = max(avail[bk], bf);
        if (bf > bs) { auto &v = cal[bk]; v.insert(lower_bound(v.begin(), v.end(), make_pair(bs, bf)), make_pair(bs, bf)); }
    }
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
