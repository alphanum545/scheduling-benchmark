// bppts.cpp - BPPTS: Balanced Prediction Priority Task Scheduling.
// M. Wang, J. Chen, H. Wang, Z. Gao, W. Bian, S. Qiao, "An enhanced list scheduling algorithm for
// heterogeneous computing using an optimized Predictive Cost Matrix", Future Generation Computer
// Systems 166 (2025) 107733. https://doi.org/10.1016/j.future.2025.107733
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Balanced Predictive Cost Matrix (Eq. 17):
//   BPCM(i,k) = wbar(i) + max_{j in succ(i)} min_n ( BPCM(j,n) + w(j,n) + [n != k] c(i,j) )
//   BPCM(exit,n) = w(exit,n)
// Priority (Eq. 18): rank(i) = mean_k BPCM(i,k) * (1 + wbar(i) / cbar(i))
//   The paper writes the factor as w(v_i) / c_ij without fixing which edge for several successors.
//   Its Table 2 is matched (within the paper's own rounding) by wbar(i) over the MEAN cost of i's
//   outgoing edges, which is used here. Exit tasks (no outgoing edge) get factor 1; a mean edge
//   cost of 0 is treated as 1 to keep the factor finite.
// Tasks are scheduled in rank order among the ready tasks (Algorithm 1, ties: lower index).
// Processor selection (Eq. 19 and Algorithm 1): out-degree > P -> processor with the smallest EFT;
//   otherwise the processor with the smallest B_EFT(i,k) = EFT(i,k) + BPCM(i,k); ties go to the smaller
//   EFT (this reproduces step 6 of the paper's Table 3). Non-insertion.
// Complexity O(e p + v p) here (the min over n uses min(X(j,k), min_n X(j,n) + c)); the paper
// states O(v^2 p).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main()
{
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, p;
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    vector<vector<int>> W(n, vector<int>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    vector<vector<pair<int,int>>> succ(n), pred(n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        int c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    {
        deque<int> q;
        for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
        while (!q.empty()) {
            int t = q.front(); q.pop_front(); topo.push_back(t);
            for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s);
        }
        if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }
    }
    vector<double> wbar(n);
    for (int t = 0; t < n; t++) { double s = 0; for (int k = 0; k < p; k++) s += W[t][k]; wbar[t] = s / p; }

    // BPCM table (Eq. 17)
    vector<vector<double>> B(n, vector<double>(p));
    vector<double> minX(n); // min_n (BPCM(j,n) + w(j,n))
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        for (int k = 0; k < p; k++) {
            if (succ[t].empty()) { B[t][k] = W[t][k]; continue; }
            double mx = 0;
            for (auto [c, cc] : succ[t]) mx = max(mx, min(B[c][k] + W[c][k], minX[c] + cc));
            B[t][k] = wbar[t] + mx;
        }
        minX[t] = 1e300;
        for (int k = 0; k < p; k++) minX[t] = min(minX[t], B[t][k] + W[t][k]);
    }
    // priority (Eq. 18)
    vector<double> rank(n);
    for (int t = 0; t < n; t++) {
        double m = 0; for (int k = 0; k < p; k++) m += B[t][k]; m /= p;
        double f = 1;
        if (!succ[t].empty()) {
            double cs = 0; for (auto [c, cc] : succ[t]) cs += cc;
            double cbar = cs / succ[t].size();
            f = 1 + wbar[t] / max(cbar, 1.0);
        }
        rank[t] = m * f;
    }
#ifdef BPPTS_DEBUG
    for (int t = 0; t < n; t++) {
        fprintf(stderr, "T%d BPCM", t + 1);
        for (int k = 0; k < p; k++) fprintf(stderr, " %.1f", B[t][k]);
        fprintf(stderr, "  rank %.1f\n", rank[t]);
    }
#endif
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rank[a] > rank[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    vector<int> order;
    {
        for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
        priority_queue<int, vector<int>, greater<int>> ready;
        for (int i = 0; i < n; i++) if (!indeg[i]) ready.push(pos[i]);
        while (!ready.empty()) {
            int t = prio[ready.top()]; ready.pop(); order.push_back(t);
            for (auto [s, c] : succ[t]) if (--indeg[s] == 0) ready.push(pos[s]);
        }
    }
    // processor selection (Eq. 19)
    vector<ll> ST(n), FT(n), avail(p, 0), eft(p);
    vector<int> PR(n, -1);
    for (int t : order) {
        for (int k = 0; k < p; k++) {
            ll est = avail[k];
            for (auto [u, cc] : pred[t]) est = max(est, FT[u] + (PR[u] == k ? 0 : cc));
            eft[k] = est + W[t][k];
        }
        int best = 0;
        if ((int)succ[t].size() > p) {
            for (int k = 1; k < p; k++) if (eft[k] < eft[best]) best = k;
        } else {
            for (int k = 1; k < p; k++) {
                double sk = eft[k] + B[t][k], sb = eft[best] + B[t][best];
                if (sk < sb - 1e-9 || (fabs(sk - sb) <= 1e-9 && eft[k] < eft[best])) best = k;
            }
        }
        PR[t] = best; FT[t] = eft[best]; ST[t] = FT[t] - W[t][best]; avail[best] = FT[t];
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
