// scct.cpp - SCCT: Sibling-Contention Cost Table scheduling.
//
// Motivation (OPPTS). OPPTS's cost table PCG claims to estimate the remaining path length but counts
// every descendant's computation twice, and its processor rule EFT + PCG counts the task's own cost
// twice. Removing those repetitions naively makes OPPTS about 7% worse (oppts_ft / oppts_fs /
// oppts_fb in algorithms.toml). SCCT keeps each quantity's meaning explicit:
//   * processor selection uses a cost table that counts each cost once and, unlike PEFT/OPPTS-style
//     optimistic tables, models siblings competing for the same processor;
//   * task priority uses a separate table whose extra weight on descendant computation is an
//     explicit parameter mu (OPPTS's double counting is exactly the case mu = 1).
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// 1. Contention table T (processor selection), bottom-up, each cost counted once:
//      exit:      T(t,k) = w(t,k)
//      otherwise: T(t,k) = w(t,k) + max_c F_c, the children c of t placed greedily on processors that
//                 all start free at t's finish time:
//                   order: descending b_c = min_q ( T(c,q) + [q != k] c(t,c) ), ties: lower child position
//                   child c may start on q at s_c(q) = max(busy_q, [q != k] c(t,c)) and finishes its
//                   downstream path at F_c(q) = s_c(q) + T(c,q); it takes q* = argmin_q F_c(q), and q*
//                   is then busy until s_c(q*) + w(c,q*)
//    Only immediate siblings are modelled, so T is an estimate, not a bound. With one child, or when
//    no two children share a processor, T equals the cost-once optimistic table.
// 2. Priority table G (task order), bottom-up:
//      G(t,k) = w(t,k) + max_c min_q ( G(c,q) + mu * w(c,q) + gamma * [q != k] c(t,c) )
//    rank(t) = mean_k G(t,k). Defaults mu = 2, gamma = 1: a descendant's computation weighs (1 + mu),
//    its communication once. mu = 1, gamma = 1 reproduces OPPTS's priority exactly.
// 3. List scheduling (non-insertion): the ready list takes the highest-rank ready task (ties: lower
//    index) and places it on argmin_k EFT(t,k) + T(t,k) (first minimum). As in OPPTS, w(t,k) enters
//    both EFT and T: the task's own cost carries an extra unit weight in this rule (see "noself").
//
// Arguments (defaults give SCCT; the rest are ablations reported in results/reports/scct/):
//   mu=<x>        descendant-computation weight in the priority table (default 2)
//   gamma=<x>     communication weight in the priority table (default 1)
//   rank=cont     rank by mean_k T(t,k) instead of G (the first SCCT version)
//   work=mean     add a computation-only upward path length (mean costs) to the rank
//   nocont        no sibling contention in T (cost-once optimistic table for selection)
//   noself        select by EFT(t,k) + T(t,k) - w(t,k)
//   cap=<m>       only the m*p most critical children occupy processors in T
//
// Complexity: tables O(e p^2 + e log e); scheduling O((v + e) p). Overall O(e p^2).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main(int argc, char **argv)
{
    double mu = 2, gamma = 1, capMul = 0;
    bool contention = true, selfWeight = true, rankCont = false, work = false;
    for (int a = 1; a < argc; a++) {
        string x = argv[a], v = x.substr(x.find('=') + 1);
        if (x.rfind("mu=", 0) == 0) mu = atof(v.c_str());
        else if (x.rfind("gamma=", 0) == 0) gamma = atof(v.c_str());
        else if (x.rfind("cap=", 0) == 0) capMul = atof(v.c_str());
        else if (x == "rank=cont") rankCont = true;
        else if (x == "work=mean") work = true;
        else if (x == "nocont") contention = false;
        else if (x == "noself") selfWeight = false;
        else { fprintf(stderr, "unknown argument %s\n", argv[a]); return 2; }
    }
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

    // 1. contention table T
    vector<vector<ll>> T(n, vector<ll>(p, 0));
    vector<ll> busy(p);
    vector<pair<ll, int>> order;
    for (int idx = n - 1; idx >= 0; idx--) {
        int t = topo[idx];
        for (int k = 0; k < p; k++) {
            ll worst = 0;
            if (!succ[t].empty()) {
                order.clear();
                for (int ci = 0; ci < (int)succ[t].size(); ci++) {
                    auto [c, cc] = succ[t][ci];
                    ll b = LLONG_MAX;
                    for (int q = 0; q < p; q++) b = min(b, T[c][q] + (q == k ? 0 : cc));
                    order.push_back({-b, ci});
                }
                sort(order.begin(), order.end());   // descending b, ties: lower child position
                fill(busy.begin(), busy.end(), 0);
                int placed = 0, cap = capMul > 0 ? max(1, (int)llround(capMul * p)) : INT_MAX;
                for (auto [nb, ci] : order) {
                    auto [c, cc] = succ[t][ci];
                    ll best = LLONG_MAX; int bq = 0;
                    for (int q = 0; q < p; q++) {
                        ll f = max(busy[q], (ll)(q == k ? 0 : cc)) + T[c][q];
                        if (f < best) { best = f; bq = q; }
                    }
                    if (contention && placed++ < cap) busy[bq] = max(busy[bq], (ll)(bq == k ? 0 : cc)) + W[c][bq];
                    worst = max(worst, best);
                }
            }
            T[t][k] = W[t][k] + worst;
        }
    }

    // 2. priority
    vector<double> rnk(n, 0);
    if (rankCont) {
        for (int t = 0; t < n; t++) { double s = 0; for (int k = 0; k < p; k++) s += (double)T[t][k]; rnk[t] = s / p; }
    } else {
        vector<vector<double>> G(n, vector<double>(p, 0));
        for (int idx = n - 1; idx >= 0; idx--) {
            int t = topo[idx];
            for (int k = 0; k < p; k++) {
                double worst = 0;
                for (auto [c, cc] : succ[t]) {
                    double b = 1e300;
                    for (int q = 0; q < p; q++) b = min(b, G[c][q] + mu * W[c][q] + (q == k ? 0.0 : gamma * cc));
                    worst = max(worst, b);
                }
                G[t][k] = W[t][k] + worst;
            }
        }
        for (int t = 0; t < n; t++) { double s = 0; for (int k = 0; k < p; k++) s += G[t][k]; rnk[t] = s / p; }
    }
    if (work) {   // + computation-only upward path length with mean costs (no communication)
        vector<double> U(n, 0);
        for (int idx = n - 1; idx >= 0; idx--) {
            int t = topo[idx]; double m = 0, wt = 0;
            for (auto [c, cc] : succ[t]) m = max(m, U[c]);
            for (int k = 0; k < p; k++) wt += (double)W[t][k] / p;
            U[t] = wt + m;
            rnk[t] += U[t];
        }
    }
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rnk[a] > rnk[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;

    // 3. list scheduling (non-insertion)
    vector<ll> avail(p, 0), ST(n), FT(n); vector<int> PR(n, -1), deg(n), seq;
    for (int i = 0; i < n; i++) deg[i] = pred[i].size();
    priority_queue<int, vector<int>, greater<int>> ready;
    for (int i = 0; i < n; i++) if (!deg[i]) ready.push(pos[i]);
    while (!ready.empty()) {
        int t = prio[ready.top()]; ready.pop(); seq.push_back(t);
        ll bestScore = LLONG_MAX; int bk = 0; ll bst = 0, bft = 0;
        for (int k = 0; k < p; k++) {
            ll est = avail[k];
            for (auto [u, cu] : pred[t]) est = max(est, FT[u] + (PR[u] == k ? 0 : cu));
            ll eft = est + W[t][k];
            ll sc = eft + T[t][k] - (selfWeight ? 0 : W[t][k]);
            if (sc < bestScore) { bestScore = sc; bk = k; bst = est; bft = eft; }
        }
        ST[t] = bst; FT[t] = bft; PR[t] = bk; avail[bk] = bft;
        for (auto [s, c] : succ[t]) if (--deg[s] == 0) ready.push(pos[s]);
    }
    for (int i = 0; i < n; i++) cout << seq[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
