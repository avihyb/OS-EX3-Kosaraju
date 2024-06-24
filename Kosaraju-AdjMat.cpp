#include <iostream>
#include <vector>
#include <stack>

using namespace std;

void dfs(int node, vector<bool>& visited, vector<vector<int>>& adjMat, stack<int>& st) {
    visited[node] = true;
    int n = adjMat.size();
    for (int neighbor = 0; neighbor < n; ++neighbor) {
        if (adjMat[node][neighbor] && !visited[neighbor]) {
            dfs(neighbor, visited, adjMat, st);
        }
    }
    st.push(node);
}

void dfsReverse(int node, vector<bool>& visited, vector<vector<int>>& reverseAdjMat, vector<int>& component) {
    visited[node] = true;
    component.push_back(node);
    int n = reverseAdjMat.size();
    for (int neighbor = 0; neighbor < n; ++neighbor) {
        if (reverseAdjMat[node][neighbor] && !visited[neighbor]) {
            dfsReverse(neighbor, visited, reverseAdjMat, component);
        }
    }
}

vector<vector<int>> kosaraju(int n, int m, vector<pair<int, int>>& edges) {
    // Create adjacency matrix with zeros
    vector<vector<int>> adjMat(n, vector<int>(n, 0));
    vector<vector<int>> reverseAdjMat(n, vector<int>(n, 0));

    // Populate adjacency matrix from edges
    for (auto edge : edges) {
        int u = edge.first - 1;  // Adjust for zero-based indexing
        int v = edge.second - 1; // Adjust for zero-based indexing
        adjMat[u][v] = 1;
        reverseAdjMat[v][u] = 1; // Reverse for reverse graph
    }

    vector<bool> visited(n, false);
    stack<int> st;
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfs(i, visited, adjMat, st);
        }
    }

    vector<vector<int>> stronglyConnectedComponents;
    fill(visited.begin(), visited.end(), false);
    while (!st.empty()) {
        int node = st.top();
        st.pop();
        if (!visited[node]) {
            vector<int> component;
            dfsReverse(node, visited, reverseAdjMat, component);
            stronglyConnectedComponents.push_back(component);
        }
    }

    return stronglyConnectedComponents;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<pair<int, int>> edges(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        edges[i] = make_pair(u, v);
    }

    vector<vector<int>> scc = kosaraju(n, m, edges);

    // Print the strongly connected components
    for (const auto& component : scc) {
        for (int node : component) {
            cout << node + 1 << " ";  // Adjust for 1-based indexing in output
        }
        cout << endl;
    }

    return 0;
}
