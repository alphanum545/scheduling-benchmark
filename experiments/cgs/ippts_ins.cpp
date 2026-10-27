// ippts_ins.cpp - repository IPPTS (algorithms/makespan/ippts.cpp) with insertion-based placement.
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

void calpcm(vector<vector<int>> &pcm,const vector<vector<int>> &commc,const vector<vector<int>> &compc, vector<int> &fin, int &ind){
    for(int i = 0;i < compc[0].size();i++){
        int maxi = 0;
       for(int j = 0;j < commc.size();j++){
            if(commc[ind][j] != -1){
                if(fin[j] == 0){
                    calpcm(pcm,commc,compc,fin,j);
                    fin[j] = 1;
                }
                int mini = INT_MAX;
                for(int k = 0;k < compc[0].size();k++){
                    if(i == k) mini = min(mini, (pcm[j][k] + compc[j][k] + compc[ind][k]));
                    else mini = min(mini,(pcm[j][k] + compc[j][k] + compc[ind][k] + commc[ind][j]));
                }
                maxi = max(mini,maxi);
            }
       }
       pcm[ind][i] = maxi;
    }
}

void getpcm(vector<vector<int>> &pcm,const vector<vector<int>> &commc,const vector<vector<int>> &compc,vector<int> &fin){
    for(int i = 0;i < fin.size();i++){
        if(fin[i] == 0){
            calpcm(pcm,commc,compc,fin,i);
            fin[i] = 1;
        }
    }
}

void performtask(vector<vector<int>> &task,const vector<int> &seq,vector<int> &avail ,const vector<vector<int>> &compc,const vector<vector<int>> &commc,const vector<vector<int>> &pcm){
    for(int i = 0;i < seq.size();i++){
        int lheadm = INT_MAX,eft = 0;
        for(int j = 0;j < avail.size();j++){
            int est = 0,lhet = 0,lhead = 0;
            for(int k = 0;k < commc.size();k++){
                if(commc[k][seq[i] - 1] != -1){
                    if(task[k][2] == j) est = max(est,task[k][1]);
                    else est = max(task[k][1] + commc[k][seq[i] - 1],est);
                }
            }
            est = APPEND ? max(est,avail[j]) : (int)slotStart(j, est, compc[seq[i] - 1][j]);
            eft = est + compc[seq[i] - 1][j];
            lhet = pcm[seq[i] - 1][j] - compc[seq[i] - 1][j];
            lhead = eft + lhet;
            if(lhead < lheadm){
                lheadm = lhead;
                task[seq[i] - 1][0] = est;
                task[seq[i] - 1][1] = eft;
                task[seq[i] - 1][2] = j;
            }
        }
        avail[task[seq[i] - 1][2]] = task[seq[i] - 1][1]; addBusy(task[seq[i] - 1][2], task[seq[i] - 1][0], task[seq[i] - 1][1]);
    }
}

int main(int argc, char **argv){
    parseArgs(argc, argv);
    int n,p;
    cin >> n >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<vector<int>> commc(n,vector<int> (n)), compc(n,vector<int> (p));
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
    for(int i = 0;i < n;i++){
        for(int j = 0;j < p;j++) cin >> compc[i][j];
    }
    for(int i = 0;i < n;i++){
        for(int j = 0;j < n;j++) cin >> commc[i][j];
    }
    vector<vector<int>> pcm(n,vector<int> (p));
    vector<int> fin(n,0);
    for(int i = 0;i < n;i++){
        if(*max_element(commc[i].begin(),commc[i].end()) == -1){
            for(int j = 0;j < p;j++){
                pcm[i][j] = compc[i][j];
            }
            fin[i] = 1;
        }
    }
    getpcm(pcm,commc,compc,fin);
    // for(int i = 0;i < n;i++){
    //     for(int j = 0;j < p;j++) cout << pcm[i][j] << " ";
    //     cout << endl;
    // }
    vector<int> out(n);
    for(int i = 0;i < n;i++){
        int count = 0;
        for(int j = 0;j < n;j++){
            if(commc[i][j]  != -1) count++;
        }
        out[i] = count;
    }
    vector<double> prank(n);
    for(int i = 0;i < n;i++){
        double k = 0;
        for(int j = 0;j < p;j++){
            k += pcm[i][j];
        }
        prank[i] = (k/p) * (out[i]);
    }
    // for(int i = 0;i < n;i++) cout << prank[i] << endl;
    priority_queue<pair<double,int>> pq;
    for(int i = 0;i < n;i++){
        pq.push({prank[i],-(i + 1)});
    }
    vector<int> seq(n);
    for(int i = 0;i < n;i++){
        int y = -(pq.top().second);
        seq[i] = y;
        pq.pop();
    }
    seq = readyOrder(seq, commc);
    // seq = {1,5,4,6,2,8,3,7,9,10};
    for(int i = 0;i < n;i++) cout << seq[i] << ",";
    cout << endl;
    vector<vector<int>> task(n,vector<int> (3));
    vector<int> avail(p,0);
    performtask(task,seq,avail,compc,commc,pcm);
    cout << "TASKS\tST\tFT\tPRO\n";
    for(int i = 0;i < n;i++){
        cout << i + 1 << "\t" << task[i][0] << "\t" << task[i][1] << "\t" << task[i][2] + 1 << endl;
    }
}