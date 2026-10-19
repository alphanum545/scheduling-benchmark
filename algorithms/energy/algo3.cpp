#include <bits/stdc++.h>
using namespace std;

bool mycomp(pair<long, int> a, pair<long, int> b)
{
    if (a.second == b.second)
        return a.first < b.first;
    return a.second > b.second;
}

double calrp(int tind, vector<int> &fin, const vector<vector<int>> &commc, vector<double> mw, vector<double> &rup)
{
    double k = 0, maxi = 0;
    for (int i = 0; i < fin.size(); i++)
    {
        if (commc[tind][i] != -1)
        {
            if (fin[i] == 0)
            {
                rup[i] = calrp(i, fin, commc, mw, rup);
                fin[i] = 1;
            }
            k = commc[tind][i] + rup[i];
            maxi = max(maxi, k);
        }
    }
    return maxi + mw[tind];
}

void cal(vector<int> &fin, const vector<vector<int>> &commc, vector<double> mw, vector<double> &rup)
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

double calen(double p_k_ind, double c_k_ef, double f_k_h, double m_k, double compc, double f_k_max)
{
    return (p_k_ind + c_k_ef * pow(f_k_h, m_k)) * compc * (f_k_max / f_k_h);
}

void calminen(vector<double> &E_min, const vector<vector<int>> &compc, vector<double> &p_k_ind, vector<double> &c_k_ef, vector<double> m_k, vector<double> &f_k_ee, vector<double> &f_k_max)
{
    for (int i = 0; i < compc.size(); i++)
    {
        double e_min = DBL_MAX;
        for (int j = 0; j < compc[i].size(); j++)
        {
            // double val = (p_k_ind[j] + c_k_ef[j] * pow(f_k_max[j], m_k[j])) * compc[i][j];
            double val = calen(p_k_ind[j], c_k_ef[j], f_k_ee[j], m_k[j], compc[i][j], f_k_max[j]);
            val = round(val * 10000.0) / 10000.0;
            e_min = min(e_min, val);
        }
        E_min[i] = e_min;
    }
}

void calmaxen(vector<double> &E_max, const vector<vector<int>> &compc, vector<double> &p_k_ind, vector<double> &c_k_ef, vector<double> m_k, vector<double> &f_k_ee, vector<double> &f_k_max)
{
    for (int i = 0; i < compc.size(); i++)
    {
        double e_max = 0;
        for (int j = 0; j < compc[i].size(); j++)
        {
            // double val = (p_k_ind[j] + c_k_ef[j] * pow(f_k_ee[j], m_k[j])) * compc[i][j] * (f_k_max[j] / f_k_ee[j]);
            double val = calen(p_k_ind[j], c_k_ef[j], f_k_max[j], m_k[j], compc[i][j], f_k_max[j]);
            val = round(val * 10000) / 10000.0;
            e_max = max(e_max, val);
        }
        E_max[i] = e_max;
    }
}

double e_giv(double e_given, const vector<vector<double>> &task, vector<double> e_min, vector<int> seq, int j)
{
    double k = 0.0;
    for (int i = 0; i < j; i++)
    {
        k += task[seq[i] - 1][4];
    }
    for (int i = j + 1; i < seq.size(); i++)
    {
        k += e_min[seq[i] - 1];
    }
    k = round(k * 10000.0) / 10000.0;
    return e_given - k;
}

void prossele(vector<vector<double>> &task, vector<double> &avail, const vector<vector<int>> &commc, const vector<vector<int>> &compc, vector<int> &seq, vector<double> &C_k_ef, vector<double> &p_k_ind, vector<double> &m_k, vector<double> &f_k_ee, vector<double> &f_k_max, double E_given, vector<double> &e_min, vector<double> &e_max)
{
    for (int i = 0; i < seq.size(); i++)
    {
        double eft = 0, min_eft = DBL_MAX;
        task[seq[i] - 1][5] = min(e_giv(E_given, task, e_min, seq, i), e_max[seq[i] - 1]);
        task[seq[i] - 1][5] = round(task[seq[i] - 1][5] * 10000.0) / 10000.0;
        double energy = 0;
        for (int j = 0; j < avail.size(); j++)
        {
            double est = 0;
            for (int k = 0; k < commc.size(); k++)
            {
                if (commc[k][seq[i] - 1] != -1)
                {
                    if (task[k][2] == j)
                    {
                        est = (est > task[k][1]) ? est : task[k][1];
                    }
                    else
                        est = (est > (task[k][1] + commc[k][seq[i] - 1])) ? est : (task[k][1] + commc[k][seq[i] - 1]);
                }
            }
            est = (est > avail[j]) ? est : avail[j];
            double u = f_k_ee[j];
            while (u <= f_k_max[j])
            {
                energy = calen(p_k_ind[j], C_k_ef[j], u, m_k[j], compc[seq[i] - 1][j], f_k_max[j]);
                energy = round(energy * 10000.0) / 10000.0;
                // cout << seq[i] << " " << energy << " " << task[seq[i] - 1][5] << " " << u << endl;
                if (energy > task[seq[i] - 1][5])
                {
                    u += 0.01;
                    u = round(u * 100.0) / 100.0;
                    continue;
                }
                eft = est + compc[seq[i] - 1][j] * (f_k_max[j] / u);
                eft = round(eft * 10000.0) / 10000.0;
                if (eft < min_eft)
                {
                    min_eft = eft;
                    task[seq[i] - 1][0] = est;
                    task[seq[i] - 1][1] = eft;
                    task[seq[i] - 1][2] = j;
                    task[seq[i] - 1][3] = u;
                    task[seq[i] - 1][4] = energy;
                }
                u += 0.01;
                u = round(u * 100.0) / 100.0;
            }
            // cout << task[seq[i] - 1][4] << endl;
        }
        avail[task[seq[i] - 1][2]] = task[seq[i] - 1][1];
    }
}

int main(int argc, char **argv)
{
    double PARAM = (argc > 1) ? atof(argv[1]) : 1.0;
    int n, p;
    cin >> n >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<double> p_k_ind(p), C_k_ef(p), m_k(p), f_k_ee(p), f_k_max(p);
    double temp;
    for (int i = 0; i < p; i++)
    {
        cin >> temp;
    }
    for (int i = 0; i < p; i++)
    {
        cin >> p_k_ind[i];
    }
    for (int i = 0; i < p; i++)
    {
        cin >> C_k_ef[i];
    }
    for (int i = 0; i < p; i++)
    {
        cin >> m_k[i];
    }
    for (int i = 0; i < p; i++)
    {
        cin >> f_k_ee[i];
    }
    for (int i = 0; i < p; i++)
    {
        cin >> f_k_max[i];
    }
    
    vector<vector<int>> compc(n, vector<int>(p));
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
    vector<double> E_min(n), E_max(n), E_avg(n);
    double E_min_g = 0.0, E_max_g = 0.0, E_avg_g = 0.0;
    calminen(E_min, compc, p_k_ind, C_k_ef, m_k, f_k_ee, f_k_max);
    calmaxen(E_max, compc, p_k_ind, C_k_ef, m_k, f_k_ee, f_k_max);
    for (int i = 0; i < n; i++)
    {
        E_min_g += round(E_min[i] * 10000.0) / 10000.0;
        E_max_g += round(E_max[i] * 10000.0) / 10000.0;
    }
    for (int i = 0; i < n; i++)
    {
        E_avg[i] = round(((E_min[i] + E_max[i]) / 2) * 10000.0) / 10000.0;
        E_avg_g += round(E_avg[i] * 10000.0) / 10000.0;
    }
    E_min_g = round(E_min_g * 10000.0) / 10000.0;
    E_max_g = round(E_max_g * 10000.0) / 10000.0;
    E_avg_g = round(E_avg_g * 10000.0) / 10000.0;
    // cout << E_min_g << endl;
    // cout << E_max_g << endl;
    // cout << E_avg_g << endl;
    double E_given_g = (1 + PARAM) * E_min_g;
    // cout << E_given_g << endl;
    vector<double> mw(n);  // average computation cost
    vector<int> fin(n, 0); // boolean vector to get track of finished tasks
    for (int i = 0; i < n; i++)
    {
        double y = 0;
        for (int j = 0; j < p; j++)
        {
            y += compc[i][j];
        }
        mw[i] = (y / p);
    }
    vector<double> rup(n); // rank up values
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
            rup[i] = mw[i];
            fin[i] = 1;
        }
    }
    cal(fin, commc, mw, rup); // function to calculate rank up
    for (int i = 0; i < n; i++)
    {
        rup[i] = round(rup[i] * 10000.0) / 10000.0;
    }
    vector<double> r_i(n), s_i(n), E_pre(n);
    double max_rank = *max_element(rup.begin(), rup.end());
    max_rank = round(max_rank * 10000.0) / 10000.0;
    for (int i = 0; i < n; i++)
    {
        r_i[i] = round(((E_max[i] - E_min[i]) / E_max[i]) * 10000.0) / 10000.0;
        s_i[i] = round((rup[i] / max_rank) * 10000.0) / 10000.0;
    }
    double alp = 0.6, t_sum = 0.0, min_sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        t_sum += (alp * r_i[i] + (1 - alp) * s_i[i]);
        t_sum = round(t_sum * 10000.0) / 10000.0;
        min_sum += E_min[i];
        min_sum = round(min_sum * 10000.0) / 10000.0;
    }
    for (int i = 0; i < n; i++)
    {
        E_pre[i] = E_min[i] + ((alp * r_i[i] + (1 - alp) * s_i[i]) / t_sum) * (E_given_g - E_min_g);
        E_pre[i] = round(E_pre[i] * 10000.0) / 10000.0;
        E_pre[i] = min(E_pre[i], E_max[i]);
    }
    vector<pair<int, long>> pseq; // used for task prioritization
    for (int i = 0; i < n; i++)
        pseq.push_back(pair<int, long>(i + 1, rup[i]));
    sort(pseq.begin(), pseq.end(), mycomp);
    vector<int> seq(n); // vector to store task schedule
    for (int i = 0; i < n; i++)
    {
        seq[i] = pseq[i].first;
    }
    vector<vector<double>> task(n, vector<double>(6, 0));
    vector<double> avail(p, 0); // avail - time of availability of processors
    prossele(task, avail, commc, compc, seq, C_k_ef, p_k_ind, m_k, f_k_ee, f_k_max, E_given_g, E_pre, E_max);
    double t_energy = 0.0, makespan = 0.0;
    for (int i = 0; i < n; i++)
    {
        t_energy += task[i][4];
        makespan = max(makespan, task[i][1]);
    }
    // cout << "Total energy: ";
    // cout << t_energy << " " << E_given_g << endl;
    // cout << "Makespan: ";
    cout << makespan << endl;
    // cout << makespan << " " << t_energy << endl;
}