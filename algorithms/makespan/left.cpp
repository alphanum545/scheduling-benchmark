// left.cpp - LEFT and LEFT-D: Level-wise Earliest Finish Time (with entry-task duplication).
// B. B. Naik, Md. S. A. Ansari, "Level-wise earliest finish time based workflow scheduling for
// heterogeneous computing systems", Cluster Computing 28 (2025) 844.
// https://doi.org/10.1007/s10586-025-05576-7
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
// Options: "dup" = LEFT-D (Algorithm 3); default LEFT (Algorithm 1).
//
// LEFT (Algorithms 1 and 2): keep the set of ready tasks (all parents scheduled) with their EFT on
//   every processor; repeatedly schedule the (task, processor) pair with the minimum EFT, update the
//   processor's release time and the EFTs of the remaining ready tasks on it, and add new ready tasks.
//   Non-insertion. Ties: smaller EFT, then the task that became ready first, then the lower
//   processor index (the paper does not specify; this reproduces its Table 4).
// LEFT-D (Algorithm 3, Section 4.2): only the entry task is duplicated. The paper places a copy of the
//   entry on processor p when the copy delivers its data earlier than a transfer from the original;
//   here: a copy is placed on every p with w(entry,p) < AFT(entry) + min_child c(entry,child), each
//   copy running from time 0 (Fig. 3c). A child on p then receives the entry's data at w(entry,p).
//   Graphs with several entry tasks are scheduled without duplication (the paper assumes one entry).
//   The schedule lists one row per task; the copies are implied, as for LSTD (evaluate --dup-entry).
// Complexity: EFTs are kept per ready task and processor and only the chosen processor's column is
//   refreshed after each step, so the total cost is O(e p + v^2 p) in the worst case.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main(int argc, char **argv)
{
    bool DUP = argc > 1 && string(argv[1]) == "dup";
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
    vector<int> entries;
    for (int i = 0; i < n; i++) if (pred[i].empty()) entries.push_back(i);
    if (entries.size() != 1) DUP = false;

    vector<ll> ST(n, 0), FT(n, 0), avail(p, 0);
    vector<int> PR(n, -1), indeg(n), order;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    vector<char> copyOn(p, 0); // LEFT-D: entry copy on this processor
    int entry = entries.empty() ? -1 : entries[0];

    vector<int> ready;               // in the order tasks became ready
    vector<vector<ll>> drt(n), eft(n);
    auto dataReady = [&](int t, int k) {
        ll d = 0;
        for (auto [u, cc] : pred[t]) {
            ll a = (PR[u] == k) ? FT[u] : FT[u] + cc;
            if (DUP && u == entry && copyOn[k]) a = min(a, (ll)W[u][k]);
            d = max(d, a);
        }
        return d;
    };
    auto makeReady = [&](int t) {
        drt[t].assign(p, 0); eft[t].assign(p, 0);
        for (int k = 0; k < p; k++) { drt[t][k] = dataReady(t, k); eft[t][k] = max(avail[k], drt[t][k]) + W[t][k]; }
        ready.push_back(t);
    };
    auto place = [&](int t, int k) {
        PR[t] = k; FT[t] = eft[t][k]; ST[t] = FT[t] - W[t][k]; avail[k] = FT[t]; order.push_back(t);
    };

    for (int t : entries) makeReady(t);
    bool first = true;
    while (!ready.empty()) {
        int bi = 0, bk = 0;
        for (int i = 0; i < (int)ready.size(); i++)
            for (int k = 0; k < p; k++)
                if (eft[ready[i]][k] < eft[ready[bi]][bk]) { bi = i; bk = k; }
        int t = ready[bi];
        ready.erase(ready.begin() + bi);
        place(t, bk);
        if (DUP && first && t == entry) {
            ll mc = LLONG_MAX; for (auto [c, cc] : succ[t]) mc = min(mc, (ll)cc);
            for (int k = 0; k < p; k++)
                if (k == bk || (mc != LLONG_MAX && W[t][k] < FT[t] + mc)) { copyOn[k] = 1; avail[k] = max(avail[k], (ll)W[t][k]); }
            for (int r : ready) for (int k = 0; k < p; k++) eft[r][k] = max(avail[k], drt[r][k]) + W[r][k];
        }
        first = false;
        for (int r : ready) eft[r][bk] = max(avail[bk], drt[r][bk]) + W[r][bk];
        for (auto [c, cc] : succ[t]) if (--indeg[c] == 0) makeReady(c);
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
