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
//       Prints JSON: {"makespan":..,"valid":0/1,"violations":..,"first":"..."}
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
    printf("{\"makespan\":%lld,\"valid\":%d,\"violations\":%d,\"first\":\"%s\"}\n", mk, nv ? 0 : 1, nv, esc(first).c_str());
    return 0;
}

int main(int argc, char **argv) {
    if (argc >= 4 && !strcmp(argv[1], "strip")) return cmdStrip(argv[2], argv[3], argc > 4 ? argv[4] : nullptr);
    if (argc >= 4 && !strcmp(argv[1], "check")) return cmdCheck(argv[2], argv[3], argc > 4 && !strcmp(argv[4], "--dup-entry"));
    fprintf(stderr, "usage: evaluate strip <raw> <out_d3|-> [out_d2] | evaluate check <d3> <stdout_file> [--dup-entry]\n");
    return 1;
}
