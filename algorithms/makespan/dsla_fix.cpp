// dsla_fix.cpp - DSLA without the repeated w(t,k) in the look-ahead term; otherwise identical to dsla.cpp.
// dsla.cpp - DSLA: Dual-policy State-aware Look-Ahead scheduling (proposed method).
//            With the argument "slas", runs SLAS (State-aware Look-Ahead Scheduling): the first
//            schedule only.
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Static tables (computed once):
//   rank(t)    HEFT upward rank: mean_k w(t,k) + max_{c in succ(t)} (c(t,c) + rank(c))
//   IOCT(t,k)  w(t,k) + max_{c in succ(t)} min( IOCT(c,k), min_q IOCT(c,q) + c(t,c) )
//   K(t)       the R = 3 children of t with the largest mean IOCT (critical children)
//
// BUILD(policy, lambda) - one non-insertion list schedule:
//   ready-task choice:  pi_rank  the ready task with the highest rank (ties: lower index)
//                       pi_eft   the ready task with the smallest min_k EFT(t,k) (ties: higher rank)
//   processor choice:   Score(t,k) = (1 - lambda) EFT(t,k) + lambda max_{c in K(t)} L(c|k)   (no extra w(t,k): L already contains EFT(t,k))
//     L(c|k) = min_q [ max( avail'(q), EFT(t,k) + [q != k] c(t,c), DRT_c(q) ) + IOCT(c,q) ]
//     avail'(q) = EFT(t,k) if q = k else avail(q)   (state after the tentative placement)
//     DRT_c(q)  = data-ready time of c on q from its already scheduled parents (kept incrementally)
//   exit tasks (no children) and lambda = 0 use plain EFT; ties: smaller EFT, then lower processor index.
//
// DSLA = the shorter of BUILD(pi_rank, 0.5) and BUILD(pi_eft, 0.1) (the first one on a tie).
// Complexity: tables O(e p + v log v); each BUILD O(e p + v R p^2) for the processor choice plus
// O(v^2 p) for the pi_eft task choice; O(v^2 p) overall (p <= v).
//
// Produces exactly the same schedules as lab3.cpp with "best=1 lamhi=0.1" (DSLA) and lab3.cpp
// default / bla_heft.cpp (SLAS).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const int R = 3;
static const double LAMBDA_RANK = 0.5, LAMBDA_EFT = 0.1;

static int n, p;
static vector<vector<int>> W;
static vector<vector<pair<int,int>>> succ, pred, crit;
static vector<vector<ll>> ioct;
static vector<int> pos;   // position in descending-rank order (smaller = higher rank)

struct Sched { vector<int> order; vector<ll> ST, FT; vector<int> PR; ll makespan = 0; };

static Sched build(bool eftPolicy, double lam)
{
    Sched S; S.ST.assign(n, 0); S.FT.assign(n, 0); S.PR.assign(n, -1);
    vector<ll> avail(p, 0), eft(p);
    vector<vector<ll>> drt(n, vector<ll>(p, 0));
    vector<int> deg(n); for (int i = 0; i < n; i++) deg[i] = pred[i].size();
    vector<int> ready; for (int i = 0; i < n; i++) if (!deg[i]) ready.push_back(i);
    vector<double> score(p);
    while (!ready.empty()) {
        int bi = 0;
        if (!eftPolicy) {
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
                score[k] = (1 - lam) * eft[k] + lam * (double)lk;   // w(t,k) already inside lk via EFT(t,k)
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
    bool single = argc > 1 && string(argv[1]) == "slas";
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    W.assign(n, vector<int>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    succ.assign(n, {}); pred.assign(n, {});
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        int c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s); } }
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }
    // IOCT table
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
    // HEFT upward rank and priority positions
    vector<double> rnk(n, 0);
    for (int i = n - 1; i >= 0; i--) {
        int t = topo[i];
        double wbar = 0; for (int k = 0; k < p; k++) wbar += W[t][k]; wbar /= p;
        double mx = 0; for (auto [c, cc] : succ[t]) mx = max(mx, cc + rnk[c]);
        rnk[t] = wbar + mx;
    }
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rnk[a] > rnk[b]; });
    pos.assign(n, 0); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    // critical children: top R by mean IOCT
    crit.assign(n, {});
    vector<double> mio(n, 0);
    for (int t = 0; t < n; t++) { for (int q = 0; q < p; q++) mio[t] += ioct[t][q]; }
    for (int t = 0; t < n; t++) {
        vector<pair<int,int>> ch = succ[t];
        stable_sort(ch.begin(), ch.end(), [&](const pair<int,int> &a, const pair<int,int> &b) {
            return mio[a.first] > mio[b.first]; });
        if ((int)ch.size() > R) ch.resize(R);
        crit[t] = ch;
    }
    Sched S = build(false, LAMBDA_RANK);
    if (!single) { Sched B = build(true, LAMBDA_EFT); if (B.makespan < S.makespan) S = B; }
    for (int i = 0; i < n; i++) cout << S.order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << S.ST[i] << "\t" << S.FT[i] << "\t" << S.PR[i] + 1 << "\n";
}
