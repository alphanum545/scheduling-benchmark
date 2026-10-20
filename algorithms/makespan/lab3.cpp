// lab3.cpp - BLA-HEFT with a communication-aware task-selection mode (round-4 experiments).
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// The processor rule is BLA-HEFT's: Score(t,k) = (1-lam) EFT(t,k) + lam [max_{top-R children} L(c|k) + w(t,k)].
// Task selection (mode):
//   rank      highest HEFT upward rank among the ready tasks (BLA-HEFT)
//   eft       ready task with the smallest min_k EFT (communication-aware: runs what can start now)
// Options (argv):
//   lam=<x> R=<n> order=rank|eft          a single schedule
//   lamhi=<x>                             blend weight used in the eft mode (default = lam)
//   auto=<T>                              graph CCR >= T -> eft mode with lamhi, else rank mode with lam
//                                         (graph CCR = mean edge cost / mean task cost, computed in O(e + v p))
//   best=1                                build both schedules (rank/lam and eft/lamhi), keep the shorter
// Complexity per schedule as BLA-HEFT, O(e p + v p^2); the eft mode adds O(|ready| p) per step for the
// task choice, O(v^2 p) in total; best=1 doubles the constant.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static int n, p;
static vector<vector<int>> W;
static vector<vector<pair<int,int>>> succ, pred;
static vector<vector<ll>> ioct;
static vector<double> rnk;
static vector<int> pos;
static vector<vector<pair<int,int>>> crit;

struct Sched { vector<int> order; vector<ll> ST, FT; vector<int> PR; ll makespan; };

static Sched run(bool eftMode, double lam)
{
    Sched S; S.ST.assign(n, 0); S.FT.assign(n, 0); S.PR.assign(n, -1);
    vector<ll> avail(p, 0), eft(p), look(p);
    vector<vector<ll>> drt(n, vector<ll>(p, 0));
    vector<int> deg(n); for (int i = 0; i < n; i++) deg[i] = pred[i].size();
    vector<int> ready; for (int i = 0; i < n; i++) if (!deg[i]) ready.push_back(i);
    while (!ready.empty()) {
        int bi = 0;
        if (!eftMode) {
            for (int i = 1; i < (int)ready.size(); i++) if (pos[ready[i]] < pos[ready[bi]]) bi = i;
        } else {
            ll be = LLONG_MAX;
            for (int i = 0; i < (int)ready.size(); i++) {
                int t = ready[i]; ll m = LLONG_MAX;
                for (int k = 0; k < p; k++) m = min(m, max(avail[k], drt[t][k]) + (ll)W[t][k]);
                if (m < be || (m == be && pos[t] < pos[ready[bi]])) { be = m; bi = i; }
            }
        }
        int t = ready[bi]; ready.erase(ready.begin() + bi); S.order.push_back(t);
        for (int k = 0; k < p; k++) eft[k] = max(avail[k], drt[t][k]) + W[t][k];
        int best = 0;
        if (succ[t].empty() || lam == 0) {
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
                score[k] = (1 - lam) * eft[k] + lam * look[k];
            }
            for (int k = 1; k < p; k++)
                if (score[k] < score[best] - 1e-9 || (fabs(score[k] - score[best]) <= 1e-9 && eft[k] < eft[best]))
                    best = k;
        }
        S.PR[t] = best; S.FT[t] = eft[best]; S.ST[t] = S.FT[t] - W[t][best]; avail[best] = S.FT[t];
        for (auto [c, cc] : succ[t]) {
            for (int q = 0; q < p; q++) drt[c][q] = max(drt[c][q], S.FT[t] + (q == best ? 0 : cc));
            if (--deg[c] == 0) ready.push_back(c);
        }
    }
    S.makespan = n ? *max_element(S.FT.begin(), S.FT.end()) : 0;
    return S;
}

int main(int argc, char **argv)
{
    double lam = 0.5, lamhi = -1, autoT = -1; int R = 3; bool eftMode = false, best = false;
    for (int i = 1; i < argc; i++) {
        string a = argv[i], v = a.substr(a.find('=') + 1);
        if (a.rfind("lam=", 0) == 0) lam = atof(v.c_str());
        else if (a.rfind("lamhi=", 0) == 0) lamhi = atof(v.c_str());
        else if (a.rfind("R=", 0) == 0) R = atoi(v.c_str());
        else if (a.rfind("order=", 0) == 0) eftMode = v == "eft";
        else if (a.rfind("auto=", 0) == 0) autoT = atof(v.c_str());
        else if (a.rfind("best=", 0) == 0) best = v == "1";
        else { fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
    }
    if (lamhi < 0) lamhi = lam;
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    W.assign(n, vector<int>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    succ.assign(n, {}); pred.assign(n, {});
    double csum = 0; long ecount = 0;
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        int c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); csum += c; ecount++; }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s); } }
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }
    ioct.assign(n, vector<ll>(p)); vector<ll> mn(n);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        for (int k = 0; k < p; k++) {
            ll mx = 0;
            for (auto [c, cc] : succ[t]) mx = max(mx, min(ioct[c][k], mn[c] + cc));
            ioct[t][k] = mx + W[t][k];
        }
        mn[t] = *min_element(ioct[t].begin(), ioct[t].end());
    }
    rnk.assign(n, 0); double wsum = 0;
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        double wbar = 0; for (int k = 0; k < p; k++) wbar += W[t][k]; wbar /= p; wsum += wbar;
        double mx = 0; for (auto [c, cc] : succ[t]) mx = max(mx, cc + rnk[c]);
        rnk[t] = wbar + mx;
    }
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rnk[a] > rnk[b]; });
    pos.assign(n, 0); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    crit.assign(n, {});
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
    double ccr = (ecount && wsum > 0) ? (csum / ecount) / (wsum / n) : 0;

    Sched S;
    if (best) {
        Sched a = run(false, lam), b = run(true, lamhi);
        S = (b.makespan < a.makespan) ? b : a;
    } else if (autoT >= 0) {
        S = ccr >= autoT ? run(true, lamhi) : run(false, lam);
    } else {
        S = run(eftMode, eftMode ? lamhi : lam);
    }
    for (int i = 0; i < n; i++) cout << S.order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << S.ST[i] << "\t" << S.FT[i] << "\t" << S.PR[i] + 1 << "\n";
}
