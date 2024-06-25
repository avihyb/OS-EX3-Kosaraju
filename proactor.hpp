#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#include <pthread.h>

// Define proactorFunc as a function pointer type that takes an int argument
typedef void* (*proactorFunc)(int);
void* proactorThreadWrapper(void* arg);

// Function to start a proactor thread
pthread_t startProactor(int client_fd, proactorFunc func);

int stopProactor(pthread_t tid);

#endif // PROACTOR_HPP
