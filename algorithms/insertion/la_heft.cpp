// la_heft.cpp - Lookahead HEFT (Bittencourt, Sakellariou, Madeira, PDP 2010, doi:10.1109/PDP.2010.56),
// "max" variant with depth 1, implemented with insertion-based placement for a fair comparison with CGS.
//
// Priority: HEFT upward rank (mean computation + full communication), as algorithms/makespan/heft.cpp.
// For the highest-rank ready task t and every processor r: tentatively place t on r (earliest idle gap
// that fits), then schedule t's children in rank order with insertion-based HEFT on the resulting partial
// schedule, considering only t and already scheduled parents (as in the paper). The value of r is the
// largest EFT among the children; t goes to the processor with the smallest value (ties: smaller EFT of t,
// then lower index). Tasks without children take their EFT processor. The tentative placements are undone.
// Option "append": non-insertion placement (closer to the repository baselines).
// Input (d3): n p, n x p computation, n x n communication (-1 = no edge).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
struct Slot { int task, proc; ll start, finish; };
static bool earlier(const Slot &a, const Slot &b) { return tie(a.start, a.finish, a.task) < tie(b.start, b.finish, b.task); }
int n, p; bool app = false;
vector<vector<ll>> W, C;
vector<vector<pair<int, ll>>> succ, pred;
vector<vector<Slot>> cal; vector<ll> avail;
vector<Slot> S; vector<char> done;

// earliest start of u on q given committed schedule plus tentative slots `local`
Slot slot(int u, int q, const vector<Slot> &local) {
    ll rel = 0, dur = W[u][q];
    for (auto [x, c] : pred[u]) if (done[x]) rel = max(rel, S[x].finish + (S[x].proc == q ? 0 : c));
    for (const Slot &s : local) { ll c = C[s.task][u]; if (c >= 0) rel = max(rel, s.finish + (s.proc == q ? 0 : c)); }
    if (app) {
        ll st = max(rel, avail[q]);
        for (const Slot &s : local) if (s.proc == q) st = max(st, s.finish);
        return {u, q, st, st + dur};
    }
    if (dur == 0) return {u, q, rel, rel};
    vector<Slot> extra;
    for (const Slot &s : local) if (s.proc == q && s.finish > s.start) extra.push_back(s);
    sort(extra.begin(), extra.end(), earlier);
    size_t i = 0, j = 0; ll st = rel;
    while (i < cal[q].size() || j < extra.size()) {
        Slot b;
        if (j == extra.size() || (i < cal[q].size() && earlier(cal[q][i], extra[j]))) b = cal[q][i++]; else b = extra[j++];
        if (b.finish <= st) continue;
        if (st + dur <= b.start) break;
        st = max(st, b.finish);
    }
    return {u, q, st, st + dur};
}
Slot bestEft(int u, const vector<Slot> &local) {
    Slot b = slot(u, 0, local);
    for (int q = 1; q < p; q++) { Slot s = slot(u, q, local); if (s.finish < b.finish) b = s; }
    return b;
}
int main(int argc, char **argv) {
    if (argc > 1 && string(argv[1]) == "append") app = true;
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << "TASK\tST\tFT\tPRO\n"; return 0; }
    W.assign(n, vector<ll>(p)); C.assign(n, vector<ll>(n));
    for (auto &r : W) for (auto &x : r) cin >> x;
    succ.assign(n, {}); pred.assign(n, {});
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        cin >> C[i][j]; if (C[i][j] != -1) { succ[i].push_back({j, C[i][j]}); pred[j].push_back({i, C[i][j]}); }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s); } }
    vector<double> rk(n, 0);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i]; double m = 0, w = 0;
        for (int k = 0; k < p; k++) w += (double)W[t][k] / p;
        for (auto [s, c] : succ[t]) m = max(m, (double)c + rk[s]);
        rk[t] = w + m;
    }
    vector<int> ord(n); iota(ord.begin(), ord.end(), 0);
    stable_sort(ord.begin(), ord.end(), [&](int a, int b) { return rk[a] > rk[b]; });
    cal.assign(p, {}); avail.assign(p, 0); S.assign(n, {-1, -1, 0, 0}); done.assign(n, 0);
    for (int t : ord) {   // descending rank is a topological order for positive costs; checked below
        for (auto [x, c] : pred[t]) if (!done[x]) { fprintf(stderr, "rank order not topological\n"); return 4; }
        vector<int> kids; for (auto [c, cc] : succ[t]) kids.push_back(c);
        stable_sort(kids.begin(), kids.end(), [&](int a, int b) { return rk[a] > rk[b]; });
        Slot best{}; ll bestVal = LLONG_MAX;
        for (int r = 0; r < p; r++) {
            vector<Slot> local{slot(t, r, {})};
            ll val = local[0].finish;
            if (!kids.empty()) {
                val = 0;
                for (int c : kids) { Slot s = bestEft(c, local); local.push_back(s); val = max(val, s.finish); }
            }
            if (val < bestVal || (val == bestVal && local[0].finish < best.finish)) { bestVal = val; best = local[0]; }
        }
        S[t] = best; done[t] = 1; avail[best.proc] = max(avail[best.proc], best.finish);
        if (best.finish > best.start) { auto &v = cal[best.proc]; v.insert(lower_bound(v.begin(), v.end(), best, earlier), best); }
    }
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << S[i].start << "\t" << S[i].finish << "\t" << S[i].proc + 1 << "\n";
}
