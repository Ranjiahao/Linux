// 基于互斥锁与条件变量实现一个线程安全的阻塞队列，实现生产者与消费者模型

#include <iostream>
#include <queue>
#include <pthread.h>

#define MAXQ 10
#define MAXTHR 1

class BlockQueue {
public:
    BlockQueue(int maxq = MAXQ)
        : _capacity(maxq) {
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond_productor, NULL);
            pthread_cond_init(&_cond_consumer, NULL);
        }

    ~BlockQueue() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_productor);
        pthread_cond_destroy(&_cond_consumer);
    }

    bool Push(const int& data) {
        pthread_mutex_lock(&_mutex);
        while (_queue.size() == _capacity) {
            pthread_cond_signal(&_cond_consumer);
            pthread_cond_wait(&_cond_productor, &_mutex);
        }
        _queue.push(data);
        std::cout << "The producer produces a data--" << data << std::endl;
        pthread_mutex_unlock(&_mutex);
        return true;
    }

    bool Pop(int* data) {
        pthread_mutex_lock(&_mutex);
        while (_queue.empty()) {
            pthread_cond_signal(&_cond_productor);
            pthread_cond_wait(&_cond_consumer, &_mutex);
        }
        *data = _queue.front();
        _queue.pop();
        std::cout << "The consumer gets a data--" << *data << std::endl;
        pthread_mutex_unlock(&_mutex);
        return true;
    }
private:
    size_t _capacity;
    std::queue<int> _queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond_productor;
    pthread_cond_t _cond_consumer;
};

void* thread_producer(void* arg) {
    BlockQueue *q = (BlockQueue*)arg;
    int data = 0;
    while (1) {
        q->Push(data++);
    }
    return NULL;
}

void* thread_consumer (void* arg) {
    BlockQueue *q = (BlockQueue*)arg;
    int data;
    while (1) {
        q->Pop(&data);
    }
    return NULL;
}

int main() {
    BlockQueue q;
    pthread_t ptid[MAXTHR], ctid[MAXTHR];
    for (int i = 0; i < MAXTHR; i++) {
        if (pthread_create(&ptid[i], NULL, thread_producer, (void*)&q)) {
            std::cerr << "create thread error" << std::endl;
            return -1;
        }
    }
    for (int i = 0; i < MAXTHR; i++) {
        if (pthread_create(&ctid[i], NULL, thread_consumer, (void*)&q)) {
            std::cerr << "create thread error" << std::endl;
            return -1;
        }
    }
    for (int i = 0; i < MAXTHR; i++) {
        pthread_join(ptid[i], NULL);
        pthread_join(ctid[i], NULL);
    }
    return 0;
}
