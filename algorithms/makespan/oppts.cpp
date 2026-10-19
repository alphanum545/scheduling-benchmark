#include <bits/stdc++.h>
using namespace std;

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

void calpcg(vector<vector<int>> &pcg,const vector<vector<int>> &commc,const vector<vector<int>> &compc, vector<int> &fin, int &ind)
{
    for (int i = 0; i < compc[0].size(); i++)
    {
        int maxi = 0;
        for (int j = 0; j < commc.size(); j++)
        {
            if (commc[ind][j] != -1)
            {
                if (fin[j] == 0)
                {
                    calpcg(pcg, commc, compc, fin, j);
                    fin[j] = 1;
                }
                int mini = INT_MAX;
                for (int k = 0; k < compc[0].size(); k++)
                {
                    if (i == k)
                        mini = min(mini, (pcg[j][k] + compc[j][k]));
                    else
                        mini = min(mini, (pcg[j][k] + commc[ind][j] + compc[j][k]));
                }
                maxi = max(mini, maxi);
            }
        }
        pcg[ind][i] = maxi + compc[ind][i];
    }
}

void getpcg(vector<vector<int>> &pcg,const vector<vector<int>> &commc,const vector<vector<int>> &compc, vector<int> &fin)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            calpcg(pcg, commc, compc, fin, i);
            fin[i] = 1;
        }
    }
}

void performtask(vector<vector<int>> &task,const vector<int> &seq, vector<int> &avail,const vector<vector<int>> &compc,const vector<vector<int>> &commc,const vector<vector<int>> &pcg)
{
    for (int i = 0; i < seq.size(); i++)
    {
        int ieftm = INT_MAX, eft = 0, outdeg = 0, meft = INT_MAX;
        for (int j = 0; j < commc.size(); j++)
        {
            if (commc[seq[i] - 1][j] != -1)
                outdeg++;
        }
        for (int j = 0; j < avail.size(); j++)
        {
            int est = 0, ieft = 0;
            for (int k = 0; k < commc.size(); k++)
            {
                if (commc[k][seq[i] - 1] != -1)
                {
                    if (task[k][2] == j)
                        est = max(est, task[k][1]);
                    else
                        est = max(task[k][1] + commc[k][seq[i] - 1], est);
                }
            }
            est = max(est, avail[j]);
            eft = est + compc[seq[i] - 1][j];
            ieft = eft + pcg[seq[i] - 1][j];
            // if (outdeg > compc[0].size())
            // {
            //     if (eft < meft)
            //     {
            //         meft = eft;
            //         task[seq[i] - 1][0] = est;
            //         task[seq[i] - 1][1] = eft;
            //         task[seq[i] - 1][2] = j;
            //     }
            // }
            // else
            // {
            if (ieft < ieftm)
            {
                ieftm = ieft;
                task[seq[i] - 1][0] = est;
                task[seq[i] - 1][1] = eft;
                task[seq[i] - 1][2] = j;
            }
            // }
        }
        avail[task[seq[i] - 1][2]] = task[seq[i] - 1][1];
    }
}

int main()
{
    int n, p;
    cin >> n >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<vector<int>> commc(n, vector<int>(n, -1)), compc(n, vector<int>(p));
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
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            cin >> commc[i][j];
    }
    vector<vector<int>> pcg(n, vector<int>(p));
    vector<int> fin(n, 0);
    for (int i = 0; i < n; i++)
    {
        if (*max_element(commc[i].begin(), commc[i].end()) == -1)
        {
            for (int j = 0; j < p; j++)
            {
                pcg[i][j] = compc[i][j];
            }
            fin[i] = 1;
        }
    }
    getpcg(pcg, commc, compc, fin);
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < p; j++)
    //         cout << pcg[i][j] << " ";
    //     cout << endl;
    // }
    vector<double> prank(n);
    vector<int> outdeg(n, 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (commc[i][j] != -1)
                outdeg[i]++;
        }
    }
    for (int i = 0; i < n; i++)
    {
        double k = 0, mw = 0;
        for (int j = 0; j < p; j++)
        {
            k += pcg[i][j];
            mw += compc[i][j];
        }
        prank[i] = (k / p);
    }
    priority_queue<pair<double, int>> pq;
    for (int i = 0; i < n; i++)
    {
        pq.push({prank[i], -(i + 1)});
    }
    vector<int> seq(n);
    for (int i = 0; i < n; i++)
    {
        int y = -(pq.top().second);
        seq[i] = y;
        pq.pop();
    }
    seq = readyOrder(seq, commc);
    vector<vector<int>> task(n, vector<int>(3));
    vector<int> avail(p, 0);
    performtask(task, seq, avail, compc, commc, pcg);
    for(int i = 0;i < n;i++) cout << seq[i] << ",";
    cout << endl;
    cout << "TASKS\tST\tFT\tPRO\n";
    for (int i = 0; i < n; i++)
    {
        cout << i + 1 << "\t" << task[i][0] << "\t" << task[i][1] << "\t" << task[i][2] + 1 << endl;
    }
}