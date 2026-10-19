#include <bits/stdc++.h>
using namespace std;

bool check(const vector<vector<int>> &commc, vector<bool> &cn, int ind)
{
    for (int i = 0; i < commc.size(); i++)
    {
        if (commc[ind][i] != -1 && cn[i])
            return true;
    }
    return false;
}

void findcnct(vector<vector<int>> &cnct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<bool> &cn, vector<int> &fin, int ind)
{
    int fg = 0;
    if (check(commc, cn, ind))
        fg = 1;
    for (int x = 0; x < compc[0].size(); x++)
    {
        int maxi = 0;
        for (int i = 0; i < fin.size(); i++)
        {
            if (commc[ind][i] != -1)
            {
                if (fin[i] == 0)
                {
                    findcnct(cnct, compc, commc, cn, fin, i);
                    fin[i] = 1;
                }
                int mini = INT_MAX;
                for (int j = 0; j < compc[i].size(); j++)
                {
                    if ((fg && cn[i]) || !fg)
                    {
                        if (j != x)
                            mini = min(mini, cnct[i][j] + commc[ind][i] + compc[i][j]);
                        else
                            mini = min(mini, cnct[i][j] + compc[i][j]);
                    }
                }
                if ((fg && cn[i]) || !fg)
                    maxi = max(maxi, mini);
            }
        }
        cnct[ind][x] = maxi;
    }
}

void getcnct(vector<vector<int>> &cnct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<bool> &cn, vector<int> &fin)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            findcnct(cnct, compc, commc, cn, fin, i);
            fin[i] = 1;
        }
    }
}

void findpct(vector<vector<int>> &pct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<int> &fin, int ind)
{
    for (int i = 0; i < compc[0].size(); i++)
    {
        int mpct = 0;
        for (int j = 0; j < commc.size(); j++)
        {
            if (commc[ind][j] != -1)
            {
                if (fin[j] != 1)
                {
                    findpct(pct, compc, commc, fin, j);
                    fin[j] = 1;
                }
                int minpct = 0;
                for (int k = 0; k < compc[0].size(); k++)
                {
                    if (i != k)
                        minpct = max(minpct, {pct[j][k] + commc[ind][j] + compc[j][k]});
                    else
                        minpct = max(minpct, {pct[j][k] + compc[j][k]});
                }
                if (mpct < minpct)
                {
                    mpct = minpct;
                }
            }
        }
        pct[ind][i] = mpct;
    }
}

void getpct(vector<vector<int>> &pct,const vector<vector<int>> &compc,const vector<vector<int>> &commc, vector<int> &fin)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            findpct(pct, compc, commc, fin, i);
            fin[i] = 1;
        }
    }
}

void calaest(vector<double> &aest,const vector<vector<int>> &commc,const vector<double> &mw, vector<int> &fin, int ind)
{
    double maxi = 0;
    for (int i = 0; i < fin.size(); i++)
    {
        if (commc[i][ind] != -1)
        {
            if (fin[i] == 0)
            {
                calaest(aest, commc, mw, fin, i);
                fin[i] = 1;
            }
            maxi = max(maxi, (aest[i] + mw[i] + commc[i][ind]));
        }
    }
    aest[ind] = maxi;
}

void getaest(vector<double> &aest,const vector<vector<int>> &commc,const vector<double> &mw, vector<int> &fin)
{
    for (int i = fin.size() - 1; i >= 0; i--)
    {
        if (fin[i] == 0)
        {
            calaest(aest, commc, mw, fin, i);
            fin[i] = 1;
        }
    }
}

void calalst(vector<double> &alst,const vector<vector<int>> &commc,const vector<double> &mw, vector<int> &fin, int ind)
{
    double mini = DBL_MAX;
    for (int i = 0; i < fin.size(); i++)
    {
        if (commc[ind][i] != -1)
        {
            if (fin[i] == 0)
            {
                calalst(alst, commc, mw, fin, i);
                fin[i] = 1;
            }
            mini = min(mini, alst[i] - commc[ind][i]);
        }
    }
    alst[ind] = round((mini - mw[ind]) * 10.0) / 10.0;
}

void getalst(vector<double> &alst,const vector<vector<int>> &commc,const vector<double> &mw, vector<int> &fin)
{
    for (int i = 0; i < fin.size(); i++)
    {
        if (fin[i] == 0)
        {
            calalst(alst, commc, mw, fin, i);
            fin[i] = 1;
        }
    }
}

void perform(vector<vector<int>> &task, vector<int> &avail,const vector<vector<int>> &commc,const vector<vector<int>> &compc,const vector<int> &seq, vector<bool> &cnp,const vector<vector<int>> &cnct)
{
    for (int i = 0; i < seq.size(); i++)
    {
        int eft = 0;
        int meft = 0, minmeft = INT_MAX;
        for (int j = 0; j < avail.size(); j++)
        {
            int est = 0;
            for (int k = 0; k < seq.size(); k++)
            {
                if (commc[k][seq[i] - 1] != -1)
                {
                    if (j == task[k][2])
                        est = max(est, task[k][1]);
                    else
                        est = max(est, task[k][1] + commc[k][seq[i] - 1]);
                }
            }
            est = max(est, avail[j]);
            eft = est + compc[seq[i] - 1][j];
            meft = (cnp[seq[i] - 1]) ? eft : eft + cnct[seq[i] - 1][j];
            if (meft < minmeft)
            {
                minmeft = meft;
                task[seq[i] - 1][0] = est;
                task[seq[i] - 1][1] = eft;
                task[seq[i] - 1][2] = j;
            }
        }
        avail[task[seq[i] - 1][2]] = task[seq[i] - 1][1];
    }
}

int main()
{
    int n, p;
    cin >> n >> p;
    vector<vector<int>> commc(n, vector<int>(n)), compc(n, vector<int>(p));
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
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
    // cout << "L";
    vector<double> mw(n);
    for (int i = 0; i < n; i++)
    {
        double sum = 0;
        for (int j = 0; j < p; j++)
            sum += compc[i][j];
        mw[i] = sum / p;
        mw[i] = round(mw[i] * 10.0) / 10.0;
    }
    vector<vector<int>> pct(n, vector<int>(p, 0));
    vector<int> fin(n, 0);
    for (int i = 0; i < n; i++)
    {
        if (*max_element(commc[i].begin(), commc[i].end()) == -1)
            fin[i] = 1;
    }
    getpct(pct, compc, commc, fin);
    vector<double> rank(n);
    for (int i = 0; i < n; i++)
    {
        double sum = 0;
        for (int j = 0; j < p; j++)
            sum += (double)pct[i][j];
        rank[i] = sum / p + mw[i];
    }
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < p; j++)
    //         cout << pct[i][j] << " ";
    //     cout << endl;
    // }
    // for (int i = 0; i < n; i++)
    //     cout << rank[i] << endl;
    fin = vector<int>(n, 0);
    vector<double> aest(n, 0);
    getaest(aest, commc, mw, fin);
    vector<double> alst = aest;
    fin = vector<int>(n, 0);
    for (int i = 0; i < n; i++)
    {
        if (*max_element(commc[i].begin(), commc[i].end()) == -1)
        {
            fin[i] = 1;
        }
    }
    getalst(alst, commc, mw, fin);
    vector<bool> cn(n, false);
    for (int i = 0; i < n; i++)
    {
        if (aest[i] == alst[i])
            cn[i] = true;
    }
    vector<bool> cnp(n, false);
    for (int i = 0; i < n; i++)
    {
        if (!cn[i])
        {
            bool flag = false;
            for (int j = 0; j < n; j++)
            {
                if (commc[i][j] != -1 && cn[j])
                    flag = true;
            }
            if (flag)
                cnp[i] = true;
        }
    }
    fin = vector<int>(n, 0);
    vector<vector<int>> cnct(n, vector<int>(p));
    getcnct(cnct, compc, commc, cn, fin);
    vector<int> seq(n);
    priority_queue<pair<double, int>> pq;
    for (int i = 0; i < n; i++)
        pq.push({rank[i], i + 1});
    for (int i = 0; i < n; i++)
    {
        seq[i] = pq.top().second;
        pq.pop();
    }
    for (int i = 0; i < n; i++)
        cout << seq[i] << " ";
    cout << endl;
    vector<int> avail(p, 0);
    vector<vector<int>> task(n, vector<int>(3));
    perform(task, avail, commc, compc, seq, cnp, cnct);
    cout << "TASKS\tAST\tAFT\tPRO\n";
    for (int i = 0; i < n; i++)
    {
        cout << i + 1 << "\t" << task[i][0] << "\t" << task[i][1] << "\t" << task[i][2] + 1 << endl;
    }
}