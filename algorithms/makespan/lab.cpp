// lab.cpp - configurable list scheduler used to evaluate the candidate methods.
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: a priority line, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Options (argv, any order):
//   rank=mean        rank = mean over processors of IOCT (AEFT's priority)
//   rank=regret:A    rank = mean IOCT + A * (second-smallest IOCT - smallest IOCT)        [M1]
//   sel=aeft         AEFT selection: min EFT + IOCT, plain EFT when out-degree > p      [baseline]
//   sel=look1        state-aware look-ahead over the critical child only                [M3]
//   sel=lookall      state-aware look-ahead over all children (max)                     [M2]
//   sel=oct          min EFT + OCT (static, no double counting of w(t,k))
//   own=X            look modes: add X * w(t,k) to the score
//   switch=0|1       keep AEFT's "out-degree > p -> plain EFT" rule in look-ahead modes (default 0)
//
// IOCT(t,k) = w(t,k) + max_{c in succ(t)} min_q ( IOCT(c,q) + [q != k] * c(t,c) )   (AEFT).
// OCT(c,q)  = IOCT(c,q) - w(c,q)  (optimistic remaining path after c finishes on q).
//
// State-aware look-ahead: when placing t on processor k with finish time FT, the estimated finish
// of the remaining path through child c is
//   L(c | t->k) = min_q [ max(avail'(q), FT + [q != k] c(t,c), DRT_c(q)) + w(c,q) + OCT(c,q) ]
// where avail' is the current processor availability with avail'(k) = FT, and DRT_c(q) is the data
// arrival time on q from c's parents that are already scheduled. t goes to the processor that
// minimises L over the critical child (look1) or the maximum of L over all children (lookall);
// ties are broken by the smaller FT. Exit tasks use FT. Non-insertion policy throughout.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static int n, p;
static vector<vector<int>> W;                 // W[t][k]
static vector<vector<pair<int,int>>> succ_, pred_; // (task, comm cost)

static string RANK = "mean", SEL = "aeft";
static double ALPHA = 1.0;
static bool SWITCH = false;
static double OWN = 0; // look modes: add OWN * w(t,k) to the score (AEFT counts w(t,k) twice)

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        auto v = a.substr(a.find('=') + 1);
        if (a.rfind("rank=", 0) == 0) {
            auto c = v.find(':');
            RANK = v.substr(0, c);
            if (c != string::npos) ALPHA = atof(v.substr(c + 1).c_str());
        } else if (a.rfind("sel=", 0) == 0) SEL = v;
        else if (a.rfind("switch=", 0) == 0) SWITCH = v == "1";
        else if (a.rfind("own=", 0) == 0) OWN = atof(v.c_str());
        else { fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
    }
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    W.assign(n, vector<int>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    succ_.assign(n, {}); pred_.assign(n, {});
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        int c; cin >> c;
        if (c != -1) { succ_[i].push_back({j, c}); pred_[j].push_back({i, c}); }
    }
    // topological order (Kahn)
    vector<int> indeg(n), topo; topo.reserve(n);
    for (int i = 0; i < n; i++) indeg[i] = pred_[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ_[t]) if (--indeg[s] == 0) q.push_back(s); } }
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }

    // IOCT, reverse topological order
    vector<vector<ll>> ioct(n, vector<ll>(p));
    for (int idx = n - 1; idx >= 0; idx--) {
        int t = topo[idx];
        for (int k = 0; k < p; k++) {
            ll mx = 0;
            for (auto [c, cc] : succ_[t]) {
                ll mn = LLONG_MAX;
                for (int q = 0; q < p; q++) mn = min(mn, ioct[c][q] + (q == k ? 0 : cc));
                mx = max(mx, mn);
            }
            ioct[t][k] = mx + W[t][k];
        }
    }
    // rank
    vector<double> rank(n);
    for (int t = 0; t < n; t++) {
        double s = 0; ll m1 = LLONG_MAX, m2 = LLONG_MAX;
        for (int k = 0; k < p; k++) {
            s += ioct[t][k];
            if (ioct[t][k] < m1) { m2 = m1; m1 = ioct[t][k]; } else if (ioct[t][k] < m2) m2 = ioct[t][k];
        }
        rank[t] = s / p;
        if (RANK == "regret" && p > 1) rank[t] += ALPHA * (double)(m2 - m1);
        else if (RANK != "mean" && RANK != "regret") { fprintf(stderr, "bad rank\n"); return 2; }
    }
    // priority list (rank desc, index asc) then ready-list order
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rank[a] > rank[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    vector<int> order; order.reserve(n);
    { for (int i = 0; i < n; i++) indeg[i] = pred_[i].size();
      priority_queue<int, vector<int>, greater<int>> rq;
      for (int i = 0; i < n; i++) if (!indeg[i]) rq.push(pos[i]);
      while (!rq.empty()) { int t = prio[rq.top()]; rq.pop(); order.push_back(t);
        for (auto [s, c] : succ_[t]) if (--indeg[s] == 0) rq.push(pos[s]); } }

    // critical child = child with the largest rank (ties: smaller index)
    vector<int> crit(n, -1);
    for (int t = 0; t < n; t++)
        for (auto [c, cc] : succ_[t]) if (crit[t] < 0 || rank[c] > rank[crit[t]]) crit[t] = c;

    vector<ll> ST(n, 0), FT(n, 0), avail(p, 0);
    vector<int> PR(n, -1);
    vector<ll> ftk(p), drt(p);
    for (int t : order) {
        // finish time of t on each processor (non-insertion)
        for (int k = 0; k < p; k++) {
            ll est = avail[k];
            for (auto [u, cc] : pred_[t]) est = max(est, FT[u] + (PR[u] == k ? 0 : cc));
            ftk[k] = est + W[t][k];
        }
        int best = 0;
        bool plain = (SEL == "aeft" && (int)succ_[t].size() > p) ||
                     (SEL != "aeft" && ((SWITCH && (int)succ_[t].size() > p) || succ_[t].empty()));
        if (plain) {
            for (int k = 1; k < p; k++) if (ftk[k] < ftk[best]) best = k;
        } else if (SEL == "aeft" || SEL == "oct") {
            ll bs = LLONG_MAX;
            for (int k = 0; k < p; k++) {
                ll s = ftk[k] + ioct[t][k] - (SEL == "oct" ? W[t][k] : 0);
                if (s < bs) { bs = s; best = k; } }
        } else if (SEL == "look1" || SEL == "lookall") {
            vector<int> kids;
            if (SEL == "look1") kids.push_back(crit[t]); else for (auto [c, cc] : succ_[t]) kids.push_back(c);
            vector<ll> score(p, 0);
            for (int c : kids) {
                ll tc = 0; for (auto [cc_, w] : succ_[t]) if (cc_ == c) { tc = w; break; }
                // data arrival on each q from c's already-scheduled parents (other than t)
                for (int q = 0; q < p; q++) {
                    ll d = 0;
                    for (auto [u, cu] : pred_[c]) if (u != t && PR[u] >= 0) d = max(d, FT[u] + (PR[u] == q ? 0 : cu));
                    drt[q] = d;
                }
                for (int k = 0; k < p; k++) {
                    ll L = LLONG_MAX;
                    for (int q = 0; q < p; q++) {
                        ll av = (q == k) ? ftk[k] : avail[q];
                        ll s = max(max(av, ftk[k] + (q == k ? 0 : tc)), drt[q]);
                        L = min(L, s + ioct[c][q]); // w(c,q) + OCT(c,q) = IOCT(c,q)
                    }
                    score[k] = max(score[k], L);
                }
            }
            if (OWN != 0) for (int k = 0; k < p; k++) score[k] += (ll)llround(OWN * W[t][k]);
            for (int k = 1; k < p; k++)
                if (score[k] < score[best] || (score[k] == score[best] && ftk[k] < ftk[best])) best = k;
        } else { fprintf(stderr, "bad sel\n"); return 2; }
        PR[t] = best; FT[t] = ftk[best]; ST[t] = FT[t] - W[t][best]; avail[best] = FT[t];
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
