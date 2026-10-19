#include <bits/stdc++.h>
using namespace std;

int maxPathMinComputeCost(const vector<vector<int>> &commc, const vector<vector<int>> &compc)
{
    const int n = (int)commc.size();
    if (n == 0)
        return 0;
    vector<long long> w(n, 0);
    for (int i = 0; i < n; ++i)
    {
        w[i] = *min_element(compc[i].begin(), compc[i].end());
    }
    vector<int> indeg(n, 0), outdeg(n, 0);
    vector<vector<int>> adj(n);
    for (int u = 0; u < n; ++u)
    {
        for (int v = 0; v < n; ++v)
        {
            if (commc[u][v] != -1)
            {
                ++indeg[v];
                ++outdeg[u];
                adj[u].push_back(v);
            }
        }
    }
    vector<int> is_src(n, 0), is_sink(n, 0);
    for (int i = 0; i < n; ++i)
    {
        if (indeg[i] == 0)
            is_src[i] = 1;
        if (outdeg[i] == 0)
            is_sink[i] = 1;
    }
    queue<int> q;
    vector<int> indeg_work = indeg;
    for (int i = 0; i < n; ++i)
        if (indeg_work[i] == 0)
            q.push(i);
    vector<int> topo;
    topo.reserve(n);
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        topo.push_back(u);
        for (int v : adj[u])
        {
            if (--indeg_work[v] == 0)
                q.push(v);
        }
    }
    const long long NEG_INF = LLONG_MIN / 4;
    vector<long long> dp(n, NEG_INF);
    vector<int> pred(n, -1);
    for (int s = 0; s < n; ++s)
        if (is_src[s])
        {
            dp[s] = w[s];
            pred[s] = -1;
        }
    for (int u : topo)
    {
        if (dp[u] == NEG_INF)
            continue;
        for (int v : adj[u])
        {
            long long cand = dp[u] + w[v];
            if (cand > dp[v])
            {
                dp[v] = cand;
                pred[v] = u;
            }
        }
    }
    int bestExit = -1;
    long long bestCost = NEG_INF;
    for (int t = 0; t < n; ++t)
        if (is_sink[t])
        {
            if (dp[t] > bestCost)
            {
                bestCost = dp[t];
                bestExit = t;
            }
        }
    vector<int> path;
    for (int v = bestExit; v != -1; v = pred[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return bestCost;
}

int main()
{
    int n, p;
    cin >> n >> p;
    vector<vector<int>> compc(n, vector<int>(p, 0)), commc(n, vector<int>(n, 0));
    for (int i = 0; i < 5; i++)
    {
        double x;
        for (int j = 0; j < p; j++)
            cin >> x;
    }
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
    int cpmin = maxPathMinComputeCost(commc, compc);
    cout << cpmin << endl;
}