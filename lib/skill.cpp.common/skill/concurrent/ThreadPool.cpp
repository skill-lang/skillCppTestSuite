//
// Created by Timm Felden on 22.01.16.
//

#include "ThreadPool.h"
#include <iostream>

using namespace skill::concurrent;

ThreadPool ThreadPool::global;

ThreadPool::ThreadPool(size_t maxThreads)
        : workers(), tasks(), queue_mutex(), condition(), stop(false) {
    while (maxThreads--)
        workers.emplace_back(
                [this] {
                    for (; ;) {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                                 [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        try {
                            task();
                        } catch (...) {
                            // ensure that wrong submissions wont kill the pool
                            if (this == &ThreadPool::global)
                                std::cerr << "A task submitted to the global thread pool crashed." << std::endl;
                            else
                                std::cerr << "A task submitted to thread pool " << (size_t) this << " crashed." <<
                                std::endl;
                        }
                    }
                }
        );
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker: workers)
        worker.join();
}