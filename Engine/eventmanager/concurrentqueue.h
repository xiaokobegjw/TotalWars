#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class concurrentqueue
{
public:
    
    bool try_pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty())
        {
            return false;
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }
    
    void wait_and_pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
    }
    
    bool empty() const
    {
       std::unique_lock<std::mutex> mlock(mutex_);
       return queue_.empty();
    }
    
    
    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }
    
    void push(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(std::move(item));
        mlock.unlock();
        cond_.notify_one();
    }
    
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
