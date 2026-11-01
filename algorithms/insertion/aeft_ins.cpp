// aeft_ins.cpp - AEFT (Wang, Wang, Qiao, Chen, Xie, Guo, "Heterogeneous system list scheduling algorithm
// based on improved optimistic cost matrix", Future Generation Computer Systems 164 (2025) 107576,
// https://doi.org/10.1016/j.future.2024.107576) with the paper's insertion-based placement.
//
// The repository's algorithms/makespan/aeft.cpp appends each task after the last task on a processor.
// The paper's Algorithm 1, line 6, computes EFT "using insertion-based scheduling policy" (Eq. 5),
// so this file is the faithful version. `append` reproduces the repository AEFT (non-insertion).
//
//   IOCT(i,k) = w(i,k) + max_{j in succ(i)} min_n ( IOCT(j,n) + [n != k] c(i,j) ),  IOCT(exit,k) = w(exit,k)  (Eq. 18)
//   rank(i)   = mean_k IOCT(i,k)                                                                        (Eq. 19)
//   ready list: highest rank among ready tasks (ties: lower index), as the repository AEFT.
//   out-degree(i) > P -> processor of minimum EFT; otherwise minimum IEFT = EFT + IOCT (Eq. 20).
//   Ties: lower processor index.
// The paper's worked example (Fig. 1, Table 3) gives makespan 76 in both modes.
//
// Input (d3): n p, n x p computation, n x n communication (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based).
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
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }
    // IOCT, Eq. 18 (min over n uses min(IOCT(j,k), min_n IOCT(j,n) + c))
    vector<vector<ll>> I(n, vector<ll>(p));
    for (int idx = n - 1; idx >= 0; idx--) {
        int i = topo[idx];
        for (int k = 0; k < p; k++) {
            ll mx = 0;
            for (auto [j, c] : succ[i]) {
                ll mn = LLONG_MAX;
                for (int q = 0; q < p; q++) mn = min(mn, I[j][q] + (q == k ? 0 : c));
                mx = max(mx, mn);
            }
            I[i][k] = W[i][k] + mx;
        }
    }
    vector<double> rk(n);
    for (int i = 0; i < n; i++) { double s = 0; for (int k = 0; k < p; k++) s += I[i][k]; rk[i] = s / p; }
    // priority list: rank desc, ties lower index (as the repository AEFT's priority queue)
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rk[a] > rk[b]; });
    vector<int> pos(n); for (int k = 0; k < n; k++) pos[prio[k]] = k;
    vector<vector<pair<ll, ll>>> busy(p);   // sorted (start, finish) per processor
    vector<ll> ST(n), FT(n), avail(p, 0); vector<int> PR(n), order;
    vector<int> left(n); for (int i = 0; i < n; i++) left[i] = pred[i].size();
    priority_queue<int, vector<int>, greater<int>> ready;
    for (int i = 0; i < n; i++) if (!left[i]) ready.push(pos[i]);
    while (!ready.empty()) {
        int t = prio[ready.top()]; ready.pop(); order.push_back(t);
        bool byEFT = (int)succ[t].size() > p;
        ll bestKey = LLONG_MAX, bs = 0, bf = 0; int bq = 0;
        for (int q = 0; q < p; q++) {
            ll drt = 0;
            for (auto [u, c] : pred[t]) drt = max(drt, FT[u] + (PR[u] == q ? 0 : c));
            ll dur = W[t][q], st;
            if (append) st = max(drt, avail[q]);
            else {
                st = drt;
                if (dur > 0)
                    for (auto [s, f] : busy[q]) { if (f <= st) continue; if (st + dur <= s) break; st = max(st, f); }
            }
            ll eft = st + dur, key = byEFT ? eft : eft + I[t][q];
            if (key < bestKey) { bestKey = key; bs = st; bf = eft; bq = q; }
        }
        ST[t] = bs; FT[t] = bf; PR[t] = bq; avail[bq] = max(avail[bq], bf);
        if (bf > bs) { auto &v = busy[bq]; v.insert(lower_bound(v.begin(), v.end(), make_pair(bs, bf)), {bs, bf}); }
        for (auto [s, c] : succ[t]) if (--left[s] == 0) ready.push(pos[s]);
    }
    for (int k = 0; k < n; k++) cout << order[k] + 1 << (k + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
