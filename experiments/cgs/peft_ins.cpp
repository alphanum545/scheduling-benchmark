// peft_ins.cpp - repository PEFT (algorithms/makespan/peft.cpp) with insertion-based placement.
// Only the start-time rule changes; ranking, ready list and ties are the original's. `append` = original.
#include <bits/stdc++.h>
using namespace std;

// ---- insertion placement (added for the CGS fairness study; `append` restores the original rule) ----
static bool APPEND = false;
static std::vector<std::vector<std::pair<long long, long long>>> BUSY;  // sorted (start, finish) per processor
static long long slotStart(int q, long long drt, long long dur) {
    if ((int)BUSY.size() <= q) BUSY.resize(q + 1);
    long long st = drt;
    if (dur > 0) for (auto &b : BUSY[q]) { if (b.second <= st) continue; if (st + dur <= b.first) break; st = std::max(st, b.second); }
    return st;
}
static void addBusy(int q, long long s, long long f) {
    if ((int)BUSY.size() <= q) BUSY.resize(q + 1);
    if (f > s) { auto &v = BUSY[q]; v.insert(std::lower_bound(v.begin(), v.end(), std::make_pair(s, f)), {s, f}); }
}
static void parseArgs(int argc, char **argv) { for (int a = 1; a < argc; a++) if (std::string(argv[a]) == "append") APPEND = true; }

// ---- ready-list ordering: highest-priority task whose predecessors are all scheduled ----
vector<int> readyOrder(const vector<int> &prio, const vector<vector<int>> &commc)
{
    int n = prio.size();
    vector<int> pos(n), indeg(n, 0), out;
    for (int k = 0; k < n; k++) pos[prio[k] - 1] = k;
    vector<vector<int>> succ(n);
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++)
        if (commc[i][j] != -1) { succ[i].push_back(j); indeg[j]++; }
    priority_queue<int, vector<int>, greater<int>> ready; // by position in priority list
    for (int i = 0; i < n; i++) if (indeg[i] == 0) ready.push(pos[i]);
    while (!ready.empty()) {
        int t = prio[ready.top()] - 1; ready.pop(); out.push_back(t + 1);
        for (int s : succ[t]) if (--indeg[s] == 0) ready.push(pos[s]);
    }
    return out;
}

bool mycomp(pair<double, int> a, pair<double, int> b)
{
    if (a.first == b.first)
        return a.second < b.second;
    return a.first > b.first;
}

void comp(int ind, vector<vector<int>> &oct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<int> &fin)
{
    vector<int> ans(compc[0].size());
    for (int i = 0; i < compc[0].size(); i++)
    {
        int octi = 0;
        for (int j = 0; j < commc.size(); j++)
        {
            int yt = INT_MAX;
            if (commc[ind][j] != -1)
            {
                if (fin[j] == 0)
                {
                    comp(j, oct, compc, commc, fin);
                    fin[j] = 1;
                }
                if (fin[j] == 1)
                {
                    for (int k = 0; k < compc[0].size(); k++)
                    {
                        if (i != k)
                            yt = min(yt, (oct[j][k] + compc[j][k] + commc[ind][j]));
                        else
                            yt = min(yt, (oct[j][k] + compc[j][k]));
                    }
                }
                octi = max(octi, yt);
            }
        }
        ans[i] = octi;
    }
    oct[ind] = ans;
}

void findoct(vector<vector<int>> &oct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<int> &fin)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            comp(i, oct, compc, commc, fin);
            fin[i] = 1;
        }
    }
}

void performp(const vector<int> &seq, vector<int> &avail,const vector<vector<int>> &commc,const vector<vector<int>> &compc, vector<vector<int>> &tasks,const vector<vector<int>> &oct)
{
    for (int i = 0; i < seq.size(); i++)
    {
        int eft = 0, oeft = 0, min_eft = INT_MAX, min_oeft = INT_MAX;
        int a1, a2, a3;
        for (int j = 0; j < avail.size(); j++)
        {
            int est = 0;
            for (int k = 0; k < commc.size(); k++)
            {
                if (commc[k][seq[i] - 1] != -1)
                {
                    if (tasks[k][2] != j + 1)
                        est = max(est, (tasks[k][1] + commc[k][seq[i] - 1]));
                    else
                        est = max(est, tasks[k][1]);
                }
            }
            // cout << avail[j] << " ";
            est = APPEND ? max(avail[j], est) : (int)slotStart(j, est, compc[seq[i] - 1][j]);
            eft = est + compc[seq[i] - 1][j];
            oeft = eft + oct[seq[i] - 1][j];
            if (oeft < min_oeft || (oeft == min_oeft && eft < min_eft))
            {
                min_oeft = oeft;
                a1 = est;
                a2 = eft;
                a3 = j + 1;
                min_eft = eft;
            }
        }
        // cout << endl;
        tasks[seq[i] - 1][0] = a1;
        tasks[seq[i] - 1][1] = a2;
        tasks[seq[i] - 1][2] = a3;
        avail[a3 - 1] = a2; addBusy(a3 - 1, a1, a2);
    }
}

int main(int argc, char **argv)
{
    parseArgs(argc, argv);
    int n, p;
    cin >> n;
    cin >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<vector<int>> compc(n, vector<int>(p));
    // for (int i = 0; i < 5; i++)
    // {
    //     double x;
    //     for (int j = 0; j < p; j++)
    //         cin >> x;
    // }
    // for (int i = 0; i < p; i++)
    // {
    //     int x;
    //     cin >> x;
    // }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < p; j++)
            cin >> compc[i][j];
    }
    vector<vector<int>> commc(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            cin >> commc[i][j];
    }
    vector<vector<int>> oct(n, vector<int>(p, 0));
    vector<int> fin(n, 0);
    for (int i = 0; i < n; i++)
    {
        int ty = *max_element(commc[i].begin(), commc[i].end());
        if (ty == -1)
            fin[i] = 1;
    }
    findoct(oct, compc, commc, fin);
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < p; j++)
    //         cout << oct[i][j] << " ";
    //     cout << endl;
    // }
    vector<double> rank(n);
    for (int i = 0; i < n; i++)
    {
        int u = 0;
        for (int j = 0; j < p; j++)
            u += oct[i][j] * 100;
        double y = (double)u / p;
        y = round(y / 10.0);
        rank[i] = y / 10.0;
    }
    // for (int i = 0; i < n; i++)
    //     cout << rank[i] << endl;
    vector<pair<double, int>> pseq(n);
    for (int i = 0; i < n; i++)
    {
        pseq[i] = pair<double, int>(rank[i], i + 1);
    }
    sort(pseq.begin(), pseq.end(), mycomp);
    vector<int> seq(n);
    for (int i = 0; i < n; i++)
        seq[i] = pseq[i].second;
    seq = readyOrder(seq, commc);
    for (int i = 0; i < n; i++)
        cout << seq[i] << ",";
    cout << endl;
    vector<vector<int>> tasks(n, vector<int>(3));
    vector<int> avail(p, 0);
    performp(seq, avail, commc, compc, tasks, oct);
    cout << "Tasks\tST\tFT\tProc\n";
    for (int i = 0; i < n; i++)
    {
        cout << i + 1 << "\t" << tasks[i][0] << "\t" << tasks[i][1] << "\t" << tasks[i][2] << endl;
    }

}