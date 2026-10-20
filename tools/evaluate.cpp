// evaluate.cpp — instance preparation and schedule validation for the benchmark.
//
//   evaluate strip <raw_instance> <out_d3|-> [out_d2]
//       Detects Format D1 / D2 / D3 by exact token count. Writes the stripped
//       Format D3 file (n p, compc, commc) unless "-" is given, optionally a
//       Format D2 file (n p, 5 parameter vectors, compc, commc; D1's extra first
//       vector dropped), and prints instance metadata as JSON:
//       {"n":..,"p":..,"format":"D1","lb":..,"seq":..,"edges":..}
//         lb  = critical path using each task's minimum cost (SLR denominator)
//         seq = min over processors of the sum of all task costs (speedup numerator)
//
//   evaluate check <d3_instance> <algorithm_stdout> [--dup-entry]
//       Parses the schedule table printed by an algorithm and validates it.
//       Prints JSON: {"makespan":..,"valid":0/1,"violations":..,"first":"..."}; for a valid schedule
//       also secondary metrics:
//         comm_ratio      communication cost paid (edges across processors) / total communication cost
//         imbalance       max processor busy time / mean processor busy time
//         utilization     total busy time / (p * makespan)
//         rob_ratio       mean makespan over 30 replays with every computation and communication cost
//                         scaled by an independent factor in U[0.8, 1.2], divided by the makespan.
//                         The replay keeps the processor mapping and each processor's task order;
//                         the random factors depend only on the instance, so all algorithms see
//                         the same perturbations (paired comparison).
//         rob_cv          coefficient of variation of the replayed makespans
//
// Build: g++ -O2 -std=c++17 -o evaluate evaluate.cpp
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
using namespace std;
typedef long long ll;

static vector<string> readTokens(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path); exit(2); }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    string buf(sz, '\0');
    if (sz && fread(&buf[0], 1, sz, f) != (size_t)sz) { fprintf(stderr, "read error\n"); exit(2); }
    fclose(f);
    vector<string> t; t.reserve(sz / 2);
    size_t i = 0;
    while (i < buf.size()) {
        while (i < buf.size() && isspace((unsigned char)buf[i])) i++;
        size_t j = i;
        while (j < buf.size() && !isspace((unsigned char)buf[j])) j++;
        if (j > i) t.emplace_back(buf, i, j - i);
        i = j;
    }
    return t;
}

struct Inst {
    int n = 0, p = 0;
    vector<vector<ll>> comp;          // n x p
    vector<vector<pair<int, ll>>> succ, pred;
    int edges = 0;
};

static void buildGraph(Inst &I, const vector<string> &t, size_t off) {
    int n = I.n, p = I.p;
    I.comp.assign(n, vector<ll>(p));
    for (int i = 0; i < n; i++) for (int j = 0; j < p; j++) I.comp[i][j] = atoll(t[off + (size_t)i * p + j].c_str());
    off += (size_t)n * p;
    I.succ.assign(n, {}); I.pred.assign(n, {});
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        const string &s = t[off + (size_t)i * n + j];
        if (s != "-1") { ll c = atoll(s.c_str()); I.succ[i].push_back({j, c}); I.pred[j].push_back({i, c}); I.edges++; }
    }
}

static vector<int> topo(const Inst &I) {
    vector<int> indeg(I.n), q; for (int i = 0; i < I.n; i++) indeg[i] = I.pred[i].size();
    for (int i = 0; i < I.n; i++) if (!indeg[i]) q.push_back(i);
    for (size_t k = 0; k < q.size(); k++) for (auto &e : I.succ[q[k]]) if (--indeg[e.first] == 0) q.push_back(e.first);
    return q;
}

static int cmdStrip(const char *in, const char *out, const char *out2) {
    vector<string> t = readTokens(in);
    if (t.size() < 2) { fprintf(stderr, "empty instance\n"); return 2; }
    Inst I; I.n = atoi(t[0].c_str()); I.p = atoi(t[1].c_str());
    if (I.n <= 0) { fprintf(stderr, "%s: instance has %d tasks (empty or truncated)\n", in, I.n); return 6; }
    if (I.p <= 0) { fprintf(stderr, "%s: instance has %d processors\n", in, I.p); return 6; }
    size_t body = (size_t)I.n * I.p + (size_t)I.n * I.n, hdr = 0; const char *fmt = nullptr;
    if (t.size() == 2 + 6 * (size_t)I.p + body) { hdr = 6 * I.p; fmt = "D1"; }
    else if (t.size() == 2 + 5 * (size_t)I.p + body) { hdr = 5 * I.p; fmt = "D2"; }
    else if (t.size() == 2 + body) { hdr = 0; fmt = "D3"; }
    else { fprintf(stderr, "%s: token count %zu matches no format\n", in, t.size()); return 3; }
    buildGraph(I, t, 2 + hdr);
    auto writeOut = [&](const char *path, bool withParams) {
        FILE *f = fopen(path, "wb");
        if (!f) { fprintf(stderr, "cannot write %s\n", path); exit(2); }
        string line;
        fprintf(f, "%d %d\n", I.n, I.p);
        if (withParams) {
            if (!strcmp(fmt, "D3")) { fprintf(stderr, "%s: D3 file has no parameter vectors\n", in); exit(5); }
            size_t h0 = 2 + hdr - 5 * (size_t)I.p;  // last 5 vectors (drops D1's extra first vector)
            for (int r = 0; r < 5; r++) { line.clear(); for (int j = 0; j < I.p; j++) { if (j) line += ' '; line += t[h0 + (size_t)r * I.p + j]; } line += '\n'; fwrite(line.data(), 1, line.size(), f); }
        }
        size_t off = 2 + hdr;
        for (int i = 0; i < I.n; i++) { line.clear(); for (int j = 0; j < I.p; j++) { if (j) line += ' '; line += t[off + (size_t)i * I.p + j]; } line += '\n'; fwrite(line.data(), 1, line.size(), f); }
        off += (size_t)I.n * I.p;
        for (int i = 0; i < I.n; i++) { line.clear(); for (int j = 0; j < I.n; j++) { if (j) line += ' '; line += t[off + (size_t)i * I.n + j]; } line += '\n'; fwrite(line.data(), 1, line.size(), f); }
        fclose(f);
    };
    if (strcmp(out, "-")) writeOut(out, false);
    if (out2) writeOut(out2, true);
    // metadata
    vector<int> order = topo(I);
    if ((int)order.size() != I.n) { fprintf(stderr, "%s: graph has a cycle\n", in); return 4; }
    vector<ll> best(I.n, 0); ll lb = 0;
    for (int k = I.n - 1; k >= 0; k--) {
        int u = order[k]; ll m = 0;
        for (auto &e : I.succ[u]) m = max(m, best[e.first]);
        best[u] = *min_element(I.comp[u].begin(), I.comp[u].end()) + m;
        lb = max(lb, best[u]);
    }
    ll seq = -1;
    for (int j = 0; j < I.p; j++) { ll s = 0; for (int i = 0; i < I.n; i++) s += I.comp[i][j]; if (seq < 0 || s < seq) seq = s; }
    printf("{\"n\":%d,\"p\":%d,\"format\":\"%s\",\"lb\":%lld,\"seq\":%lld,\"edges\":%d}\n", I.n, I.p, fmt, lb, seq, I.edges);
    return 0;
}

static string esc(const string &s) { string r; for (char c : s) { if (c == '"' || c == '\\') r += '\\'; r += c; } return r; }

static int cmdCheck(const char *inst, const char *sched, bool dupEntry) {
    vector<string> t = readTokens(inst);
    Inst I; I.n = atoi(t[0].c_str()); I.p = atoi(t[1].c_str());
    if (t.size() != 2 + (size_t)I.n * I.p + (size_t)I.n * I.n) { fprintf(stderr, "check needs a D3 instance\n"); return 2; }
    buildGraph(I, t, 2);
    // parse schedule: rows of 4 integers after the first line containing a letter
    ifstream in(sched); string line; bool header = false;
    vector<ll> st(I.n, -1), ft(I.n, -1); vector<int> pr(I.n, -1);
    int rows = 0;
    while (getline(in, line)) {
        if (!header) { for (char c : line) if (isalpha((unsigned char)c)) { header = true; break; } continue; }
        istringstream ss(line); ll a, b, c, d;
        if (!(ss >> a >> b >> c >> d)) continue;
        if (a < 1 || a > I.n) continue;
        st[a - 1] = b; ft[a - 1] = c; pr[a - 1] = (int)d - 1; rows++;
    }
    int nv = 0; string first;
    auto bad = [&](const string &m) { if (!nv) first = m; nv++; };
    if (!header) bad("no schedule table found in output");
    ll mk = 0;
    for (int i = 0; i < I.n; i++) {
        if (pr[i] < 0 || pr[i] >= I.p) { bad("task " + to_string(i + 1) + " missing or bad processor"); continue; }
        mk = max(mk, ft[i]);
        if (ft[i] - st[i] != I.comp[i][pr[i]]) bad("task " + to_string(i + 1) + " duration " + to_string(ft[i] - st[i]) + " != cost " + to_string(I.comp[i][pr[i]]));
        if (st[i] < 0) bad("task " + to_string(i + 1) + " negative start");
    }
    if (!nv) {
        for (int v = 0; v < I.n; v++) for (auto &e : I.pred[v]) {
            int u = e.first; ll need = (pr[u] == pr[v]) ? ft[u] : ft[u] + e.second;
            if (dupEntry && I.pred[u].empty()) need = min(need, I.comp[u][pr[v]]);
            if (st[v] < need) bad("t" + to_string(u + 1) + "->t" + to_string(v + 1) + " starts " + to_string(st[v]) + " before data ready " + to_string(need));
        }
        vector<vector<pair<ll, ll>>> slots(I.p);
        for (int i = 0; i < I.n; i++) if (ft[i] > st[i]) slots[pr[i]].push_back({st[i], ft[i]});
        for (int j = 0; j < I.p; j++) {
            sort(slots[j].begin(), slots[j].end());
            for (size_t k = 1; k < slots[j].size(); k++) if (slots[j][k].first < slots[j][k - 1].second) bad("overlap on processor " + to_string(j + 1));
        }
    }
    if (nv || mk <= 0) {
        printf("{\"makespan\":%lld,\"valid\":%d,\"violations\":%d,\"first\":\"%s\"}\n", mk, nv ? 0 : 1, nv, esc(first).c_str());
        return 0;
    }
    // ---- secondary metrics (valid schedules only)
    double commAll = 0, commPaid = 0;
    for (int v = 0; v < I.n; v++) for (auto &e : I.pred[v]) {
        commAll += e.second;
        if (pr[e.first] != pr[v] && !(dupEntry && I.pred[e.first].empty())) commPaid += e.second;
    }
    vector<double> busy(I.p, 0);
    for (int i = 0; i < I.n; i++) busy[pr[i]] += ft[i] - st[i];
    double tot = 0, mx = 0; for (double b : busy) { tot += b; mx = max(mx, b); }
    double imbalance = tot > 0 ? mx / (tot / I.p) : 1, util = tot / ((double)I.p * mk);
    // robustness replay: per-processor order by start time; tasks in topological order of
    // (precedence + processor order); perturbed costs drawn per task and per edge in a fixed order
    vector<vector<int>> onp(I.p);
    for (int i = 0; i < I.n; i++) onp[pr[i]].push_back(i);
    for (auto &q : onp) sort(q.begin(), q.end(), [&](int a, int b) { return st[a] < st[b] || (st[a] == st[b] && a < b); });
    vector<int> prevOn(I.n, -1);
    for (auto &q : onp) for (size_t k = 1; k < q.size(); k++) prevOn[q[k]] = q[k - 1];
    vector<int> ord(I.n); for (int i = 0; i < I.n; i++) ord[i] = i;
    sort(ord.begin(), ord.end(), [&](int a, int b) { return st[a] < st[b] || (st[a] == st[b] && ft[a] < ft[b]) || (st[a] == st[b] && ft[a] == ft[b] && a < b); });
    const int RUNS = 30; vector<double> mks;
    unsigned long long seed = 1469598103934665603ULL; // instance-dependent seed (FNV over n, p, edges)
    for (unsigned long long x : {(unsigned long long)I.n, (unsigned long long)I.p, (unsigned long long)I.edges}) { seed ^= x; seed *= 1099511628211ULL; }
    unsigned long long rs = seed;
    auto rnd = [&]() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (double)(rs >> 11) / 9007199254740992.0; };
    for (int r = 0; r < RUNS; r++) {
        vector<double> w(I.n); for (int i = 0; i < I.n; i++) w[i] = I.comp[i][pr[i]] * (0.8 + 0.4 * rnd());
        vector<vector<double>> cm(I.n);
        for (int v = 0; v < I.n; v++) for (auto &e : I.pred[v]) cm[v].push_back(e.second * (0.8 + 0.4 * rnd()));
        vector<double> s2(I.n, 0), f2(I.n, 0);
        for (int v : ord) {
            double t0 = prevOn[v] >= 0 ? f2[prevOn[v]] : 0;
            for (size_t k = 0; k < I.pred[v].size(); k++) {
                int u = I.pred[v][k].first;
                double need = (pr[u] == pr[v]) ? f2[u] : f2[u] + cm[v][k];
                if (dupEntry && I.pred[u].empty()) need = min(need, w[u]);
                t0 = max(t0, need);
            }
            s2[v] = t0; f2[v] = t0 + w[v];
        }
        mks.push_back(*max_element(f2.begin(), f2.end()));
    }
    double mean = 0; for (double x : mks) mean += x; mean /= RUNS;
    double var = 0; for (double x : mks) var += (x - mean) * (x - mean); var /= RUNS;
    printf("{\"makespan\":%lld,\"valid\":1,\"violations\":0,\"first\":\"\",\"comm_ratio\":%.6f,\"imbalance\":%.6f,"
           "\"utilization\":%.6f,\"rob_ratio\":%.6f,\"rob_cv\":%.6f}\n",
           mk, commAll > 0 ? commPaid / commAll : 0.0, imbalance, util, mean / mk, mean > 0 ? sqrt(var) / mean : 0.0);
    return 0;
}

int main(int argc, char **argv) {
    if (argc >= 4 && !strcmp(argv[1], "strip")) return cmdStrip(argv[2], argv[3], argc > 4 ? argv[4] : nullptr);
    if (argc >= 4 && !strcmp(argv[1], "check")) return cmdCheck(argv[2], argv[3], argc > 4 && !strcmp(argv[4], "--dup-entry"));
    fprintf(stderr, "usage: evaluate strip <raw> <out_d3|-> [out_d2] | evaluate check <d3> <stdout_file> [--dup-entry]\n");
    return 1;
}
