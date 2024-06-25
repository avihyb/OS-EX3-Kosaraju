

#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <stack>
#include <functional>

using reactorFunc = std::function<void(int)>;

class Reactor {
public:
    Reactor();
    ~Reactor();

    void *startReactor();
    int addFdToReactor(int fd, reactorFunc func);
    int removeFdFromReactor(int fd);
    int stopReactor();

private:
    int max_fd;
    fd_set read_set;
    std::unordered_map<int, reactorFunc> fd_callbacks;
};

#endif // REACTOR_H
