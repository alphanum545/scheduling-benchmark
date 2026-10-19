#include <bits/stdc++.h>
using namespace std;

bool mycomp(pair<long,int> a,pair<long,int> b) {
    if(a.second == b.second) return a.first < b.first;
    return a.second > b.second;
}

long calrp(int tind,vector<int> &fin,const vector<vector<int>> &commc,const vector<double> &mw,vector<long> &rup) {
    long k = 0,maxi = 0;
    for(int i = 0;i < fin.size(); i++) {
        if(commc[tind][i] != -1) {
            if(fin[i] == 0) {
                rup[i] = calrp(i,fin,commc,mw,rup);
                fin[i] = 1;
            }
            k = commc[tind][i] * 10000 + rup[i] + mw[i] * 10000;
            maxi = max(maxi,k);
        }
    }
    return maxi;
}

void cal(vector<int> &fin,const vector<vector<int>> &commc,const vector<double> &mw,vector<long> &rup) {
    for(int i = 0;i < fin.size();i++) {
        if(fin[i] == 0) {
            rup[i] = calrp(i,fin,commc,mw,rup);
            fin[i] = 1;
        }
    }
}

void performphase(vector<int> &aft, vector<int> &ast, vector<int> &pros, vector<int> &avail,const vector<vector<int>> &commc,const vector<int> &seq,const vector<vector<int>> &compc,const vector<int> &tdp) {
    for(int i = 1;i < seq.size();i++) {
        int ans1, ans2, ans3,min_eft = INT_MAX;
        for(int j = 0;j < avail.size();j++) {
            int st,est = 0,eft = 0,max_est = 0, pro;
            int fg = 0;
            for(int k = 0;k < seq.size();k++) {
                if(commc[k][seq[i] - 1] != -1) {
                    fg = 1;
                    if(pros[k] != j + 1 && !(k == seq[0] - 1 && tdp[j] == 1)) {
                        est = max(avail[j] ,commc[k][seq[i] - 1] + aft[k]);
                    }
                    else est = max(avail[j] , aft[k]);
                }
                else if(!fg) est = avail[j];
                if(max_est < est) {
                    max_est = est;
                }
            }
            eft = max_est + compc[seq[i] - 1][j];
            if(eft < min_eft  || (eft == min_eft && abs(avail[pro - 1] - avail[j]) > abs((eft - max_est) - (ans1 - ans2)))) {
                pro = j + 1;
                st = max_est;
                min_eft = eft;
                ans1 = min_eft;
                ans2 = st;
                ans3 = pro;
            }
        }
        aft[seq[i] - 1] = ans1;
        ast[seq[i] - 1] = ans2;
        pros[seq[i] - 1] = ans3;
        avail[ans3 - 1] = ans1;
     }
}

int main() {
    int n,p;
    cin >> n >> p;
    if (n == 0 || p == 0)
    {
        cout << 0.0 << endl;
        return 0;
    }
    vector<vector<int>> compc(n,vector<int> (p));
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
    for(int i = 0;i < n;i++) {
        for(int j = 0;j < p;j++) cin >> compc[i][j];
    }
    vector<int> seq(n);
    vector<vector<int>> commc(n,vector<int> (n));
    for(int i = 0;i < n;i++) {
        for(int j = 0;j < n;j++) cin >> commc[i][j];
    }
    for(int i = 0;i < n;i++){
        for(int j = 0;j < n;j++) {
            // cout << commc[i][j] << " ";
        }
    }
    vector<double> mw(n);
    vector<int> fin(n,0);
    for(int i = 0;i < n;i++) {
        double y = 0,x = INT_MAX;
        for(int j = 0;j < p;j++) {
            x = (x > compc[i][j]) ? compc[i][j] : x;
            y = (y < compc[i][j]) ? compc[i][j] : y;
        }
        double num = (y/x);
        mw[i] = (y - x) / num;
    }
    vector<long> rup(n,0);
    for(int i = 0;i < n;i++) {
        int count = 0;
        for(int j = 0;j < n;j++) {
            if(commc[i][j] == -1) count++;
        }
        if(count == n) {
            fin[i] = 1;
        }
    }
    cal(fin,commc,mw,rup);
    for(int i = 0;i < n;i++) {
        double h = (double )rup[i]/10;
        rup[i] = round(h);
    }
    vector<pair<int,long>> pseq;
    for(int i = 0;i < n;i++) pseq.push_back(pair<int,long> (i + 1,rup[i]));
    sort(pseq.begin(),pseq.end(),mycomp);
    for(int i = 0;i < n;i++) {
        seq[i] = pseq[i].first;
    }
    vector<int> aft(n,0),ast(n,0),pros(n,0),avail(p,0);
    int as = 0,ef = 0, minef = INT_MAX, pro;
    if(n > 0){
    for(int i = 0;i < p; i++) {
        if(minef > compc[seq[0] - 1][i]) {
            minef = compc[seq[0] - 1][i];
            pro = i;
        }
    }
    }
    // seq = {1,4,6,2,3,5,8,7,9,10};
    // seq = {8,6,4,1,3,2,7,5,30,29,26,34,31,32,24,33,25,27,15,19,12,23,13,28,36,10,21,20,18,14,17,16,35,9,11,22,37,38,42,43,44,39,40,41,46,45,47,48,49,50};
    // seq = {8,1,4,6,2,3,38,7,5,28,30,29,32,27,26,18,20,21,25,24,33,34,13,10,19,14,31,23,9,16,15,17,12,36,11,35,22,37,43,42,44,41,39,45,40,46,47,48,49,50};
    // seq = {1,5,2,4,8,6,3,7,38,9,23,21,35,25,13,29,30,33,10,18,27,28,14,31,16,36,20,15,34,12,26,22,24,32,11,17,37,19,45,42,46,41,40,39,43,47,44,48,49,50};
    vector<int> tdp(p,0);
    if(n > 0){
    for(int i = 0;i < p && i != pro;i++) {
        int fg = 0;
        for(int j = 0;j < n;j++) {
            if(commc[seq[0] - 1][j] != -1 && (compc[seq[0] - 1][i] >= (compc[seq[0] - 1][pro] + commc[seq[0] - 1][j]))) fg = 1;
        }
        if(!fg) {
            tdp[i] = 1;
        }
    }
    ast[seq[0] - 1] = 0,aft[seq[0] - 1] = compc[seq[0] - 1][pro];
    pros[seq[0] - 1] = pro + 1;
    avail[pro] = aft[seq[0] - 1];
    for(int i = 0;i < p;i++){
        if(tdp[i] == 1) avail[i] = compc[seq[0] - 1][i];
    }
    }
    for(int i = 0;i < n;i++) cout << seq[i] << ",";
    cout << endl;
    performphase(aft,ast,pros,avail,commc,seq,compc,tdp);
    cout << "Tasks\tAST\tAFT\tProcessor\n";
    for(int i = 0;i < n;i++) {
        cout << i + 1 << "\t" << ast[i] << "\t" << aft[i] << "\t" << pros[i] <<endl;
    }
}