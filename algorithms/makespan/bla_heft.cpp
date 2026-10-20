// bla_heft.cpp - BLA-HEFT: Blended Look-Ahead HEFT (proposed method).
//
// Static list scheduling of a DAG on heterogeneous processors, non-insertion policy.
//
// Input : n p, then the n x p computation-cost matrix W, then the n x n communication matrix C
//         (C[i][j] = -1 means no edge i -> j).
// Output: the scheduling order (1-based), then "TASK ST FT PRO" rows (1-based task and processor).
//
// 1. Priority: HEFT upward rank, rank(t) = mean_k W(t,k) + max_{c in succ(t)} (C(t,c) + rank(c)).
//    Tasks are taken in rank order among the ready tasks (all parents scheduled).
// 2. Remaining-path table (AEFT's IOCT):
//      IOCT(t,k) = W(t,k) + max_{c in succ(t)} min_q ( IOCT(c,q) + [q != k] C(t,c) )
// 3. Processor selection for task t. For each processor k:
//      EFT(t,k)  = max(avail(k), DRT_t(k)) + W(t,k)
//      L(c|k)    = min_q [ max(avail'(q), EFT(t,k) + [q != k] C(t,c), DRT_c(q)) + IOCT(c,q) ]
//                  with avail'(k) = EFT(t,k) and avail'(q) = avail(q) otherwise
//      Look(t,k) = max over the R most critical children c of L(c|k)  +  W(t,k)
//      Score(t,k)= (1 - LAMBDA) * EFT(t,k) + LAMBDA * Look(t,k)
//    t goes to the processor with the smallest score (ties: smaller EFT, then lower index).
//    Exit tasks use plain EFT. DRT_c(q), the data-ready time of child c on q from its already
//    scheduled parents, is updated incrementally after every assignment.
//
// Complexity: O(e p) for the table and the DRT updates, O(R v p^2) for the selection,
// i.e. O(e p + v p^2) = O(v^2 p) for constant R.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const double LAMBDA = 0.5; // blend weight of the look-ahead term
static const int R = 3;           // number of children looked at

int main()
{
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, p;
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    vector<vector<int>> W(n, vector<int>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    vector<vector<pair<int,int>>> succ(n), pred(n); // (task, communication cost)
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        int c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); }
    }

    // topological order
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

    // IOCT table; min_q (X(c,q) + [q != k] C) = min(X(c,k), min_q X(c,q) + C) keeps it O(e p)
    vector<vector<ll>> ioct(n, vector<ll>(p));
    vector<ll> ioctMin(n);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        for (int k = 0; k < p; k++) {
            ll mx = 0;
            for (auto [c, cc] : succ[t]) mx = max(mx, min(ioct[c][k], ioctMin[c] + cc));
            ioct[t][k] = mx + W[t][k];
        }
        ioctMin[t] = *min_element(ioct[t].begin(), ioct[t].end());
    }

    // HEFT upward rank
    vector<double> rank(n);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        double wbar = 0; for (int k = 0; k < p; k++) wbar += W[t][k]; wbar /= p;
        double mx = 0;
        for (auto [c, cc] : succ[t]) mx = max(mx, cc + rank[c]);
        rank[t] = wbar + mx;
    }

    // scheduling order: highest rank among the ready tasks (ties: lower index)
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

    // the R most critical children of each task: largest mean IOCT (ties: edge order)
    vector<vector<pair<int,int>>> crit(n);
    for (int t = 0; t < n; t++) {
        vector<pair<int,int>> ch = succ[t];
        stable_sort(ch.begin(), ch.end(), [&](const pair<int,int> &a, const pair<int,int> &b) {
            double sa = 0, sb = 0;
            for (int q = 0; q < p; q++) { sa += ioct[a.first][q]; sb += ioct[b.first][q]; }
            return sa > sb;
        });
        if ((int)ch.size() > R) ch.resize(R);
        crit[t] = ch;
    }

    vector<ll> ST(n), FT(n), avail(p, 0), eft(p), look(p);
    vector<int> PR(n, -1);
    vector<vector<ll>> drt(n, vector<ll>(p, 0));
    for (int t : order) {
        for (int k = 0; k < p; k++) eft[k] = max(avail[k], drt[t][k]) + W[t][k];
        int best = 0;
        if (succ[t].empty()) {
            for (int k = 1; k < p; k++) if (eft[k] < eft[best]) best = k;
        } else {
            vector<double> score(p);
            for (int k = 0; k < p; k++) {
                ll lk = 0;
                for (auto [c, cc] : crit[t]) {
                    ll L = LLONG_MAX;
                    for (int q = 0; q < p; q++) {
                        ll av = (q == k) ? eft[k] : avail[q];
                        ll start = max(max(av, eft[k] + (q == k ? 0 : cc)), drt[c][q]);
                        L = min(L, start + ioct[c][q]);
                    }
                    lk = max(lk, L);
                }
                look[k] = lk + W[t][k];
                score[k] = (1 - LAMBDA) * eft[k] + LAMBDA * look[k];
            }
            for (int k = 1; k < p; k++)
                if (score[k] < score[best] - 1e-9 || (fabs(score[k] - score[best]) <= 1e-9 && eft[k] < eft[best]))
                    best = k;
        }
        PR[t] = best; FT[t] = eft[best]; ST[t] = FT[t] - W[t][best]; avail[best] = FT[t];
        for (auto [c, cc] : succ[t])
            for (int q = 0; q < p; q++) drt[c][q] = max(drt[c][q], FT[t] + (q == best ? 0 : cc));
    }

    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
