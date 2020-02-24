// 使用信号量实现一个线程安全的环形队列，实现生产者与消费者模型

#include <iostream>
#include <vector>
#include <semaphore.h>

#define MAXQ 10
#define MAXTHR 1

class RingQueue {
public:
    RingQueue(int maxq = MAXQ)
        : _capacity(maxq)
        , _step_read(0)
        , _step_write(0)
        , _queue(maxq) {
             sem_init(&_lock, 0, 1);
             sem_init(&_idle_space, 0, maxq);
             sem_init(&_data_space, 0, 0);
         }

    ~RingQueue() {
        sem_destroy(&_lock);
        sem_destroy(&_data_space);
        sem_destroy(&_idle_space);
    }

    bool Push(const int& data) {
        sem_wait(&_idle_space);
        sem_wait(&_lock);
        _queue[_step_write] = data;
        _step_write = (_step_write + 1) % _capacity;
        std::cout << "The producer produces a data--" << data << std::endl;
        sem_post(&_lock);
        sem_post(&_data_space);
        return true;
    }

    bool Pop(int* data) {
        sem_wait(&_data_space);
        sem_wait(&_lock);
        *data = _queue[_step_read];
        _step_read = (_step_read + 1) % _capacity;
        std::cout << "The consumer gets a data--" << *data << std::endl;
        sem_post(&_lock);
        sem_post(&_idle_space);
        return true;
    }
private:
    int _capacity;
    int _step_read;
    int _step_write;
    std::vector<int> _queue;
    sem_t _lock;       // 用于实现互斥
    sem_t _idle_space; // 对空闲空间计数，对生产者来说当空闲空间大于0时，才能写入数据
    sem_t _data_space; // 对数据空间计数，对消费者来说当数据空间大于0时，才能取出数据
};

void* thread_producer(void* arg) {
    RingQueue* q = (RingQueue*)arg;
    int data = 0;
    while (1) {
        q->Push(data++);
    }
    return NULL;
}

void* thread_consumer(void* arg) {
    RingQueue* q = (RingQueue*)arg;
    int data;
    while (1) {
        q->Pop(&data);
    }
    return NULL;
}

int main() {
    RingQueue q;
    std::vector<pthread_t> ptid(MAXTHR);
    std::vector<pthread_t> ctid(MAXTHR);
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
