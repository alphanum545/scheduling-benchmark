// pmms.cpp - PMMS: workflow scheduling based on the Predict Makespan Matrix.
// L. Zhang, M. Ai, R. Tan et al., "Efficient Prediction of Makespan Matrix Workflow Scheduling
// Algorithm for Heterogeneous Cloud Environments", Journal of Grid Computing 21 (2023) 75.
// https://doi.org/10.1007/s10723-023-09711-9
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Predict Makespan matrix (Eq. 8):
//   PM(i,j) = max_{k in succ(i)} min_{g in P} ( PM(k,g) + w(i,g) + [g != j] c(i,k) ),  PM(exit,j) = w(exit,j)
//   (note: the paper's term is w(i,g), the current task's cost on the successor's VM; reproduced as written.
//    The min over g is computed as min(A_j, min_g A_g + c) with A_g = PM(k,g) + w(i,g), O(p) per edge.)
// Priority (Eq. 9): rank(i) = mean_j PM(i,j).
// Scaling-down policy (Eq. 10, Algorithm 1 lines 7-14): for each task i in index order and each successor g
//   with rank(i) <= rank(g): PM(g,j) <- PM(g,j) * rank(i) / (rank(g) + theta), theta = 1.0, then rank(g)
//   is recomputed. (Algorithm 1 line 12 says "recalculate rank(i)"; the text says the successor's rank.)
// Task order: descending rank; as for every algorithm in this benchmark, a task is taken only when all its
//   predecessors are scheduled (highest-rank ready task; ties: lower index).
// VM selection (Eq. 11): the VM minimizing OSL(i,j) = EFT(i,j) + PM(i,j). Algorithm 2 updates on
//   "OSL <= best", so ties go to the later VM. Non-insertion (EST uses avail(vm_j), Eq. 3).
//
// Verified on the paper's motivational example (Fig. 1 DAG with the Table 3 costs): PM table as in Table 4
// (the paper's row for task 8, [98, 107, 94], is inconsistent with its own rank 98.00 and with the task-2
// row; Eq. 8 gives [98, 98, 98]; its entry-task row also differs, e.g. PM(1, vm2) = 147 where Eq. 8 gives
// 151 through task 5, which does not affect the schedule), priority list {1,4,5,3,2,6,9,8,7,10},
// every Table 5 assignment and finish time, makespan 161.
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
    // PM matrix (Eq. 8)
    vector<vector<double>> PM(n, vector<double>(p, 0));
    vector<double> A(p);
    for (int x = n - 1; x >= 0; x--) {
        int i = topo[x];
        if (succ[i].empty()) { for (int j = 0; j < p; j++) PM[i][j] = W[i][j]; continue; }
        vector<double> mx(p, 0);
        for (auto [k, c] : succ[i]) {
            double mA = 1e300;
            for (int g = 0; g < p; g++) { A[g] = PM[k][g] + W[i][g]; mA = min(mA, A[g]); }
            for (int j = 0; j < p; j++) mx[j] = max(mx[j], min(A[j], mA + c));
        }
        PM[i] = mx;
    }
    vector<double> rank(n);
    auto rk = [&](int t) { double s = 0; for (int j = 0; j < p; j++) s += PM[t][j]; return s / p; };
    for (int t = 0; t < n; t++) rank[t] = rk(t);
    // scaling-down policy (Eq. 10)
    const double THETA = 1.0;
    for (int i = 0; i < n; i++)
        for (auto [g, c] : succ[i])
            if (rank[i] <= rank[g]) {
                double f = rank[i] / (rank[g] + THETA);
                for (int j = 0; j < p; j++) PM[g][j] *= f;
                rank[g] = rk(g);
            }
#ifdef PMMS_DEBUG
    for (int t = 0; t < n; t++) {
        fprintf(stderr, "T%d PM", t + 1);
        for (int j = 0; j < p; j++) fprintf(stderr, " %.2f", PM[t][j]);
        fprintf(stderr, "  rank %.2f\n", rank[t]);
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
    // VM selection (Eq. 11)
    vector<ll> ST(n), FT(n), avail(p, 0), eft(p);
    vector<int> PR(n, -1);
    for (int t : order) {
        for (int k = 0; k < p; k++) {
            ll est = avail[k];
            for (auto [u, cc] : pred[t]) est = max(est, FT[u] + (PR[u] == k ? 0 : cc));
            eft[k] = est + W[t][k];
        }
        int best = 0;
        for (int k = 1; k < p; k++)
            if (eft[k] + PM[t][k] <= eft[best] + PM[t][best] + 1e-9) best = k;
        PR[t] = best; FT[t] = eft[best]; ST[t] = FT[t] - W[t][best]; avail[best] = FT[t];
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
