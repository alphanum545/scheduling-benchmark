// read_only.cpp - timing reference: reads a d3 instance exactly as the schedulers do and does nothing else.
#include <bits/stdc++.h>
using namespace std;
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    long long n, p; if (!(cin >> n >> p)) return 0;
    long long x, s = 0;
    for (long long i = 0; i < n * p + n * n && (cin >> x); i++) s += x;
    cout << (s & 1) << "\n";
}
