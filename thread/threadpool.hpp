#include <iostream>
#include <queue>
#include <vector>
#include <pthread.h>

#define MAX_THREAD  5

typedef void (*handler_t)(int);

class MyTask {
public:
    MyTask(int data, handler_t handle)
        : _data(data)
        , _handler(handle) {}

    void SetTask(int data, handler_t handle) {
        _data = data; 
        _handler = handle; 
    }

    void Run() {
        return _handler(_data);
    }
private:
    int _data;
    handler_t _handler;
};

class ThreadPool {
public: 
    ThreadPool (int maxt = MAX_THREAD)
        : _thread_cur(0)
        , _thread_max(maxt)
        , _quit_flag(false)
        , _task_list()
        , _thread_list(maxt) {
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond_con, NULL);
        }

    ~ThreadPool() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_con);
    }

    // 创建线程可能会失败，不要在构造函数中创建线程
    bool Init() {
        for (int i = 0; i < _thread_max; i++) {
            if (pthread_create(&_thread_list[i], NULL, thr_start, (void*)this)) {
                std::cerr << "create pool thread error" << std::endl;
                return false;
            }
            pthread_detach(_thread_list[i]);
            _thread_cur++;
        }
        return true;
    }

    // 生产者
    bool Add(const MyTask& task) {
        if (_quit_flag) {
            return false;
        }
        pthread_mutex_lock(&_mutex);
        _task_list.push(task);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond_con);
        return true;
    }

    void Stop() {
        if (!_quit_flag) {
            pthread_mutex_lock(&_mutex);
            _quit_flag = true;
            pthread_cond_broadcast(&_cond_con);
            while (_thread_cur > 0) {
                pthread_cond_wait(&_cond_con, &_mutex);
            }
            pthread_mutex_unlock(&_mutex);
        }
    }
private:
    int _thread_cur; // 线程池中当前线程数
    int _thread_max; // 线程池中最大线程数
    bool _quit_flag; // 用于控制线程退出
    std::queue<MyTask> _task_list;
    std::vector<pthread_t> _thread_list; // 存储线程id
    pthread_mutex_t _mutex;
    pthread_cond_t _cond_con; // 消费者等待队列

    // 消费者
    static void* thr_start(void* arg) {
        ThreadPool* pthread_pool = (ThreadPool*)arg;
        while (1) {
            pthread_mutex_lock(&pthread_pool->_mutex);
            while (pthread_pool->_task_list.empty()) {
                if (pthread_pool->_quit_flag) {
                    std::cout << "thread exit " << pthread_self() << std::endl;
                    pthread_pool->_thread_cur--;
                    pthread_mutex_unlock(&pthread_pool->_mutex);
                    pthread_cond_signal(&pthread_pool->_cond_con);
                    pthread_exit(NULL);
                }
                pthread_cond_wait(&pthread_pool->_cond_con, &pthread_pool->_mutex);
            }
            MyTask task = pthread_pool->_task_list.front();
            pthread_pool->_task_list.pop();
            pthread_mutex_unlock(&pthread_pool->_mutex);
            task.Run();
        }
    }
};
