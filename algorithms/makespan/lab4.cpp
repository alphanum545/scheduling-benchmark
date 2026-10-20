// lab4.cpp - OPPTS family: static look-ahead table + priority + processor selection (round-5 experiments).
// Everything stays static (tables computed once before scheduling); no state-aware look-ahead and
// no multiple schedules, so the method remains distinct from DSLA.
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: the scheduling order, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Table (tbl=):
//   opcm  OPCM(i,k) = w(i,k) + max_c min_q [OPCM(c,q) + w(c,q) + [q != k] c(i,c)]   (OPPTS, default)
//   pcm   PCM-style without re-adding the child's cost: w(i,k) + max_c min_q [T(c,q) + [q != k] c(i,c)]
//   gamma=<g> overrides both: w(i,k) + max_c min_q [T(c,q) + g * w(c,q) + [q != k] c(i,c)]
//         (g = 0 is the IOCT / PCM-style table, g = 1 is OPCM)
// Priority (rank=):
//   mean  mean_k T(i,k) (OPPTS, default)
//   cv    mean_k T(i,k) * (1 + cv_i), cv_i = std/mean of w(i,.) (heterogeneity-aware)
//   heft  HEFT upward rank
// Selection score (sel=):  EFT(t,k) + beta * L(t,k), beta given by beta= (default 1)
//   ieft  L = T(t,k)                (OPPTS: counts the task's own cost twice)
//   net   L = T(t,k) - w(t,k)       (downstream part only)
// exit=1   exit tasks are placed by EFT alone (the table carries no downstream information for them)
// fan=1    AEFT's rule: tasks with out-degree > p are placed by EFT alone
// ins=1   insertion policy: a task may start in an idle gap of the processor (HEFT's insertion rule)
// Ties on the score: smaller EFT, then lower processor index (OPPTS: first strictly smaller score).
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int main(int argc, char **argv)
{
    string tbl = "opcm", rk = "mean", sel = "ieft"; double beta = 1, gam = -1; bool exitEft = false, fan = false, tie = false, ins = false;
    for (int i = 1; i < argc; i++) {
        string a = argv[i], v = a.substr(a.find('=') + 1);
        if (a.rfind("tbl=", 0) == 0) tbl = v;
        else if (a.rfind("rank=", 0) == 0) rk = v;
        else if (a.rfind("sel=", 0) == 0) sel = v;
        else if (a.rfind("beta=", 0) == 0) beta = atof(v.c_str());
        else if (a.rfind("gamma=", 0) == 0) gam = atof(v.c_str());
        else if (a.rfind("exit=", 0) == 0) exitEft = v == "1";
        else if (a.rfind("fan=", 0) == 0) fan = v == "1";
        else if (a.rfind("tie=", 0) == 0) tie = v == "1";
        else if (a.rfind("ins=", 0) == 0) ins = v == "1";
        else { fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
    }
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, p;
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    vector<vector<ll>> W(n, vector<ll>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    vector<vector<pair<int,ll>>> succ(n), pred(n);
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
    // table, O(e p) with the min trick
    if (gam < 0) gam = (tbl == "opcm") ? 1 : 0;
    vector<vector<double>> T(n, vector<double>(p));
    vector<double> A(p);
    for (int x = n - 1; x >= 0; x--) {
        int i = topo[x];
        vector<double> mx(p, 0);
        for (auto [c, cc] : succ[i]) {
            double mA = 1e300;
            for (int q = 0; q < p; q++) { A[q] = T[c][q] + gam * W[c][q]; mA = min(mA, A[q]); }
            for (int k = 0; k < p; k++) mx[k] = max(mx[k], min(A[k], mA + cc));
        }
        for (int k = 0; k < p; k++) T[i][k] = mx[k] + W[i][k];
    }
    // priority
    vector<double> pr(n, 0);
    if (rk == "heft") {
        for (int x = n - 1; x >= 0; x--) {
            int t = topo[x]; double wb = 0; for (int k = 0; k < p; k++) wb += W[t][k]; wb /= p;
            double mx = 0; for (auto [c, cc] : succ[t]) mx = max(mx, cc + pr[c]);
            pr[t] = wb + mx;
        }
    } else {
        for (int t = 0; t < n; t++) {
            double s = 0; for (int k = 0; k < p; k++) s += T[t][k];
            pr[t] = s / p;
            if (rk == "cv") {
                double m = 0, v = 0; for (int k = 0; k < p; k++) m += W[t][k]; m /= p;
                for (int k = 0; k < p; k++) v += (W[t][k] - m) * (W[t][k] - m); v /= p;
                pr[t] *= 1 + (m > 0 ? sqrt(v) / m : 0);
            }
        }
    }
    // OPPTS order: descending priority, ties to the lower index; then highest-priority ready task
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return pr[a] > pr[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    vector<int> order;
    { for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
      priority_queue<int, vector<int>, greater<int>> ready;
      for (int i = 0; i < n; i++) if (!indeg[i]) ready.push(pos[i]);
      while (!ready.empty()) { int t = prio[ready.top()]; ready.pop(); order.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) ready.push(pos[s]); } }
    // selection
    vector<ll> ST(n), FT(n), avail(p, 0), eft(p);
    vector<int> PR(n, -1);
    vector<vector<pair<ll,ll>>> busy(p);          // sorted [start, finish) per processor (insertion policy)
    for (int t : order) {
        for (int k = 0; k < p; k++) {
            ll drt = 0;
            for (auto [u, cc] : pred[t]) drt = max(drt, FT[u] + (PR[u] == k ? 0 : cc));
            ll est = max(drt, avail[k]);
            if (ins) {
                ll cur = drt; est = -1;
                for (auto &b : busy[k]) {
                    if (b.first >= cur + W[t][k]) { est = cur; break; }
                    cur = max(cur, b.second);
                }
                if (est < 0) est = cur;
            }
            eft[k] = est + W[t][k];
        }
        bool plain = (exitEft && succ[t].empty()) || (fan && (int)succ[t].size() > p);
        int best = 0; double bs = 1e300;
        for (int k = 0; k < p; k++) {
            double L = plain ? 0 : (sel == "net" ? T[t][k] - W[t][k] : T[t][k]);
            double s = eft[k] + beta * L;
            if (s < bs - 1e-9 || (tie && fabs(s - bs) <= 1e-9 && eft[k] < eft[best])) { bs = s; best = k; }
        }
        PR[t] = best; FT[t] = eft[best]; ST[t] = FT[t] - W[t][best]; avail[best] = max(avail[best], FT[t]);
        if (ins) busy[best].insert(lower_bound(busy[best].begin(), busy[best].end(), make_pair(ST[t], FT[t])), {ST[t], FT[t]});
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
