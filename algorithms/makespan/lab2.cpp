// lab2.cpp - second-round configurable list scheduler (complexity-bounded look-ahead + adaptive blend).
//
// Input (d3): n p, the n x p computation matrix, the n x n communication matrix (-1 = no edge).
// Output: a priority line, then "TASK ST FT PRO" rows (1-based task and processor).
//
// Options (argv, any order):
//   tbl=ioct | opcm       table used by the rank and the static score: opcm = OPPTS's table, which adds the
//                         child's w(c,q) once more: OPCM(t,k) = w(t,k) + max_c min_q (OPCM(c,q) + w(c,q) + [q!=k]c)
//                         (the look-ahead always uses IOCT)
//   rank=ioct | heft      ioct: mean_k IOCT(t,k) (AEFT/OPPTS priority); heft: upward rank (mean w + mean c)
//   kids=R                look-ahead over the R children with the largest rank (0 = all children)
//   own=X                 add X * w(t,k) to the look-ahead score
//   switch=0|1            out-degree > p -> plain EFT (AEFT's fan-out rule)
//   lam=const:L           score = L * look + (1-L) * static
//   lam=ccr:a:b           per task, L_t = clamp(a + b * log10(lccr_t), 0, 1), lccr_t = mean out-edge cost / mean w(t)
//   lam=het:a:b           per task, L_t = clamp(a + b * cv_t, 0, 1), cv_t = coefficient of variation of w(t,.)
//   base=static | eft     static = EFT + IOCT(t,k) (OPPTS / AEFT without the fan-out rule); eft = EFT only
//
// IOCT(t,k) = w(t,k) + max_{c in succ(t)} min_q ( IOCT(c,q) + [q != k] c(t,c) )
// Look-ahead estimate for child c when t goes to k with finish time FT_k:
//   L(c | k) = min_q [ max(avail'(q), FT_k + [q != k] c(t,c), DRT_c(q)) + IOCT(c,q) ]
// with avail'(k) = FT_k and DRT_c(q) the data-ready time on q from c's parents already scheduled.
// Complexity: DRT is maintained incrementally, O(e p) in total. Each task evaluates at most R children
// in O(R p^2), i.e. O(v p^2) for constant R. The tables use min_q(X(c,q) + [q!=k]cc) =
// min(X(c,k), min_q X(c,q) + cc), so they cost O(e p). Overall O(e p + v p^2) = O(v^2 p) (p <= v).
// With kids=0 (all children) the selection costs O(e p^2) instead.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static int n, p;
static vector<vector<int>> W;
static vector<vector<pair<int,int>>> succ_, pred_;

int main(int argc, char **argv)
{
    string TBL = "ioct", RANK = "ioct", LAM = "const", BASE = "static";
    double LA = 1, LB = 0, OWN = 1;
    int KIDS = 0; bool SWITCH = false;
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        auto v = a.substr(a.find('=') + 1);
        if (a.rfind("rank=", 0) == 0) RANK = v;
        else if (a.rfind("tbl=", 0) == 0) TBL = v;
        else if (a.rfind("kids=", 0) == 0) KIDS = atoi(v.c_str());
        else if (a.rfind("own=", 0) == 0) OWN = atof(v.c_str());
        else if (a.rfind("switch=", 0) == 0) SWITCH = v == "1";
        else if (a.rfind("base=", 0) == 0) BASE = v;
        else if (a.rfind("lam=", 0) == 0) {
            vector<string> f; string cur; for (char ch : v) { if (ch == ':') { f.push_back(cur); cur.clear(); } else cur += ch; }
            f.push_back(cur);
            LAM = f[0]; if (f.size() > 1) LA = atof(f[1].c_str()); if (f.size() > 2) LB = atof(f[2].c_str());
        } else { fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
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
    vector<int> indeg(n), topo; topo.reserve(n);
    for (int i = 0; i < n; i++) indeg[i] = pred_[i].size();
    { deque<int> q; for (int i = 0; i < n; i++) if (!indeg[i]) q.push_back(i);
      while (!q.empty()) { int t = q.front(); q.pop_front(); topo.push_back(t);
        for (auto [s, c] : succ_[t]) if (--indeg[s] == 0) q.push_back(s); } }
    if ((int)topo.size() != n) { fprintf(stderr, "graph has a cycle\n"); return 3; }

    vector<vector<ll>> ioct(n, vector<ll>(p));
    vector<ll> ioctmin(n);
    for (int idx = n - 1; idx >= 0; idx--) {
        int t = topo[idx];
        for (int k = 0; k < p; k++) {
            ll mx = 0;
            for (auto [c, cc] : succ_[t]) mx = max(mx, min(ioct[c][k], ioctmin[c] + cc));
            ioct[t][k] = mx + W[t][k];
        }
        ioctmin[t] = *min_element(ioct[t].begin(), ioct[t].end());
    }
    vector<vector<ll>> tab = ioct;
    if (TBL == "opcm") {
        vector<ll> mn(n);
        for (int idx = n - 1; idx >= 0; idx--) {
            int t = topo[idx];
            for (int k = 0; k < p; k++) {
                ll mx = 0;
                for (auto [c, cc] : succ_[t]) mx = max(mx, min(tab[c][k] + W[c][k], mn[c] + cc));
                tab[t][k] = mx + W[t][k];
            }
            mn[t] = LLONG_MAX; for (int q = 0; q < p; q++) mn[t] = min(mn[t], tab[t][q] + W[t][q]);
        }
    } else if (TBL != "ioct") { fprintf(stderr, "bad tbl\n"); return 2; }
    vector<double> wbar(n), rank(n);
    for (int t = 0; t < n; t++) { double s = 0; for (int k = 0; k < p; k++) s += W[t][k]; wbar[t] = s / p; }
    if (RANK == "ioct") {
        for (int t = 0; t < n; t++) { double s = 0; for (int k = 0; k < p; k++) s += tab[t][k]; rank[t] = s / p; }
    } else if (RANK == "heft") {
        for (int idx = n - 1; idx >= 0; idx--) {
            int t = topo[idx]; double mx = 0;
            for (auto [c, cc] : succ_[t]) mx = max(mx, cc + rank[c]);
            rank[t] = wbar[t] + mx;
        }
    } else { fprintf(stderr, "bad rank\n"); return 2; }

    vector<int> prio(n); iota(prio.begin(), prio.end(), 0);
    stable_sort(prio.begin(), prio.end(), [&](int a, int b) { return rank[a] > rank[b]; });
    vector<int> pos(n); for (int i = 0; i < n; i++) pos[prio[i]] = i;
    vector<int> order; order.reserve(n);
    { for (int i = 0; i < n; i++) indeg[i] = pred_[i].size();
      priority_queue<int, vector<int>, greater<int>> rq;
      for (int i = 0; i < n; i++) if (!indeg[i]) rq.push(pos[i]);
      while (!rq.empty()) { int t = prio[rq.top()]; rq.pop(); order.push_back(t);
        for (auto [s, c] : succ_[t]) if (--indeg[s] == 0) rq.push(pos[s]); } }

    // per-task look-ahead children: top-R by mean IOCT (ties: smaller index)
    vector<vector<pair<int,int>>> kids(n);
    for (int t = 0; t < n; t++) {
        kids[t] = succ_[t];
        auto key = [&](const pair<int,int> &e) { double s = 0; for (int q = 0; q < p; q++) s += ioct[e.first][q]; return s; };
        vector<double> kv; for (auto &e : kids[t]) kv.push_back(key(e));
        vector<int> id(kids[t].size()); iota(id.begin(), id.end(), 0);
        stable_sort(id.begin(), id.end(), [&](int a, int b) { return kv[a] > kv[b]; });
        vector<pair<int,int>> s; for (int i : id) s.push_back(kids[t][i]);
        if (KIDS > 0 && (int)s.size() > KIDS) s.resize(KIDS);
        kids[t] = s;
    }
    // per-task blend weight
    vector<double> lam(n, LA);
    for (int t = 0; t < n; t++) {
        double L = LA;
        if (LAM == "ccr") {
            double cs = 0; for (auto [c, cc] : succ_[t]) cs += cc;
            double lccr = succ_[t].empty() ? 1 : (cs / succ_[t].size()) / max(1.0, wbar[t]);
            L = LA + LB * log10(max(1e-3, lccr));
        } else if (LAM == "het") {
            double v = 0; for (int k = 0; k < p; k++) v += (W[t][k] - wbar[t]) * (W[t][k] - wbar[t]);
            double cv = sqrt(v / p) / max(1.0, wbar[t]);
            L = LA + LB * cv;
        } else if (LAM != "const") { fprintf(stderr, "bad lam\n"); return 2; }
        lam[t] = min(1.0, max(0.0, L));
    }

    vector<ll> ST(n, 0), FT(n, 0), avail(p, 0);
    vector<int> PR(n, -1);
    vector<vector<ll>> drt(n, vector<ll>(p, 0)); // data-ready time from scheduled parents
    vector<ll> ftk(p);
    vector<double> score(p);
    for (int t : order) {
        for (int k = 0; k < p; k++) ftk[k] = max(avail[k], drt[t][k]) + W[t][k];
        int best = 0;
        bool plain = succ_[t].empty() || (SWITCH && (int)succ_[t].size() > p);
        if (plain) {
            for (int k = 1; k < p; k++) if (ftk[k] < ftk[best]) best = k;
        } else {
            vector<ll> look(p, 0);
            if (lam[t] > 0) for (auto [c, tc] : kids[t]) {
                for (int k = 0; k < p; k++) {
                    ll L = LLONG_MAX;
                    for (int q = 0; q < p; q++) {
                        ll av = (q == k) ? ftk[k] : avail[q];
                        ll s = max(max(av, ftk[k] + (q == k ? 0 : tc)), drt[c][q]);
                        L = min(L, s + ioct[c][q]);
                    }
                    look[k] = max(look[k], L);
                }
            }
            for (int k = 0; k < p; k++) {
                double lk = look[k] + OWN * W[t][k];
                double st = BASE == "eft" ? ftk[k] : ftk[k] + tab[t][k];
                score[k] = lam[t] * lk + (1 - lam[t]) * st;
            }
            for (int k = 1; k < p; k++)
                if (score[k] < score[best] - 1e-9 || (fabs(score[k] - score[best]) <= 1e-9 && ftk[k] < ftk[best])) best = k;
        }
        PR[t] = best; FT[t] = ftk[best]; ST[t] = FT[t] - W[t][best]; avail[best] = FT[t];
        for (auto [c, cc] : succ_[t])
            for (int q = 0; q < p; q++) drt[c][q] = max(drt[c][q], FT[t] + (q == best ? 0 : cc));
    }
    for (int i = 0; i < n; i++) cout << order[i] + 1 << (i + 1 < n ? " " : "\n");
    cout << "TASK\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++) cout << i + 1 << "\t" << ST[i] << "\t" << FT[i] << "\t" << PR[i] + 1 << "\n";
}
