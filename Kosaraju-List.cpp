#include <iostream>
#include <vector>
#include <stack>
#include <list>

using namespace std;

void dfs(int node, vector<bool>& visited, vector<list<int>>& graph, stack<int>& st) {
    visited[node] = true;
    for (int neighbor : graph[node]) {
        if (!visited[neighbor]) {
            dfs(neighbor, visited, graph, st);
        }
    }
    st.push(node);
}

void dfsReverse(int node, vector<bool>& visited, vector<list<int>>& reverseGraph, vector<int>& component) {
    visited[node] = true;
    component.push_back(node);
    for (int neighbor : reverseGraph[node]) {
        if (!visited[neighbor]) {
            dfsReverse(neighbor, visited, reverseGraph, component);
        }
    }
}

vector<vector<int>> kosaraju(int n, int m, list<pair<int, int>>& edges) {
    vector<list<int>> graph(n);
    vector<list<int>> reverseGraph(n);
    for (auto edge : edges) {
        int u = edge.first - 1;  // Adjust for zero-based indexing
        int v = edge.second - 1; // Adjust for zero-based indexing
        graph[u].push_back(v);
        reverseGraph[v].push_back(u);
    }

    vector<bool> visited(n, false);
    stack<int> st;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs(i, visited, graph, st);
        }
    }

    vector<vector<int>> stronglyConnectedComponents;
    fill(visited.begin(), visited.end(), false);
    while (!st.empty()) {
        int node = st.top();
        st.pop();
        if (!visited[node]) {
            vector<int> component;
            dfsReverse(node, visited, reverseGraph, component);
            stronglyConnectedComponents.push_back(component);
        }
    }

    return stronglyConnectedComponents;
}

int main() {
    int n, m;
    cin >> n >> m;

    list<pair<int, int>> edgesList;
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        edgesList.push_back(make_pair(u, v));
    }

    vector<vector<int>> scc = kosaraju(n, m, edgesList);

    // Print the strongly connected components
    for (const auto& component : scc) {
        for (int node : component) {
            cout << node + 1 << " ";  // Adjust for 1-based indexing in output
        }
        cout << endl;
    }

    return 0;
}
