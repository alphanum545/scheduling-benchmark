#include <bits/stdc++.h>
using namespace std;

bool mycomp(pair<long, int> a, pair<long, int> b)
{
    if (a.second == b.second)
        return a.first < b.first;
    return a.second > b.second;
}

long calrp(int tind, vector<int> &fin, const vector<vector<int>> &commc, const vector<double> &mw, vector<long> &rup)
{
    long k = 0, maxi = 0;
    for (int i = 0; i < fin.size(); i++)
    {
        if (commc[tind][i] != -1)
        {
            if (fin[i] == 0)
            {
                rup[i] = calrp(i, fin, commc, mw, rup);
                fin[i] = 1;
            }
            k = commc[tind][i] * 10000 + rup[i];
            maxi = max(maxi, k);
        }
    }
    return maxi + mw[tind] * 10000;
}

void cal(vector<int> &fin, const vector<vector<int>> &commc, const vector<double> &mw, vector<long> &rup)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            rup[i] = calrp(i, fin, commc, mw, rup);
            fin[i] = 1;
        }
    }
}

long calrd(int tind, vector<int> &fin, const vector<vector<int>> &commc, const vector<double> &mw, vector<long> &rd)
{
    long k = 0, maxi = 0;
    for (int i = 0; i < fin.size(); i++)
    {
        if (commc[i][tind] != -1)
        {
            if (fin[i] == 0)
            {
                rd[i] = calrd(i, fin, commc, mw, rd);
                fin[i] = 1;
            }
            k = commc[i][tind] * 10000 + rd[i] + mw[i] * 10000;
            maxi = max(maxi, k);
        }
    }
    return maxi;
}

void calc(vector<int> &fin, const vector<vector<int>> &commc, const vector<double> &mw, vector<long> &rd)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            rd[i] = calrd(i, fin, commc, mw, rd);
            fin[i] = 1;
        }
    }
}

void performphase(vector<int> &aft, vector<int> &ast, vector<int> &pros, vector<int> &avail, const vector<vector<int>> &commc, const vector<int> &seq, const vector<vector<int>> &compc, const vector<int> &crit)
{
    for (int i = 0; i < seq.size(); i++)
    {
        int ans1, ans2, ans3, min_eft = INT_MAX;
        for (int j = 0; j < avail.size(); j++)
        {
            if (crit[i] == -1 || (crit[i] == j))
            {
                int st, est = 0, eft = 0, max_est = 0, pro;
                int fg = 0;
                for (int k = 0; k < seq.size(); k++)
                {
                    if (commc[k][seq[i] - 1] != -1)
                    {
                        fg = 1;
                        if (pros[k] != j + 1)
                        {
                            est = max(avail[j], commc[k][seq[i] - 1] + aft[k]);
                        }
                        else
                            est = max(avail[j], aft[k]);
                    }
                    else if (!fg)
                        est = avail[j];
                    if (max_est < est)
                    {
                        max_est = est;
                    }
                }
                eft = max_est + compc[seq[i] - 1][j];
                if (eft < min_eft || (eft == min_eft && abs(avail[pro - 1] - avail[j]) > abs((eft - max_est) - (ans1 - ans2))))
                {
                    pro = j + 1;
                    st = max_est;
                    min_eft = eft;
                    ans1 = min_eft;
                    ans2 = st;
                    ans3 = pro;
                }
            }
        }
        aft[seq[i] - 1] = ans1;
        ast[seq[i] - 1] = ans2;
        pros[seq[i] - 1] = ans3;
        avail[ans3 - 1] = ans1;
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
    vector<int> seq(n);
    vector<vector<int>> commc(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            cin >> commc[i][j];
    }
    vector<double> mw(n);
    vector<int> fin(n, 0);
    for (int i = 0; i < n; i++)
    {
        double y = 0;
        for (int j = 0; j < p; j++)
        {
            y += compc[i][j];
        }
        mw[i] = (y / p);
    }
    vector<long> rup(n);
    for (int i = 0; i < n; i++)
    {
        int count = 0;
        for (int j = 0; j < n; j++)
        {
            if (commc[i][j] == -1)
                count++;
        }
        if (count == n)
        {
            rup[i] = mw[i] * 10000;
            fin[i] = 1;
        }
    }
    cal(fin, commc, mw, rup);
    for (int i = 0; i < n; i++)
    {
        double h = (double)rup[i] / 10;
        rup[i] = round(h);
    }
    fin = vector<int>(n, 0);
    vector<long> rd(n, 0);
    for (int i = 0; i < n; i++)
    {
        int count = 0;
        for (int j = 0; j < n; j++)
        {
            if (commc[j][i] == -1)
                count++;
        }
        if (count == n)
        {
            fin[i] = 1;
        }
    }
    calc(fin, commc, mw, rd);
    for (int i = 0; i < n; i++)
    {
        double h = (double)rd[i] / 10;
        rd[i] = round(h);
    }
    vector<long> prio(n);
    for (int i = 0; i < n; i++)
    {
        prio[i] = rup[i] + rd[i];
    }
    priority_queue<pair<long, int>> pq;
    for (int i = 0; i < n; i++)
    {
        pq.push(pair<long, int>(prio[i], -(i + 1)));
    }
    fin = vector<int>(n, 0);
    for (int i = 0; i < n; i++)
    {
        vector<pair<long, int>> te;
        while (!pq.empty())
        {
            int fg = 1;
            pair<long, int> e = pq.top();
            e.second = (-1) * (e.second);
            for (int j = 0; j < n; j++)
            {
                if (commc[j][e.second - 1] != -1 && fin[j] == 0)
                {
                    fg = 0;
                    te.push_back(pair<long, int>(e.first, -e.second));
                    break;
                }
            }
            if (fg == 1)
            {
                seq[i] = e.second;
                fin[e.second - 1] = 1;
                pq.pop();
                break;
            }
            pq.pop();
        }
        for (int j = 0; j < te.size(); j++)
        {
            pq.push(te[j]);
        }
        te.clear();
    }
    long cpr;
    if (n > 0)
        cpr = prio[seq[0] - 1];
    vector<int> cp;
    for (int i = 0; i < n; i++)
    {
        if (prio[seq[i] - 1] == cpr)
            cp.push_back(i);
    }
    int minpro, mini = INT_MAX;
    if (n > 0)
    {
        for (int i = 0; i < p; i++)
        {
            int sum = 0;
            for (int j = 0; j < cp.size(); j++)
            {
                sum += compc[seq[cp[j]] - 1][i];
            }
            if (mini > sum)
            {
                mini = sum;
                minpro = i;
            }
        }
    }
    vector<int> crit(n, -1);
    for (int i = 0; i < cp.size(); i++)
    {
        crit[cp[i]] = minpro;
    }
    // for(int i = 0;i < n;i++){
    //     cout << seq[i] << " ";
    // }
    // cout << endl;
    vector<int> aft(n, 0), ast(n, 0), pros(n, 0), avail(p, 0);
    performphase(aft, ast, pros, avail, commc, seq, compc, crit);
    // cout << "Tasks\tAST\tAFT\tProcessor\n";
    for (int i = 0; i < n; i++)
        cout << seq[i] << " ";
    cout << endl;
    cout << "TASKS\tAST\tAFT\tPRO\n";
    for (int i = 0; i < n; i++)
    {
        //
        cout << i + 1 << "\t" << ast[i] << "\t" << aft[i] << "\t" << pros[i] << endl;
        // }
    }
}