// lab5.cpp - OPPTS family with task duplication into idle gaps (round-6 prototype).
// Base (from lab4 round 5b): HEFT upward rank, IOCT-style table (no repeated child cost, gamma = 0),
// selection EFT + beta * T(t,k), insertion policy. Static tables only; one schedule.
//
// Duplication (dup=D, default 0 = off): when task t is evaluated on processor k, the parent whose data
// arrives last on k is copied onto k if the copy fits in an idle gap (insertion) and finishes before
// that parent's data would otherwise arrive; repeated for up to D parents per (t, k). A copy's own
// inputs come from the existing copies of its parents (no recursive duplication).
// dupsel=eft  keep the duplication on k only if it lowers EFT(t,k) (greedy, default)
// dupsel=crit duplicate only for tasks whose rank is at least the mean rank of their level (critical)
//
// Output: "TASK ST FT PRO" rows for every task (primary copy) and "DUP task ST FT PRO" rows for copies.
// With check=1 the program validates the schedule itself (precedence through any copy, no overlap) and
// prints "MAKESPAN m VALID v COPIES c" to stderr.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct Copy { int proc; ll st, ft; };

int n, p;
vector<vector<ll>> W;
vector<vector<pair<int,ll>>> succ, pred;
vector<vector<Copy>> cp;                      // all copies of each task (cp[t][0] = primary)
vector<vector<pair<ll,ll>>> busy;             // per processor, sorted busy intervals

static ll arrival(int u, ll c, int k) {        // earliest time u's output is on k
    ll a = LLONG_MAX;
    for (auto &x : cp[u]) a = min(a, x.proc == k ? x.ft : x.ft + c);
    return a;
}
static ll gapStart(const vector<pair<ll,ll>> &b, ll ready, ll len) {
    ll cur = ready;
    for (auto &iv : b) { if (iv.first >= cur + len) return cur; cur = max(cur, iv.second); }
    return cur;
}
static void insertBusy(vector<pair<ll,ll>> &b, ll s, ll f) {
    b.insert(lower_bound(b.begin(), b.end(), make_pair(s, f)), {s, f});
}

int main(int argc, char **argv)
{
    double beta = 1; int D = 0; bool check = false; string dupsel = "eft";
    for (int i = 1; i < argc; i++) {
        string a = argv[i], v = a.substr(a.find('=') + 1);
        if (a.rfind("beta=", 0) == 0) beta = atof(v.c_str());
        else if (a.rfind("dup=", 0) == 0) D = atoi(v.c_str());
        else if (a.rfind("check=", 0) == 0) check = v == "1";
        else if (a.rfind("dupsel=", 0) == 0) dupsel = v;
        else { fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
    }
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if (!(cin >> n >> p) || n == 0 || p == 0) { cout << 0 << "\n"; return 0; }
    W.assign(n, vector<ll>(p));
    for (auto &r : W) for (auto &x : r) cin >> x;
    succ.assign(n, {}); pred.assign(n, {});
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        ll c; cin >> c;
        if (c != -1) { succ[i].push_back({j, c}); pred[j].push_back({i, c}); }
    }
    vector<int> indeg(n), topo;
    for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) q.push_back(s); } }
    // IOCT-style table
    vector<vector<double>> T(n, vector<double>(p));
    for (int x = n - 1; x >= 0; x--) {
        int i = topo[x]; vector<double> mx(p, 0);
        for (auto [c, cc] : succ[i]) {
            double mA = *min_element(T[c].begin(), T[c].end());
            for (int k = 0; k < p; k++) mx[k] = max(mx[k], min(T[c][k], mA + cc));
        }
        for (int k = 0; k < p; k++) T[i][k] = mx[k] + W[i][k];
    }
    // HEFT rank, ready-list order
    vector<double> pr(n, 0);
    for (int x = n - 1; x >= 0; x--) {
        int t = topo[x]; double wb = 0; for (int k = 0; k < p; k++) wb += W[t][k]; wb /= p;
        double mx = 0; for (auto [c, cc] : succ[t]) mx = max(mx, cc + pr[c]);
        pr[t] = wb + mx;
    }
    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return pr[a] > pr[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    vector<int> order;
    { for (int i = 0; i < n; i++) indeg[i] = pred[i].size();
      priority_queue<int, vector<int>, greater<int>> ready;
      for (int i = 0; i < n; i++) if (!indeg[i]) ready.push(pos[i]);
      while (!ready.empty()) { int t = prio[ready.top()]; ready.pop(); order.push_back(t);
        for (auto [s, c] : succ[t]) if (--indeg[s] == 0) ready.push(pos[s]); } }
    vector<char> critical(n, 1);
    if (dupsel == "crit") {   // critical = on a longest path by rank: rank(t) + downward >= 0.99 * max
        vector<double> down(n, 0);
        for (int t : topo) for (auto [s, c] : succ[t]) {
            double wb = 0; for (int k = 0; k < p; k++) wb += W[t][k]; wb /= p;
            down[s] = max(down[s], down[t] + wb + c);
        }
        double cp_len = 0; for (int t = 0; t < n; t++) cp_len = max(cp_len, pr[t] + down[t]);
        for (int t = 0; t < n; t++) critical[t] = pr[t] + down[t] >= 0.99 * cp_len;
    }
    cp.assign(n, {}); busy.assign(p, {});
    int copies = 0;
    for (int t : order) {
        double bestScore = 1e300; int bestK = -1; ll bestST = 0;
        vector<Copy> bestDups;
        for (int k = 0; k < p; k++) {
            // plain placement
            ll drt = 0;
            for (auto [u, c] : pred[t]) drt = max(drt, arrival(u, c, k));
            ll st = gapStart(busy[k], drt, W[t][k]);
            vector<Copy> dups;
            if (D > 0 && !pred[t].empty() && (dupsel != "crit" || critical[t])) {
                vector<pair<ll,ll>> b = busy[k];
                vector<int> dupd;
                for (int r = 0; r < D; r++) {
                    // parent whose data arrives last on k
                    int lu = -1; ll la = -1;
                    for (auto [u, c] : pred[t]) {
                        bool here = false;
                        for (auto &x : cp[u]) if (x.proc == k) here = true;
                        for (int d : dupd) if (d == u) here = true;
                        if (here) continue;
                        ll a = arrival(u, c, k); if (a > la) { la = a; lu = u; }
                    }
                    if (lu < 0) break;
                    ll udrt = 0;
                    for (auto [g, c] : pred[lu]) udrt = max(udrt, arrival(g, c, k));
                    ll ust = gapStart(b, udrt, W[lu][k]), uft = ust + W[lu][k];
                    if (uft >= la) break;                       // the copy would not arrive earlier
                    // new data-ready time of t with this copy
                    vector<pair<ll,ll>> b2 = b; insertBusy(b2, ust, uft);
                    ll drt2 = 0;
                    for (auto [u, c] : pred[t]) {
                        ll a = arrival(u, c, k);
                        if (u == lu) a = min(a, uft);
                        for (auto &dc : dups) if (dc.proc == u) a = min(a, dc.ft);   // proc field reused: task id
                        drt2 = max(drt2, a);
                    }
                    ll st2 = gapStart(b2, drt2, W[t][k]);
                    if (st2 >= st) break;                       // no gain for t
                    b = b2; st = st2; dupd.push_back(lu);
                    dups.push_back({lu, ust, uft});             // proc field holds the task id here
                }
            }
            ll eft = st + W[t][k];
            double score = eft + beta * T[t][k];
            if (score < bestScore - 1e-9) { bestScore = score; bestK = k; bestST = st; bestDups = dups; }
        }
        int k = bestK;
        for (auto &d : bestDups) { cp[d.proc].push_back({k, d.st, d.ft}); insertBusy(busy[k], d.st, d.ft); copies++; }
        cp[t].insert(cp[t].begin(), {k, bestST, bestST + W[t][k]});
        insertBusy(busy[k], bestST, bestST + W[t][k]);
    }
    ll mk = 0; for (int t = 0; t < n; t++) for (auto &x : cp[t]) mk = max(mk, x.ft);
    if (check) {
        bool ok = true;
        for (int t = 0; t < n; t++) for (auto &x : cp[t]) {
            if (x.ft - x.st != W[t][x.proc]) ok = false;
            for (auto [u, c] : pred[t]) if (x.st < arrival(u, c, x.proc)) ok = false;
        }
        for (int k = 0; k < p; k++) {
            vector<pair<ll,ll>> iv;
            for (int t = 0; t < n; t++) for (auto &x : cp[t]) if (x.proc == k) iv.push_back({x.st, x.ft});
            sort(iv.begin(), iv.end());
            for (size_t i = 1; i < iv.size(); i++) if (iv[i].first < iv[i - 1].second) ok = false;
        }
        fprintf(stderr, "MAKESPAN %lld VALID %d COPIES %d\n", mk, ok ? 1 : 0, copies);
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int t = 0; t < n; t++) cout << t + 1 << "\t" << cp[t][0].st << "\t" << cp[t][0].ft << "\t" << cp[t][0].proc + 1 << "\n";
    for (int t = 0; t < n; t++) for (size_t j = 1; j < cp[t].size(); j++)
        cout << "DUP\t" << t + 1 << "\t" << cp[t][j].st << "\t" << cp[t][j].ft << "\t" << cp[t][j].proc + 1 << "\n";
}
