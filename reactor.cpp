// reactor.cpp

#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

Reactor::Reactor() : max_fd(-1) {
    FD_ZERO(&read_set);
}

Reactor::~Reactor() {
    stopReactor();
}

void *Reactor::startReactor() {
    std::cout << "Reactor started\n";
    return this;
}

int Reactor::addFdToReactor(int fd, reactorFunc func) {
    if (fd < 0) {
        return -1;
    }

    fd_callbacks[fd] = func;

    if (fd > max_fd) {
        max_fd = fd;
    }

    FD_SET(fd, &read_set);

    std::cout << "Added fd " << fd << " to reactor\n";

    return 0; // Success
}

int Reactor::removeFdFromReactor(int fd) {
    if (fd_callbacks.find(fd) == fd_callbacks.end()) {
        return -1; // FD not found
    }

    fd_callbacks.erase(fd);

    if (fd == max_fd) {
        for (auto const& pair : fd_callbacks) {
            if (pair.first > max_fd) {
                max_fd = pair.first;
            }
        }
    }

    FD_CLR(fd, &read_set);

    std::cout << "Removed fd " << fd << " from reactor\n";

    return 0; // Success
}

int Reactor::stopReactor() {
    fd_callbacks.clear();
    std::cout << "Reactor stopped\n";
    return 0; // Success
}
