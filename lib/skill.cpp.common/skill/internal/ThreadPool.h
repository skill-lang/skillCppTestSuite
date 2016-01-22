//
// Created by Timm Felden on 22.01.16.
//

#ifndef SKILL_CPP_COMMON_THREADPOOL_H
#define SKILL_CPP_COMMON_THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace skill {
    namespace internal {

        /**
         * A thread pool.
         *
         * @note thread pool seems not to be working as intended; also we lack a barrier
         */
        class ThreadPool {

            // need to keep track of threads so we can join them
            std::vector<std::thread> workers;
            // the task queue
            std::queue<std::function<void()>> tasks;

            // synchronization
            std::mutex queue_mutex;
            std::condition_variable condition;
            bool stop;

        public:
            /**
             * A global thread pool that contains a thread for each core.
             */
            static ThreadPool global;

            /**
             * create a pool of threads that will execute functions concurrently
             *
             * @param maxThreads the number of threads to allocate; defaults to hardware concurrency
             */
            ThreadPool(size_t maxThreads = std::thread::hardware_concurrency());

            ~ThreadPool();

            /**
             * execute the argument function as soon as a thread is ready
             * @param f the function to be executed
             * @return a future, that will eventually contain the result of f
             */
            template<class F, class... Args>
            auto execute(F &&f, Args &&... args)
            -> std::future<typename std::result_of<F(Args...)>::type> {
                using return_type = typename std::result_of<F(Args...)>::type;

                auto task = std::make_shared<std::packaged_task<return_type()> >(
                        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

                std::future<return_type> res = task->get_future();
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    if (stop)
                        throw std::runtime_error("This ThreadPool is shutting down");

                    tasks.emplace([task]() { (*task)(); });
                }
                condition.notify_one();
                return res;
            }

            /**
             * run a function eventually
             */
            inline void run(std::function<void()> &&f) {
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    if (stop)
                        throw std::runtime_error("This ThreadPool is shutting down");

                    tasks.push(f);
                }
                condition.notify_one();
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_THREADPOOL_H
