#include <iostream>
#include <thread>
#include <mutex>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexCondition = PTHREAD_MUTEX_INITIALIZER;
bool condition = false;

void* func(void* arg){
    while(true){
        pthread_mutex_lock(&mutexCondition);
        while(!condition){
            pthread_cond_wait(&cond, &mutexCondition);
        }
        // Reset condition and unlock mutex before printing
        condition = false;
        pthread_mutex_unlock(&mutexCondition);
        std::cout << "Thread woke up" << std::endl;
    }
    return NULL;
}

int main(){
    pthread_t thread;
    pthread_create(&thread, NULL, func, NULL);
    while(true){
        std::string input;
        std::cin >> input;
        if(input == "wake"){
            pthread_mutex_lock(&mutexCondition);
            condition = true;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutexCondition);
        }
    }
    return 0;
}
