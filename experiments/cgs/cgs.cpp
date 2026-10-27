// cgs.cpp - CGS: Contention-table, Gated join-closure Scheduling (experimental; unifies SCCT and RCIS).
// Independent of DSLA, SCCT's and RCIS's sources are not called; their ideas are re-implemented here.
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: "TASK ST FT PRO" rows (1-based task and processor).
//
// 1. Contention table T(t,k) (from SCCT): w(t,k) plus the latest downstream finish of t's children when
//    they are placed greedily, most critical first, on processors that are all free at t's finish
//    (siblings compete for processors). Each cost counted once.
// 2. Priority: HEFT upward rank (mean computation + full communication). Ready queue: highest rank
//    first, ties lower index.
// 3. Single-task placement: insertion-based (earliest idle gap that fits); processor
//    argmin_k FT(t,k) + lam * T(t,k), ties lower index.
// 4. Gated join closure (from RCIS, repaired): when the popped task u has a child v whose remaining
//    parents (at most `parents`) are all ready, and no other ready task outranks v once v is credited
//    with the largest communication cost on its edges from those parents (the cost a split would add)
//    - gate: rank(x) > rank(v) + credit * max_u c(u,v) rejects the closure - the remaining
//    parents of v are placed jointly: every parent order, every processor q for v, and for each parent
//    its `shortlist` best processors by the placement score plus q. A plan is scored by the
//    lexicographic key (max over plan tasks of FT + T - w, FT of v, sum of FT, total work); the
//    sequential single-task plan is always a candidate, so the chosen plan never has a worse key.
//    Only the parents are committed; v returns to the ready queue.
//
// Options (defaults = CGS):
//   lam=<x>      selection weight on T (default 0.5)
//   single       no closures (insertion list scheduling with the contention table)
//   nogate       form closures regardless of other ready tasks' rank
//   credit=<x>   communication credit in the gate (default 1; 0 = plain rank gate)
//   commit=all   also commit the join from the closure plan
//   tail=none    closure key uses FT only (no downstream estimate)
//   parents=<m>  closure size cap, 1..3 (default 2); shortlist=<s> 1..3 (default 2)
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
struct Slot { int task, proc; ll start, finish; };
static bool earlier(const Slot &a, const Slot &b) { return tie(a.start, a.finish, a.task) < tie(b.start, b.finish, b.task); }

int n, p;
vector<vector<ll>> W, C;
vector<vector<pair<int, ll>>> succ, pred;
vector<vector<ll>> T;
vector<double> rnk;
double lam = 0.5, credit = 1; bool bundles = true, gate = true, commitAll = false, tailT = true; int maxPar = 2, shortlist = 2;
vector<vector<Slot>> cal;
vector<Slot> S; vector<char> done, rdy; vector<int> leftP, readyPar;
vector<vector<ll>> drt;
long long nClos = 0, nGated = 0;

Slot slot(int u, int q, const vector<Slot> &local) {
    ll rel = drt[u][q], dur = W[u][q];
    for (const Slot &s : local) { ll c = C[s.task][u]; if (c >= 0) rel = max(rel, s.finish + (s.proc == q ? 0 : c)); }
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
double score(const Slot &s) { return s.finish + lam * T[s.task][s.proc]; }
Slot place(int u, const vector<Slot> &local) {
    Slot best = slot(u, 0, local); double bs = score(best);
    for (int q = 1; q < p; q++) { Slot s = slot(u, q, local); double sc = score(s); if (sc < bs) { bs = sc; best = s; } }
    return best;
}
typedef tuple<ll, ll, ll, ll> Key;
Key key(const vector<Slot> &plan, int join) {
    ll hz = 0, jf = 0, sum = 0, work = 0;
    for (const Slot &s : plan) {
        hz = max(hz, s.finish + (tailT ? T[s.task][s.proc] - W[s.task][s.proc] : 0));
        sum += s.finish; work += W[s.task][s.proc]; if (s.task == join) jf = s.finish;
    }
    return {hz, jf, sum, work};
}
void commit(const Slot &s, priority_queue<pair<double, int>> &pq) {
    int u = s.task;
    if (done[u] || !rdy[u] || leftP[u]) { fprintf(stderr, "unsafe commit\n"); exit(4); }
    S[u] = s; done[u] = 1; rdy[u] = 0;
    if (s.finish > s.start) { auto &v = cal[s.proc]; v.insert(lower_bound(v.begin(), v.end(), s, earlier), s); }
    for (auto [v, c] : succ[u]) {
        readyPar[v]--; leftP[v]--;
        for (int q = 0; q < p; q++) drt[v][q] = max(drt[v][q], s.finish + (s.proc == q ? 0 : c));
        if (!leftP[v]) { rdy[v] = 1; pq.push({rnk[v], -v}); for (auto [x, cx] : succ[v]) readyPar[x]++; }
    }
}
vector<Slot> closure(int join, vector<int> par) {
    vector<Slot> best;
    sort(par.begin(), par.end(), [&](int a, int b) { return rnk[a] != rnk[b] ? rnk[a] > rnk[b] : a < b; });
    for (int u : par) best.push_back(place(u, best));
    best.push_back(place(join, best));
    Key bk = key(best, join);
    vector<vector<int>> sl(n);
    for (int u : par) {
        vector<pair<double, int>> v;
        for (int q = 0; q < p; q++) v.push_back({score(slot(u, q, {})), q});
        int m = min(shortlist, p); partial_sort(v.begin(), v.begin() + m, v.end());
        for (int k = 0; k < m; k++) sl[u].push_back(v[k].second);
    }
    sort(par.begin(), par.end());
    do {
        for (int q = 0; q < p; q++) {
            vector<Slot> local;
            function<void(size_t)> visit = [&](size_t d) {
                if (d == par.size()) {
                    local.push_back(slot(join, q, local));
                    Key k = key(local, join); if (k < bk) { bk = k; best = local; }
                    local.pop_back(); return;
                }
                int u = par[d]; auto ch = sl[u];
                if (find(ch.begin(), ch.end(), q) == ch.end()) ch.push_back(q);
                for (int k : ch) { local.push_back(slot(u, k, local)); visit(d + 1); local.pop_back(); }
            };
            visit(0);
        }
    } while (next_permutation(par.begin(), par.end()));
    return best;
}

int main(int argc, char **argv) {
    for (int a = 1; a < argc; a++) {
        string x = argv[a], v = x.substr(x.find('=') + 1);
        if (x.rfind("lam=", 0) == 0) lam = atof(v.c_str());
        else if (x == "single") bundles = false;
        else if (x == "nogate") gate = false;
        else if (x.rfind("credit=", 0) == 0) credit = atof(v.c_str());
        else if (x == "commit=all") commitAll = true;
        else if (x == "tail=none") tailT = false;
        else if (x.rfind("parents=", 0) == 0) maxPar = atoi(v.c_str());
        else if (x.rfind("shortlist=", 0) == 0) shortlist = atoi(v.c_str());
        else { fprintf(stderr, "unknown argument %s\n", argv[a]); return 2; }
    }
    if (maxPar < 1 || maxPar > 3 || shortlist < 1 || shortlist > 3) { fprintf(stderr, "bad caps\n"); return 2; }
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
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }
    // contention table (SCCT, cost once)
    T.assign(n, vector<ll>(p, 0));
    vector<ll> busy(p); vector<pair<ll, int>> order;
    for (int idx = n - 1; idx >= 0; idx--) {
        int t = topo[idx];
        for (int k = 0; k < p; k++) {
            ll worst = 0;
            if (!succ[t].empty()) {
                order.clear();
                for (int ci = 0; ci < (int)succ[t].size(); ci++) {
                    auto [c, cc] = succ[t][ci]; ll b = LLONG_MAX;
                    for (int q = 0; q < p; q++) b = min(b, T[c][q] + (q == k ? 0 : cc));
                    order.push_back({-b, ci});
                }
                sort(order.begin(), order.end());
                fill(busy.begin(), busy.end(), 0);
                for (auto [nb, ci] : order) {
                    auto [c, cc] = succ[t][ci]; ll best = LLONG_MAX; int bq = 0;
                    for (int q = 0; q < p; q++) { ll f = max(busy[q], (ll)(q == k ? 0 : cc)) + T[c][q]; if (f < best) { best = f; bq = q; } }
                    busy[bq] = max(busy[bq], (ll)(bq == k ? 0 : cc)) + W[c][bq];
                    worst = max(worst, best);
                }
            }
            T[t][k] = W[t][k] + worst;
        }
    }
    // HEFT upward rank
    rnk.assign(n, 0);
    for (int idx = n - 1; idx >= 0; idx--) {
        int t = topo[idx]; double m = 0, wt = 0;
        for (int k = 0; k < p; k++) wt += (double)W[t][k] / p;
        for (auto [c, cc] : succ[t]) m = max(m, (double)cc + rnk[c]);
        rnk[t] = wt + m;
    }
    // ready-queue order must match a stable sort by rank (ties lower index): use (rank, -index)
    cal.assign(p, {}); S.assign(n, {-1, -1, 0, 0}); done.assign(n, 0); rdy.assign(n, 0);
    leftP.assign(n, 0); readyPar.assign(n, 0); drt.assign(n, vector<ll>(p, 0));
    priority_queue<pair<double, int>> pq;
    for (int i = 0; i < n; i++) leftP[i] = pred[i].size();
    for (int i = 0; i < n; i++) if (!leftP[i]) { rdy[i] = 1; pq.push({rnk[i], -i}); for (auto [x, cx] : succ[i]) readyPar[x]++; }
    int placed = 0;
    while (placed < n) {
        while (!pq.empty() && done[-pq.top().second]) pq.pop();
        if (pq.empty()) { fprintf(stderr, "ready queue exhausted\n"); return 5; }
        int u = -pq.top().second; pq.pop();
        int join = -1;
        if (bundles)
            for (auto [v, c] : succ[u]) {
                if (done[v] || leftP[v] < 1 || leftP[v] > maxPar || readyPar[v] != leftP[v]) continue;
                if (join < 0 || rnk[v] > rnk[join] || (rnk[v] == rnk[join] && v < join)) join = v;
            }
        vector<int> par;
        if (join >= 0) {
            for (auto [x, c] : pred[join]) if (!done[x]) par.push_back(x);
            if (gate) {
                double top = -1;
                for (int x = 0; x < n; x++) if (rdy[x] && !done[x] && find(par.begin(), par.end(), x) == par.end()) top = max(top, rnk[x]);
                double cr = 0;   // communication the join would pay if its parents are split
                for (int x : par) cr = max(cr, (double)C[x][join]);
                if (top > rnk[join] + credit * cr) { join = -1; nGated++; }
            }
        }
        if (join < 0) { commit(place(u, {}), pq); placed++; continue; }
        nClos++;
        auto plan = closure(join, par);
        for (const Slot &s : plan) if (commitAll || s.task != join) { commit(s, pq); placed++; }
    }
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << S[i].start << "\t" << S[i].finish << "\t" << S[i].proc + 1 << "\n";
    ll mk = 0; for (auto &s : S) mk = max(mk, s.finish);
    fprintf(stderr, "method=cgs makespan=%lld closures=%lld gated=%lld\n", mk, nClos, nGated);
}
