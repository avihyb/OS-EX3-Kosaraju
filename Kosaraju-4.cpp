#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define PORT "9034"

using namespace std;

vector<vector<int>> adjMat;

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

vector<vector<int>> inputGraph(int n, int m, int client_fd) {
    vector<vector<int>> adj(n, vector<int>(n, 0));
    for (int i = 0; i < m; ++i) {
        char buf[256];
        int bytes_received = recv(client_fd, buf, sizeof buf, 0);
        if (bytes_received <= 0) {
            cerr << "Error receiving graph input from client" << endl;
            break;
        }
        buf[bytes_received] = '\0';  // Null-terminate the string
        int u, v;
        sscanf(buf, "%d %d", &u, &v);
        adj[u-1][v-1] = 1; // Assuming edges are 1-based, adjust to 0-based indexing
    }
    return adj;
}

void printStronglyConnectedComponents(const vector<vector<int>>& scc, int client_fd) {
    // Print the strongly connected components
    for (const auto& component : scc) {
        string result;
        for (int node : component) {
            result += to_string(node + 1) + " ";  // Adjust for 1-based indexing in output
        }
        result += "\n";
        send(client_fd, result.c_str(), result.size(), 0);
    }
}

void HandleCommand(const string& command, int client_fd) {
    cout << "Received command: " << command << endl;

    try {
        if (command.substr(0, 8) == "Newgraph") {
            cout << "Processing Newgraph command" << endl;
            size_t pos = 9;
            size_t next_space = command.find(" ", pos);
            int n = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int m = stoi(command.substr(pos));
            adjMat = inputGraph(n, m, client_fd);
        } else if (command.substr(0, 8) == "Kosaraju") {
            cout << "Processing Kosaraju command" << endl;
            vector<vector<int>> scc = kosaraju(adjMat.size());
            printStronglyConnectedComponents(scc, client_fd);
        } else if (command.substr(0, 7) == "Newedge") {
            cout << "Processing Newedge command" << endl;
            size_t pos = 8;
            size_t next_space = command.find(" ", pos);
            int u = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int v = stoi(command.substr(pos));
            adjMat[u-1][v-1] = 1;
            send(client_fd, "Edge added\n", 11, 0);
        } else if (command.substr(0, 10) == "Removeedge") {
            cout << "Processing Removeedge command" << endl;
            size_t pos = 11;
            size_t next_space = command.find(" ", pos);
            int u = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int v = stoi(command.substr(pos));
            adjMat[u-1][v-1] = 0;
            send(client_fd, "Edge removed\n", 13, 0);
        } else {
            cout << "Unknown command: " << command << endl;
            send(client_fd, "Unknown command\n", 16, 0);
        }
    } catch (const exception& e) {
        cerr << "Error handling command: " << e.what() << endl;
        send(client_fd, "Error processing command\n", 25, 0);
    }
}



// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket() {
    int listener;     // Listening socket descriptor
    int yes = 1;      // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(vector<pollfd>& pfds, int newfd) {
    pfds.push_back({newfd, POLLIN, 0});
}

// Remove an index from the set
void del_from_pfds(vector<pollfd>& pfds, int i) {
    pfds.erase(pfds.begin() + i);
}

// Main
int main() {
    int listener;     // Listening socket descriptor

    int newfd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[256];    // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    vector<pollfd> pfds;
    pfds.reserve(5);

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        cerr << "error getting listening socket" << endl;
        exit(1);
    }

    // Add the listener to set
    pfds.push_back({listener, POLLIN, 0});

    // Main loop
    for (;;) {
        int poll_count = poll(pfds.data(), pfds.size(), -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (size_t i = 0; i < pfds.size(); i++) {
            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN) { // We got one!!
                if (pfds[i].fd == listener) {
                    // If listener is ready to read, handle new connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr*)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(pfds, newfd);

                        cout << "pollserver: new connection from "
                             << inet_ntop(remoteaddr.ss_family,
                                          get_in_addr((struct sockaddr*)&remoteaddr),
                                          remoteIP, INET6_ADDRSTRLEN)
                             << " on socket " << newfd << endl;
                    }
                } else {
                    // If not the listener, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0) {
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            cout << "pollserver: socket " << sender_fd << " hung up" << endl;
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        del_from_pfds(pfds, i);

                    } else {
                        // We got some good data from a client
                        string command(buf, nbytes);
                        HandleCommand(command, sender_fd);
                    }
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
