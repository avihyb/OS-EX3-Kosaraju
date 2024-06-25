// proactor.cpp

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "proactor.hpp"

void* proactorThreadWrapper(void* arg) {
    auto* data = static_cast<std::pair<int, proactorFunc>*>(arg);
    int client_fd = data->first;
    proactorFunc func = data->second;
    delete data; // Clean up dynamically allocated memory
    return func(client_fd);
}

pthread_t startProactor(int client_fd, proactorFunc func) {
    pthread_t thread_id;
    auto* arg = new std::pair<int, proactorFunc>(client_fd, func);
    if (pthread_create(&thread_id, nullptr, proactorThreadWrapper, arg) != 0) {
        std::cerr << "Failed to create proactor thread" << std::endl;
        return 0;
    }
    return thread_id;
}

// Function to stop the proactor thread
int stopProactor(pthread_t tid) {
    // Stopping the thread
    if (pthread_cancel(tid) != 0) {
        std::cerr << "Error stopping proactor thread" << std::endl;
        return -1;
    }
    if (pthread_join(tid, NULL) != 0) {
        std::cerr << "Error joining proactor thread" << std::endl;
        return -1;
    }
    return 0;
}
