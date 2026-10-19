#include <bits/stdc++.h>
using namespace std;

/*
  Algo4: WESMM-HEFT (Weighted ESMM extension of Algo3)

  Input format (same as your algo3.cpp):
    n p
    [p numbers ignored]
    p_k_ind[0..p-1]
    C_k_ef [0..p-1]
    m_k    [0..p-1]
    f_k_ee [0..p-1]
    f_k_max[0..p-1]
    compc matrix n x p (int)
    commc matrix n x n (int)  // -1 means no edge

  Output:
    makespan (double printed as in algo3: default cout)
*/

static inline double rnd4(double x) { return round(x * 10000.0) / 10000.0; }

// Energy model used in your codes (same as algo3.cpp)
static inline double calen(double p_ind, double c_eff, double f_h, double m, double compc, double f_max) {
    // (Pind + Ceff*f^m) * comp * (fmax/f)
    return (p_ind + c_eff * pow(f_h, m)) * compc * (f_max / f_h);
}

// Average computation cost mw[i] = avg_k compc[i][k]
static vector<double> compute_mw(const vector<vector<int>>& compc) {
    int n = (int)compc.size();
    int p = (int)compc[0].size();
    vector<double> mw(n, 0.0);
    for (int i = 0; i < n; i++) {
        double s = 0;
        for (int k = 0; k < p; k++) s += compc[i][k];
        mw[i] = s / p;
    }
    return mw;
}

static vector<vector<int>> build_succ(const vector<vector<int>>& commc) {
    int n = (int)commc.size();
    vector<vector<int>> succ(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (commc[i][j] != -1) succ[i].push_back(j);
    return succ;
}

static vector<vector<int>> build_pred(const vector<vector<int>>& commc) {
    int n = (int)commc.size();
    vector<vector<int>> pred(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (commc[i][j] != -1) pred[j].push_back(i);
    return pred;
}

// Upward rank: ranku[i] = mw[i] + max_{succ}( comm(i,s) + ranku[s] )
static double ranku_dfs(int u,
                        const vector<vector<int>>& succ,
                        const vector<vector<int>>& commc,
                        const vector<double>& mw,
                        vector<double>& memo,
                        vector<char>& vis) {
    if (vis[u]) return memo[u];
    vis[u] = 1;
    double best = 0.0;
    for (int v : succ[u]) {
        best = max(best, (double)commc[u][v] + ranku_dfs(v, succ, commc, mw, memo, vis));
    }
    memo[u] = mw[u] + best;
    return memo[u];
}

static vector<double> compute_ranku(const vector<vector<int>>& commc, const vector<double>& mw) {
    int n = (int)commc.size();
    auto succ = build_succ(commc);
    vector<double> memo(n, 0.0);
    vector<char> vis(n, 0);
    for (int i = 0; i < n; i++) (void)ranku_dfs(i, succ, commc, mw, memo, vis);
    for (int i = 0; i < n; i++) memo[i] = rnd4(memo[i]);
    return memo;
}

// Downward rank: rankd[i] = max_{pred}( rankd[p] + mw[p] + comm(p,i) )
// Compute in topological order.
static vector<double> compute_rankd(const vector<vector<int>>& commc, const vector<double>& mw) {
    int n = (int)commc.size();
    auto pred = build_pred(commc);

    vector<int> indeg(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (commc[i][j] != -1) indeg[j]++;

    queue<int> q;
    for (int i = 0; i < n; i++) if (indeg[i] == 0) q.push(i);

    vector<int> topo;
    topo.reserve(n);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        topo.push_back(u);
        for (int v = 0; v < n; v++) {
            if (commc[u][v] == -1) continue;
            if (--indeg[v] == 0) q.push(v);
        }
    }

    vector<double> rankd(n, 0.0);
    for (int u : topo) {
        double best = 0.0;
        for (int p : pred[u]) {
            best = max(best, rankd[p] + mw[p] + (double)commc[p][u]);
        }
        rankd[u] = best;
    }

    for (int i = 0; i < n; i++) rankd[i] = rnd4(rankd[i]);
    return rankd;
}

// rankp = ranku - rankd + rankd(exit) where exit is any sink with maximum rankd
static vector<double> compute_rankp(const vector<vector<int>>& commc,
                                   const vector<double>& ranku,
                                   const vector<double>& rankd) {
    int n = (int)commc.size();
    auto succ = build_succ(commc);

    double exit_rankd = 0.0;
    for (int i = 0; i < n; i++) {
        if (succ[i].empty()) exit_rankd = max(exit_rankd, rankd[i]);
    }
    exit_rankd = rnd4(exit_rankd);

    vector<double> rankp(n, 0.0);
    for (int i = 0; i < n; i++) rankp[i] = rnd4(ranku[i] - rankd[i] + exit_rankd);
    return rankp;
}

// Level number LN from entry (simple DAG level).
static vector<int> compute_level_number(const vector<vector<int>>& commc) {
    int n = (int)commc.size();
    vector<int> indeg(n, 0), LN(n, 1);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (commc[i][j] != -1) indeg[j]++;

    queue<int> q;
    for (int i = 0; i < n; i++) if (indeg[i] == 0) q.push(i);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v = 0; v < n; v++) {
            if (commc[u][v] == -1) continue;
            LN[v] = max(LN[v], LN[u] + 1);
            if (--indeg[v] == 0) q.push(v);
        }
    }
    return LN;
}

// Favourite processors: sorted by compc[ti][k] (faster first)
static vector<int> favourites(const vector<vector<int>>& compc, int ti) {
    int p = (int)compc[0].size();
    vector<int> idx(p);
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int a, int b) {
        return compc[ti][a] < compc[ti][b];
    });
    return idx;
}

// FED: average energy at f_max on top-2 favourite processors
static double FED_top2(int ti,
                       const vector<vector<int>>& compc,
                       const vector<double>& p_ind,
                       const vector<double>& c_eff,
                       const vector<double>& mexp,
                       const vector<double>& fmax) {
    auto fav = favourites(compc, ti);
    int a = fav[0];
    int b = (fav.size() > 1) ? fav[1] : fav[0];

    double Ea = calen(p_ind[a], c_eff[a], fmax[a], mexp[a], compc[ti][a], fmax[a]);
    double Eb = calen(p_ind[b], c_eff[b], fmax[b], mexp[b], compc[ti][b], fmax[b]);
    return 0.5 * (Ea + Eb);
}

// Weight W(ti) = FED(ti) * (1 + 1/(sqrt(LN(ti)) + sqrt(LN(entry))))
static double weightW(int ti, int entry,
                      const vector<int>& LN,
                      const vector<vector<int>>& compc,
                      const vector<double>& p_ind,
                      const vector<double>& c_eff,
                      const vector<double>& mexp,
                      const vector<double>& fmax) {
    double fed = FED_top2(ti, compc, p_ind, c_eff, mexp, fmax);
    double denom = sqrt((double)LN[ti]) + sqrt((double)LN[entry]);
    if (denom <= 0) denom = 1.0;
    return fed * (1.0 + 1.0 / denom);
}

// Compute E_min per task (minimum across processors at f_ee)
static void calminen(vector<double>& E_min,
                     const vector<vector<int>>& compc,
                     const vector<double>& p_ind,
                     const vector<double>& c_eff,
                     const vector<double>& mexp,
                     const vector<double>& f_ee,
                     const vector<double>& fmax) {
    int n = (int)compc.size();
    int p = (int)compc[0].size();
    for (int i = 0; i < n; i++) {
        double e_min = DBL_MAX;
        for (int k = 0; k < p; k++) {
            double val = calen(p_ind[k], c_eff[k], f_ee[k], mexp[k], compc[i][k], fmax[k]);
            val = rnd4(val);
            e_min = min(e_min, val);
        }
        E_min[i] = e_min;
    }
}

// Compute E_max per task (maximum across processors at f_max)
static void calmaxen(vector<double>& E_max,
                     const vector<vector<int>>& compc,
                     const vector<double>& p_ind,
                     const vector<double>& c_eff,
                     const vector<double>& mexp,
                     const vector<double>& fmax) {
    int n = (int)compc.size();
    int p = (int)compc[0].size();
    for (int i = 0; i < n; i++) {
        double e_max = 0.0;
        for (int k = 0; k < p; k++) {
            double val = calen(p_ind[k], c_eff[k], fmax[k], mexp[k], compc[i][k], fmax[k]);
            val = rnd4(val);
            e_max = max(e_max, val);
        }
        E_max[i] = e_max;
    }
}

// Compute pre-assigned energy for the current task using ESMM-style weighted slack sharing
static double preassignEnergyWeightedSlack(int curTask, int iPos, const vector<int>& seq,
                                          double remainingEnergy,
                                          const vector<double>& E_min,
                                          const vector<double>& E_max,
                                          const vector<double>& W) {
    double sumW = 0.0, sumEmin = 0.0;
    for (int k = iPos; k < (int)seq.size(); k++) {
        int t = seq[k];
        sumW += W[t];
        sumEmin += E_min[t];
    }
    double Eslack = remainingEnergy - sumEmin; // Eq. (22)
    if (Eslack < 0) Eslack = 0;

    double frac = (sumW <= 1e-12) ? 0.0 : (W[curTask] / sumW);
    double Epre = E_min[curTask] + frac * Eslack; // Eq. (21)

    Epre = min(Epre, E_max[curTask]);
    Epre = max(Epre, E_min[curTask]);
    return rnd4(Epre);
}

/*
  task[t][0]=AST, [1]=AFT, [2]=processor id, [3]=chosen frequency, [4]=energy used, [5]=E_pre (cap)
*/
static void schedule_WESMM_HEFT(vector<vector<double>>& task,
                               vector<double>& avail,
                               const vector<vector<int>>& commc,
                               const vector<vector<int>>& compc,
                               const vector<int>& seq,
                               const vector<double>& C_k_ef,
                               const vector<double>& p_k_ind,
                               const vector<double>& m_k,
                               const vector<double>& f_k_ee,
                               const vector<double>& f_k_max,
                               double E_given_global,
                               const vector<double>& E_min,
                               const vector<double>& E_max,
                               const vector<double>& W) {
    int n = (int)compc.size();
    int p = (int)compc[0].size();

    for (int i = 0; i < (int)seq.size(); i++) {
        int t = seq[i];

        // remaining energy = E_given_global - sum energy of scheduled tasks
        double consumed = 0.0;
        for (int x = 0; x < n; x++) consumed += task[x][4];
        consumed = rnd4(consumed);
        double remaining = E_given_global - consumed;
        if (remaining < 0) remaining = 0;

        // pre-assign energy cap for this task (stored in task[t][5])
        task[t][5] = preassignEnergyWeightedSlack(t, i, seq, remaining, E_min, E_max, W);

        double bestEFT = DBL_MAX;
        double bestAST = 0.0;
        int bestProc = 0;
        double bestFreq = f_k_max[0];
        double bestEnergy = 0.0;

        for (int k = 0; k < p; k++) {
            // compute EST based on predecessors and processor availability
            double est = 0.0;
            for (int pred = 0; pred < n; pred++) {
                if (commc[pred][t] == -1) continue;
                int predProc = (int)task[pred][2];
                double ready = task[pred][1];
                if (predProc != k) ready += commc[pred][t];
                est = max(est, ready);
            }
            est = max(est, avail[k]);

            // choose the largest frequency step that meets energy cap and minimizes EFT
            // (same structure as your algo3 loop: from f_ee to f_max step 0.01)
            double chosenEFT = DBL_MAX, chosenFreq = f_k_max[k], chosenEnergy = 0.0;
            bool found = false;

            for (double f = f_k_ee[k]; f <= f_k_max[k] + 1e-9; f += 0.01) {
                f = round(f * 100.0) / 100.0;

                double compTime = (double)compc[t][k] * (f_k_max[k] / f);
                double eft = est + compTime;
                double energy = calen(p_k_ind[k], C_k_ef[k], f, m_k[k], compc[t][k], f_k_max[k]);
                energy = rnd4(energy);

                if (energy <= task[t][5] + 1e-9) {
                    // among feasible freqs, take the smallest EFT (ties: smaller energy)
                    if (!found || eft < chosenEFT - 1e-12 || (fabs(eft - chosenEFT) <= 1e-12 && energy < chosenEnergy)) {
                        found = true;
                        chosenEFT = eft;
                        chosenFreq = f;
                        chosenEnergy = energy;
                    }
                }
            }

            if (!found) {
                // if nothing satisfies cap, fall back to max frequency (keeps schedule feasible)
                double f = f_k_max[k];
                double compTime = (double)compc[t][k] * (f_k_max[k] / f);
                double eft = est + compTime;
                double energy = calen(p_k_ind[k], C_k_ef[k], f, m_k[k], compc[t][k], f_k_max[k]);
                chosenEFT = eft;
                chosenFreq = f;
                chosenEnergy = rnd4(energy);
            }

            if (chosenEFT < bestEFT) {
                bestEFT = chosenEFT;
                bestAST = est;
                bestProc = k;
                bestFreq = chosenFreq;
                bestEnergy = chosenEnergy;
            }
        }

        // store decision
        task[t][0] = rnd4(bestAST);
        task[t][1] = rnd4(bestEFT);
        task[t][2] = bestProc;
        task[t][3] = rnd4(bestFreq);
        task[t][4] = rnd4(bestEnergy);

        // update processor availability
        avail[bestProc] = task[t][1];
    }
}

int main(int argc, char** argv) {
    double PARAM = (argc > 1) ? atof(argv[1]) : 1.0;

    int n, p;
    cin >> n >> p;
    if (n == 0 || p == 0) {
        cout << 0.0 << "\n";
        return 0;
    }

    vector<double> p_k_ind(p), C_k_ef(p), m_k(p), f_k_ee(p), f_k_max(p);

    // In your files, there is a first loop that reads and discards p numbers.
    // Keep it to remain compatible with your dataset format.
    // double temp;
    // for (int i = 0; i < p; i++) cin >> temp;

    for (int i = 0; i < p; i++) cin >> p_k_ind[i];
    for (int i = 0; i < p; i++) cin >> C_k_ef[i];
    for (int i = 0; i < p; i++) cin >> m_k[i];
    for (int i = 0; i < p; i++) cin >> f_k_ee[i];
    for (int i = 0; i < p; i++) cin >> f_k_max[i];

    vector<vector<int>> compc(n, vector<int>(p));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            cin >> compc[i][j];

    vector<vector<int>> commc(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cin >> commc[i][j];

    // E_min / E_max
    vector<double> E_min(n), E_max(n);
    calminen(E_min, compc, p_k_ind, C_k_ef, m_k, f_k_ee, f_k_max);
    calmaxen(E_max, compc, p_k_ind, C_k_ef, m_k, f_k_max);

    double E_min_g = 0.0;
    for (int i = 0; i < n; i++) E_min_g += rnd4(E_min[i]);
    E_min_g = rnd4(E_min_g);
    double E_max_g = 0.0;
    for (int i = 0; i < n; i++) E_max_g += rnd4(E_max[i]);
    E_max_g = rnd4(E_max_g);

    // Global energy budget (same style as your algo3.cpp): (1 + PARAM) * E_min_g
    double E_given_g = rnd4((1.0 + PARAM) * E_min_g);
    // double E_given_g = rnd4(0.5 * E_max_g);


    // ---- Stage A: ESMM-style priority (rankp) ----
    vector<double> mw = compute_mw(compc);
    vector<double> ranku = compute_ranku(commc, mw);
    vector<double> rankd = compute_rankd(commc, mw);
    vector<double> rankp = compute_rankp(commc, ranku, rankd);

    // Build priority list (descending rankp)
    vector<int> seq(n);
    iota(seq.begin(), seq.end(), 0);
    sort(seq.begin(), seq.end(), [&](int a, int b) {
        if (fabs(rankp[a] - rankp[b]) > 1e-12) return rankp[a] > rankp[b];
        return a < b;
    });

    // ---- Stage B: weights W(t) for weighted slack-energy sharing ----
    auto pred = build_pred(commc);

    // pick an "entry" task: any task with no predecessors (lowest id)
    int entry = 0;
    for (int i = 0; i < n; i++) {
        if (pred[i].empty()) { entry = i; break; }
    }

    vector<int> LN = compute_level_number(commc);
    vector<double> W(n, 0.0);
    for (int i = 0; i < n; i++) {
        W[i] = weightW(i, entry, LN, compc, p_k_ind, C_k_ef, m_k, f_k_max);
    }

    // ---- Stage C: EFT + DVFS selection under per-task energy cap ----
    vector<vector<double>> task(n, vector<double>(6, 0.0));
    vector<double> avail(p, 0.0);
    schedule_WESMM_HEFT(task, avail, commc, compc, seq, C_k_ef, p_k_ind, m_k, f_k_ee, f_k_max,
                       E_given_g, E_min, E_max, W);

    double makespan = 0.0;
    for (int i = 0; i < n; i++) makespan = max(makespan, task[i][1]);

    cout << rnd4(makespan) << "\n";
    return 0;
}
