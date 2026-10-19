#include <bits/stdc++.h>
using namespace std;

void perform(const vector<vector<int>> &commc,const vector<vector<int>> &compc,vector<vector<int>> &task,const vector<int> &seq,vector<int> &avail,const vector<vector<int>> &oct,const vector<vector<double>> &k){
    for(int i = 0;i < seq.size();i++){
        int eft = 0;
        double meft,minmeft = DBL_MAX;
        for(int j = 0;j < avail.size();j++){
            int est = 0;
            for(int k = 0;k < seq.size();k++){
                if(commc[k][seq[i] - 1] != -1){
                    if(j == task[k][2]) est = max(est,task[k][1]);
                    else est = max(est,task[k][1] + commc[k][seq[i] - 1]);
                }
            }
            est = max(est,avail[j]);
            eft = est + compc[seq[i] - 1][j];
            meft = eft + oct[seq[i] - 1][j] * k[seq[i] - 1][j];
            if(meft < minmeft){
                minmeft = meft;
                task[seq[i] - 1][0] = est;
                task[seq[i] - 1][1] = eft;
                task[seq[i] - 1][2] = j;
            }
        }
        avail[task[seq[i] - 1][2]] = task[seq[i] - 1][1];
    }
    
}


void compoff(vector<int> &fin,int ind,const vector<vector<int>> &commc,vector<set<int>> &off){
    set<int> temp;
    for(int i = 0;i < commc.size();i++){
        if(commc[ind][i] != -1){
        if(fin[i] != 1){
            compoff(fin,i,commc,off);
            fin[i] = 1;
        }
        temp.insert(i + 1);
        if(off[i].size() != 0){
            for(auto it : off[i]){
                temp.insert(it);
            }
        }
        }
    }
    off[ind] = temp;
}

void getoff(vector<int> &fin,const vector<vector<int>> &commc,vector<set<int>> &off){
    for(int i = 0;i < fin.size();i++){
        if(fin[i] == 0){
            compoff(fin,i,commc,off);
            fin[i] = 1;
        }
    }
}

void findDCT(vector<int> &dct,const vector<vector<int>> &commc,const vector<vector<int>> &compc){
    for(int i = 0;i < commc.size();i++){
        int mini = INT_MAX,sum = 0;
        for(int j = 0;j < compc[0].size();j++){
            mini = min(mini,compc[i][j]);
        }
        for(int j = 0;j < commc.size();j++){
            if(commc[i][j] != -1){
                sum += commc[i][j];
            }
        }
        sum += mini;
        dct[i] = sum;
    }
}

void calrank(const vector<set<int>>  &off,vector<int> &dct,vector<int> &rank){
    for(int i = 0;i < off.size();i++){
        int sum = dct[i];
        for(auto k : off[i]) sum += dct[k-1];
        rank[i] = sum;
    }
}

void findOCT(vector<vector<int>> &oct,const vector<vector<int>> &compc,const vector<vector<int>> &commc,vector<int> &fin,int ind,vector<vector<int>> &cps){
    for(int i = 0;i < compc[0].size();i++){
        int moct = 0;
        for(int j = 0;j < commc.size();j++){
            if(commc[ind][j] != -1){
                if(fin[j] != 1){
                    findOCT(oct,compc,commc,fin,j,cps);
                    fin[j] = 1;
                }
                int minoct = INT_MAX;
                for(int k = 0;k < compc[0].size();k++){
                    if(i != k) minoct = min(minoct,{oct[j][k] + compc[j][k] + commc[ind][j]});
                    else minoct = min(minoct, {oct[j][k] + compc[j][k]});
                }
                if(moct < minoct) {
                    moct = minoct;
                    cps[ind][i] = j;
                }
            }
        }
        oct[ind][i] = moct;
    }
}

void getOCT(vector<vector<int>> &oct,const vector<vector<int>> &compc,const vector<vector<int>> &commc,vector<int> &fin,vector<vector<int>> &cps){
    for(int i = 0;i < fin.size();i++){
        if(fin[i] == 0){
            findOCT(oct,compc,commc,fin,i,cps);
            fin[i] = 1;
        }
    }
}


int main(){
    int n,p;
    cin >> n;
    cin >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<vector<int>> compc(n, vector<int> (p));
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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            cin >> compc[i][j];
        }
    }
    vector<vector<int>> commc(n, vector<int> (n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> commc[i][j];
        }
    }
    vector<int> dct(n);
    findDCT(dct,commc,compc);
    vector<int> fin(n,0);
    vector<set<int>> off(n);
    for(int i = 0;i < n;i++){
        if(accumulate(commc[i].begin(),commc[i].end(),0) == -n) fin[i] = 1;
    }
    getoff(fin,commc,off);
    vector<int> rank(n);
    calrank(off,dct,rank);
    priority_queue<pair<int,int>> pq;
    for(int i = 0;i < n;i++){
        pq.push({rank[i],i + 1});
    }
    vector<int> seq(n);
    int  i = 0;
    while(!pq.empty()){
        seq[i] = pq.top().second;
        pq.pop();
        i++;
    }
    // seq = {1,4,6,2,3,5,8,7,9,10};
    vector<vector<int>> oct(n,vector<int>(p,0)),cps(n,vector<int> (p,0));
    vector<vector<double>> kval(n,vector<double> (p,0));
    fin = vector<int> (n,0);
    for(int i = 0;i < n;i++){
        if(accumulate(commc[i].begin(),commc[i].end(),0) == -n) fin[i] = 1;
    }
    for(int i = 0;i < n;i++) cout << seq[i] << ",";
    cout << endl;
    getOCT(oct,compc,commc,fin,cps);
    for(int i = 0;i < n;i++){
        int count = 0;
        double y = 0;
        for(int j = 0;j < n;j++){
            if(commc[i][j] != -1) {
                count++;
                y += (commc[i][j] + rank[j]);
            }
        }
        for(int j = 0;j < p;j++){
            if(count > p + 1){
                double x = rank[cps[i][j]];
                double z = x / y;
                kval[i][j] = ceil(z * 10)/10.0;
            }
            else kval[i][j] = 1;
        }
    }
    // cout << "K values:\n";
    // for(int i = 0;i < n;i++){
    //     for(int j = 0;j < p;j++) cout << kval[i][j] << " ";
    //     cout << endl;
    // }
    // cout << "OCT values:\n";
    // for(int i = 0;i < n;i++){
    //     for(int j = 0;j < p;j++) cout << oct[i][j] << " ";
    //     cout << endl;
    // }
    vector<vector<int>> task(n,vector<int> (3));
    vector<int> avail(p,0);
    perform(commc,compc,task,seq,avail,oct,kval);
    cout << "TASKS\tAST\tAFT\tPRO\n";
    for(int i = 0;i < n;i++){
        cout << i + 1 << "\t" << task[i][0] << "\t" << task[i][1] << "\t" << task[i][2] + 1 << endl;
    }
}