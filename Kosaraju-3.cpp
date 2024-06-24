#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

vector<vector<int>> adjMat; // Global adjacency matrix

void dfs(int node, vector<bool>& visited, stack<int>& st) {
    visited[node] = true;
    int n = adjMat.size();
    for (int neighbor = 0; neighbor < n; ++neighbor) {
        if (adjMat[node][neighbor] && !visited[neighbor]) {
            dfs(neighbor, visited, st);
        }
    }
    st.push(node);
}

void dfsReverse(int node, vector<bool>& visited, vector<int>& component) {
    visited[node] = true;
    component.push_back(node);
    int n = adjMat.size();
    for (int neighbor = 0; neighbor < n; ++neighbor) {
        if (adjMat[neighbor][node] && !visited[neighbor]) {
            dfsReverse(neighbor, visited, component);
        }
    }
}

vector<vector<int>> kosaraju(int n) {
    vector<bool> visited(n, false);
    stack<int> st;
    // Phase 1: Perform DFS on the original graph
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            dfs(i, visited, st);
        }
    }

    // Create reverse adjacency matrix
    vector<vector<int>> reverseAdjMat(n, vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            reverseAdjMat[i][j] = adjMat[j][i];
        }
    }

    // Phase 2: Perform DFS on the reverse graph based on finish times
    fill(visited.begin(), visited.end(), false);
    vector<vector<int>> stronglyConnectedComponents;
    while (!st.empty()) {
        int node = st.top();
        st.pop();
        if (!visited[node]) {
            vector<int> component;
            dfsReverse(node, visited, component);
            stronglyConnectedComponents.push_back(component);
        }
    }

    return stronglyConnectedComponents;
}

vector<vector<int>> inputGraph(int n, int m) {
    vector<vector<int>> adj(n, vector<int>(n, 0));
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u-1][v-1] = 1; // Assuming edges are 1-based, adjust to 0-based indexing
    }
    return adj;
}

void printStronglyConnectedComponents(vector<vector<int>>& scc) {
    // Print the strongly connected components
    for (const auto& component : scc) {
        for (int node : component) {
            cout << node + 1 << " ";  // Adjust for 1-based indexing in output
        }
        cout << endl;
    }
}

int main() {
    string command;

    while (true) {

        getline(cin, command);

       if(command.substr(0,8) == "Newgraph"){
        int n = stoi(command.substr(9, command.find(" ", 9) - 9));
        int m = stoi(command.substr(command.find(" ", 9) + 1));

        adjMat = inputGraph(n, m);

       }

       else if (command.substr(0, 8) == "Kosaraju") {
        cout << "KOS!" << endl;
        vector<vector<int>> scc = kosaraju(adjMat.size());
        printStronglyConnectedComponents(scc);
       }

       else if (command.substr(0, 7) == "Newedge") {
        
        int u = stoi(command.substr(8, command.find(" ", 8) - 8));
        int v = stoi(command.substr(command.find(" ", 8) + 1));

        adjMat[u-1][v-1] = 1;
       }

       else if (command.substr(0, 9) == "Removeedge") {
        
        int u = stoi(command.substr(10, command.find(" ", 10) - 10));
        int v = stoi(command.substr(command.find(" ", 10) + 1));

        adjMat[u-1][v-1] = 0;
       }

       else if (command == "Quit") {
        break;
       }

       

       
    }

    return 0;
}
