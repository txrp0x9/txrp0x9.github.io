#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<set<int>> g;
vector<int> s, t;
long long maxSize = 0, total = 0;
unordered_map<int, int> dist;
vector<vector<int>> cliques;

void update(int i, set<int>& cur) {
    if (i > n) return;
    if (i == n) {
        ++total;
        int sz = cur.size();
        ++dist[sz];
        if (sz > maxSize) maxSize = sz;
        cliques.push_back(vector<int>(cur.begin(), cur.end()));
        if (total % 500 == 0) cout << total << " cliques found...\n";
        return;
    }

    vector<int> in, out, adj(g[i].begin(), g[i].end());
    auto it = cur.begin(); int j = 0;
    while (it != cur.end() && j < adj.size()) {
        if (*it == adj[j]) in.push_back(*it), ++it, ++j;
        else if (*it < adj[j]) out.push_back(*it), ++it;
        else ++j;
    }
    while (it != cur.end()) out.push_back(*it), ++it;

    if (!out.empty()) update(i + 1, cur);

    for (int x : in) for (int y : g[x]) if (!cur.count(y) && y != i) t[y]++;
    for (int x : out) for (int y : g[x]) if (!cur.count(y)) s[y]++;

    bool ok = true;
    for (int y : g[i]) if (!cur.count(y) && y < i && t[y] == in.size()) { ok = false; break; }

    if (ok) {
        vector<int> outIdx = out; sort(outIdx.begin(), outIdx.end());
        for (int k = 1; k <= outIdx.size(); ++k) {
            int jk = outIdx[k - 1]; bool flag = false;
            for (int y : g[jk]) {
                if (!cur.count(y) && y < i && t[y] == in.size()) {
                    if (y >= jk) s[y]--;
                    else if (!flag) {
                        if (s[y] + k - 1 == outIdx.size() && ((k - 1 == 0 && y >= 0) || y >= outIdx[k - 2])) {
                            ok = false; flag = true;
                        }
                    }
                }
            }
        }

        if (ok) {
            int jp = outIdx.empty() ? 0 : outIdx.back();
            if (!in.empty()) {
                for (int y = 0; y < i; ++y)
                    if (!cur.count(y) && y != i && t[y] == in.size() && s[y] == 0 && jp < y)
                        ok = false;
            } else if (jp < i - 1) ok = false;
        }
    }

    for (int x : in) for (int y : g[x]) if (!cur.count(y) && y != i) t[y] = 0;
    for (int x : out) for (int y : g[x]) if (!cur.count(y)) s[y] = 0;

    if (ok) {
        set<int> backup(out.begin(), out.end());
        cur.clear();
        cur.insert(in.begin(), in.end());
        cur.insert(i);
        update(i + 1, cur);
        cur.erase(i);
        cur.insert(backup.begin(), backup.end());
    }
}

void findCliques() {
    int start = 0;
    while (start < n && g[start].empty()) ++start;
    s.assign(n, 0); t.assign(n, 0);
    set<int> cur = {start};
    update(start + 1, cur);
}

int main() {
    ifstream in("input.txt");
    cin.rdbuf(in.rdbuf());

    in >> n >> m;
    g.resize(n);
    for (int i = 0; i < m; ++i) {
        int u, v; in >> u >> v;
        g[u].insert(v); g[v].insert(u);
    }

    vector<pair<int, int>> deg;
    for (int i = 0; i < n; ++i) deg.emplace_back(g[i].size(), i);
    sort(deg.begin(), deg.end());
    unordered_map<int, int> re;
    for (int i = 0; i < n; ++i) re[deg[i].second] = i;

    vector<set<int>> newG(n);
    for (int i = 0; i < n; ++i)
        for (int x : g[i]) newG[re[i]].insert(re[x]);
    g = move(newG);

    auto st = chrono::high_resolution_clock::now();
    findCliques();
    auto ed = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(ed - st).count();

    cout << "Execution time: " << ms << " ms\n";
    cout << "Largest clique size: " << maxSize << "\n";
    cout << "Clique size distribution:\n";
    for (auto& [sz, cnt] : dist)
        cout << "  Size " << sz << ": " << cnt << "\n";
    return 0;
}
