#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <stack>
#include <algorithm>

using namespace std;

unordered_map<int, unordered_set<int>> graph;
vector<int> degeneracy_order;
unordered_map<int, int> vertex_pos;
int max_clique_size = 0;

void load_graph(const string& filename) {
    ifstream file(filename);
    int u, v;
    while (file >> u >> v) {
        graph[u].insert(v);
        graph[v].insert(u);
    }
    file.close();
}

void compute_degeneracy_order() {
    unordered_map<int, int> degree;
    for (const auto& [v, neighbors] : graph) {
        degree[v] = neighbors.size();
    }

    set<pair<int, int>> bucket;
    for (const auto& [v, d] : degree) {
        bucket.insert({d, v});
    }

    while (!bucket.empty()) {
        auto [d, v] = *bucket.begin();
        bucket.erase(bucket.begin());
        degeneracy_order.push_back(v);
        for (int u : graph[v]) {
            if (degree.count(u)) {
                bucket.erase({degree[u], u});
                degree[u]--;
                bucket.insert({degree[u], u});
            }
        }
        degree.erase(v);
    }

    for (int i = 0; i < degeneracy_order.size(); ++i) {
        vertex_pos[degeneracy_order[i]] = i;
    }
}

void bron_kerbosch(vector<int>& R, unordered_set<int>& P, unordered_set<int>& X, ofstream& out) {
    if (P.empty() && X.empty()) {
        for (int v : R) out << v << " ";
        out << "\n";
        if (R.size() > max_clique_size) {
            max_clique_size = R.size();
        }
        return;
    }

    int pivot = -1, max_count = -1;
    unordered_set<int> P_union_X = P;
    P_union_X.insert(X.begin(), X.end());

    for (int u : P_union_X) {
        int count = 0;
        for (int v : P) {
            if (graph[u].count(v)) count++;
        }
        if (count > max_count) {
            max_count = count;
            pivot = u;
        }
    }

    unordered_set<int> diff;
    for (int v : P) {
        if (!graph[pivot].count(v)) diff.insert(v);
    }

    for (int v : diff) {
        R.push_back(v);

        unordered_set<int> newP, newX;
        for (int u : P) {
            if (graph[v].count(u)) newP.insert(u);
        }
        for (int u : X) {
            if (graph[v].count(u)) newX.insert(u);
        }

        bron_kerbosch(R, newP, newX, out);

        R.pop_back();
        P.erase(v);
        X.insert(v);
    }
}

void list_cliques(const string& output_file) {
    ofstream out(output_file);
    for (int v : degeneracy_order) {
        unordered_set<int> P, X;
        for (int u : graph[v]) {
            if (vertex_pos[u] > vertex_pos[v]) P.insert(u);
            else X.insert(u);
        }
        vector<int> R;
        R.push_back(v);
        bron_kerbosch(R, P, X, out);
    }
    out.close();
}

#include <chrono>

int main() {
    load_graph("input.txt");
    auto start = chrono::high_resolution_clock::now();
    compute_degeneracy_order();
    list_cliques("maximal_cliques.txt");
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Maximal cliques written to maximal_cliques.txt\n";
    cout << "Maximum clique size: " << max_clique_size << endl;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;
    return 0;
}
