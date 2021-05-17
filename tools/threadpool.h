/*
 * @copyright: zwenc
 * @email: zwence@163.com
 * @Date: 2021-05-15 16:57:27
 * @FilePath: \cpptools\tools\threadpool.h
 */

#pragma once

#include <thread>
#include <queue>
#include <vector>
#include <future>
#include <functional>
#include <atomic>

class threadpool {
 private:
    using Task = std::function<void()>;

    std::vector<std::thread> pool;
    std::atomic<int> free_thread_num;
    std::atomic<bool> stopped;
    std::mutex m_lock;
    std::condition_variable task_condition;
    std::queue<Task> tasks;

 private:
    void thread_fun();

 public:
    inline threadpool(int size);
    ~threadpool();

    template<class F, class... Args>
    auto commit(F f, Args... args) -> std::future<decltype(f(args...))>;

    void join();
    void close();

    int thread_num() const;
};

threadpool::threadpool(int size) : stopped(false) {
    free_thread_num = size > 1 ? size : 1;

    for (int i = 0; i < free_thread_num; ++i) {
        pool.emplace_back(std::bind(&threadpool::thread_fun, this));
    }
}

threadpool::~threadpool() {
    this->join();
}

void threadpool::thread_fun() {
    threadpool::Task task;
    while(!this->stopped) {
        {
            std::unique_lock<std::mutex> lock{this->m_lock};  // 加锁
            this->task_condition.wait(                        // 这里面会先解锁，然后线程进入等待，被唤醒后重新加锁
                lock, [this] {return this->stopped.load() || !this->tasks.empty();}
            );
            if (this->stopped && this->tasks.empty()) {
                return ;
            }

            task = std::move(this->tasks.front());
            this->tasks.pop();
        } 
        
        this->free_thread_num --;
        task();
        this->free_thread_num ++;
    }
}

template<class F, class... Args>
auto threadpool::commit(F f, Args... args) -> std::future<decltype(f(args...))> {
    if (this->stopped.load()) {
        throw std::runtime_error("commit thread Pool is stopped !");
    }

    using retType = decltype(f(args...));
    // 因为使用了std::bind函数，task的形参已经消失了，所以retType()里面不需要有参数，统一了接口
    auto task = std::make_shared<std::packaged_task<retType()>> (
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<retType> future = task->get_future();

    {
        std::lock_guard<std::mutex> lock(this->m_lock);

        tasks.emplace(
            [task] {
                (*task)();
            }
        );
    }

    this->task_condition.notify_one();
    return future;
}

void threadpool::close() {
    stopped.store(true);
}

void threadpool::join() {
    stopped.store(true);

    task_condition.notify_all();

    for (auto &x : this->pool) {
        if (x.joinable()) {    // 等待线程结束，如果线程死循环了，就出不去了
            x.join();
        }
    }
}

int threadpool::thread_num() const { 
    return this->free_thread_num; 
}
