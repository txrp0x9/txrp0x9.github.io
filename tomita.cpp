#include <bits/stdc++.h>

using namespace std;

unordered_map<int, unordered_set<int>> graph;  // Adjacency list
int maxSize = -1;

void expand(unordered_set<int> R, unordered_set<int> P, unordered_set<int> X, ofstream &output) {
    if (P.empty() && X.empty()) {
        for (int v : R) output << v << " ";
        output << "\n";
        maxSize = max((int)R.size(), maxSize);
        return;
    }

    int pivot = -1, max_neighbors = -1;
    for (int u : P) {
        int count = 0;
        for (int v : P)
            if (graph[u].count(v)) count++;
        if (count > max_neighbors) { 
            max_neighbors = count;
            pivot = u;
        }
    }

    unordered_set<int> P_copy = P;
    for (int v : P_copy) {
        if (pivot != -1 && graph[pivot].count(v)) continue;

        unordered_set<int> new_R = R, new_P, new_X;
        new_R.insert(v);
        for (int u : P)
            if (graph[v].count(u)) new_P.insert(u);
        for (int u : X)
            if (graph[v].count(u)) new_X.insert(u);
        
        expand(new_R, new_P, new_X, output);

        P.erase(v);
        X.insert(v);
    }
}

void read_graph(const string &filename) {
    ifstream file(filename);
    int u, v;
    while (file >> u >> v) {
        graph[u].insert(v);
        graph[v].insert(u);  // Undirected graph
    }
    file.close();
}

void run_tomita(const string &output_file) {
    ofstream output(output_file);
    unordered_set<int> R, P, X;
    for (const auto &[node, _] : graph) P.insert(node);

    expand(R, P, X, output);
    output.close();
}

int main() {
    string input_file = "input.txt";
    string output_file = "maximal_cliques.txt";

    read_graph(input_file);
    
    auto start = chrono::high_resolution_clock::now();
    run_tomita(output_file);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Maximal cliques saved in " << output_file << endl;
    cout << "Biggest maximal clique " << maxSize << endl;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;
    return 0;
}
