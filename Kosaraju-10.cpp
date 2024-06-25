#include <iostream>
#include <pthread.h>
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
#include <thread>
#include <mutex>
#include "reactor.hpp"
#include "proactor.hpp"
#include <map> // Include map for storing client information

/*
Part 10:
Usage of POSIX condition to handle the case where the majority of the graph is strongly connected.
 */
// Define a structure to store client information
struct ClientInfo {
    int client_fd;
    pthread_t thread_id;
};


#define PORT "9034"

using namespace std;

vector<vector<int>> adjMat;
mutex adjMatMutex; // Mutex to protect the adjacency matrix since it is shared among threads.
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexCondition = PTHREAD_MUTEX_INITIALIZER;
bool mostGraphConnected = false;
bool wasMajority = false;
bool notLongerMajority = false;


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
        int bytes_received = recv(client_fd, buf, sizeof(buf), 0);
        if (bytes_received <= 0) {
            cerr << "Error receiving graph input from client" << endl;
            break;
        }
        buf[bytes_received] = '\0';  // Null-terminate the string
        int u, v;
        sscanf(buf, "%d %d", &u, &v);
        adj[u-1][v-1] = 1; // Assuming edges are 1-based, adjust to 0-based indexing
    }
    send(client_fd, "Graph input received\n", 21, 0);
    return adj;
}

void printStronglyConnectedComponents(const vector<vector<int>>& scc, int client_fd) {
    bool foundMajority = false;
    
    pthread_mutex_lock(&mutexCondition);
    for (int i = 0; i < scc.size(); ++i) {
        if (scc[i].size() > adjMat.size() / 2) {
            mostGraphConnected = true;
            wasMajority = true;
            foundMajority = true;
            pthread_cond_signal(&cond); 
            break;
        }
    }
    std::cout << "foundMajority: " << foundMajority << std::endl;
    std::cout << "wasMajority: " << wasMajority << std::endl;

    if(foundMajority == false && wasMajority == true){
        notLongerMajority = true;
        pthread_cond_signal(&cond); 
    }

    pthread_mutex_unlock(&mutexCondition);



        

    
    // Print the strongly connected components
    for (const auto& component : scc) {
        string result;

        for (int node : component) {
            result += to_string(node + 1) + " ";  // Adjust for 1-based indexing in output
        }
        result += "\n";
        send(client_fd, result.c_str(), result.size(), 0);
    }

    pthread_mutex_unlock(&mutexCondition);
}

void HandleCommand(const string& command, int client_fd) {
        cout << "Received command: " << command << endl;
        // cout << "isMajority: " << mostGraphConnected << endl;
        // cout << "wasMajority: " << wasMajority << endl;
        // cout << "notLongerMajority: " << notLongerMajority << endl;
        

    try {
        
        if (command.substr(0, 8) == "Newgraph") {
            cout << "Processing Newgraph command" << endl;
            size_t pos = 9;
            size_t next_space = command.find(" ", pos);
            int n = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int m = stoi(command.substr(pos));
            lock_guard<mutex> lock(adjMatMutex); // Lock the adjacency matrix while updating it.
            adjMat = inputGraph(n, m, client_fd);
        } else if (command.substr(0, 8) == "Kosaraju") {
            cout << "Processing Kosaraju command" << endl;
            lock_guard<mutex> lock(adjMatMutex);
            vector<vector<int>> scc = kosaraju(adjMat.size());
            printStronglyConnectedComponents(scc, client_fd);
        } else if (command.substr(0, 7) == "Newedge") {
            cout << "Processing Newedge command" << endl;
            size_t pos = 8;
            size_t next_space = command.find(" ", pos);
            int u = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int v = stoi(command.substr(pos));
            {
                lock_guard<mutex> lock(adjMatMutex);
                adjMat[u-1][v-1] = 1;
            }
            send(client_fd, "Edge added\n", 11, 0);
        } else if (command.substr(0, 10) == "Removeedge") {
            cout << "Processing Removeedge command" << endl;
            size_t pos = 11;
            size_t next_space = command.find(" ", pos);
            int u = stoi(command.substr(pos, next_space - pos));
            pos = next_space + 1;
            int v = stoi(command.substr(pos));
            {
                lock_guard<mutex> lock(adjMatMutex);
                adjMat[u-1][v-1] = 0;
            }
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

void* proactorFunction(int client_fd) {
    char buf[256];
    while (true) {
        int nbytes = recv(client_fd, buf, sizeof(buf), 0);
        if (nbytes <= 0) {
            if (nbytes == 0) {
                cout << "Client " << client_fd << " disconnected" << endl;
            } else {
                perror("recv");
            }
            close(client_fd);
            break;
        } else {
            string command(buf, nbytes);
            HandleCommand(command, client_fd);
        }
    }
    return nullptr;
}

void* majorityFunction(void* arg) {
    while(true){
        pthread_mutex_lock(&mutexCondition);
        
        // Wait until either `mostGraphConnected` or `notLongerMajority` is true
        while (!mostGraphConnected && !notLongerMajority) {
            pthread_cond_wait(&cond, &mutexCondition);
        }

        // Local copies of the conditions for printing and logic
        bool localMostGraphConnected = mostGraphConnected;
        bool localNotLongerMajority = notLongerMajority;

        // Reset conditions if needed
        if (mostGraphConnected) {
            mostGraphConnected = false;
            wasMajority = true;
        }

        if (notLongerMajority) {
            notLongerMajority = false;
            wasMajority = false;
        }

        pthread_mutex_unlock(&mutexCondition);
        
        if (localMostGraphConnected) {
            cout << "At least 50% of the graph belongs to the same SCC\n";
        }

        if (localNotLongerMajority) {
            cout << "At least 50% of the graph NO LONGER belongs to the same SCC\n";
        }
    }
    
    return nullptr;
}



int main() {
    int listener = get_listener_socket();
    if (listener == -1) {
        cerr << "Error getting listener socket" << endl;
        return 1;
    }

    // Reusable set of file descriptors
    fd_set master;
    fd_set read_fds;
    int fdmax;

    std::map<int, ClientInfo> clients;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(listener, &master);
    fdmax = listener;
    pthread_t t;
    pthread_create(&t, nullptr, majorityFunction, nullptr); // additional thread to check if the majority of the graph is strongly connected

    cout << "Waiting for connections..." << endl;


    while (true) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            cerr << "Select error" << endl;
            return 1;
        }

        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    struct sockaddr_storage remoteaddr;
                    socklen_t addrlen = sizeof remoteaddr;
                    int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                    if (newfd == -1) {
                        cerr << "Accept error" << endl;
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
                        cout << "New connection from " << inet_ntoa(((struct sockaddr_in *)&remoteaddr)->sin_addr) << " on socket " << newfd << endl;

                        // Start a proactor thread
                        pthread_t tid = startProactor(newfd, proactorFunction);
                        if (tid == 0) {
                            cerr << "Failed to start proactor thread" << endl;
                        } else {
                            clients[newfd] = {newfd, tid};
                        }
                    }
                } else {
                    char buf[256];
                    int bytes_received = recv(i, buf, sizeof buf, 0);
                    if (bytes_received <= 0) {
                        if (bytes_received == 0) {
                            
                            cout << "Socket " << i << " hung up" << endl;
                        } else {
                            cerr << "Receive error on socket " << i << endl;
                        }
                        close(i);
                        FD_CLR(i, &master);

                         // Find and stop the corresponding proactor thread
                        auto it = clients.find(i);
                        if (it != clients.end()) {
                            stopProactor(it->second.thread_id);
                            clients.erase(it); // Erase client info from map
                        }

                    } else {
                        buf[bytes_received] = '\0';
                        string command(buf);
                        HandleCommand(command, i);
                    }
                }
            }
        }
    }


  for (const auto& client : clients) {
        stopProactor(client.second.thread_id);
    }
   
     pthread_join(t, NULL);

    return 0;
}
