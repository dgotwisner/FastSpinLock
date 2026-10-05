#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <condition_variable>
#include <atomic>

#include "../FastSpinLock.hpp"

// ------------------------------------------------------------
// Basic lock/unlock
// ------------------------------------------------------------
TEST(FastSpinLock, BasicLockUnlock)
{
    FastSpinLock lock;
    lock.lock();
    lock.unlock();
    SUCCEED();
}

// ------------------------------------------------------------
// std::lock_guard compatibility
// ------------------------------------------------------------
TEST(FastSpinLock, LockGuardCompatibility)
{
    FastSpinLock lock;
    int value = 0;

    {
        std::lock_guard<FastSpinLock> guard(lock);
        value = 42;
    }

    ASSERT_EQ(value, 42);
}

// ------------------------------------------------------------
// std::unique_lock compatibility
// ------------------------------------------------------------
TEST(FastSpinLock, UniqueLockCompatibility)
{
    FastSpinLock lock;
    int value = 0;

    {
        std::unique_lock<FastSpinLock> ulock(lock);
        value = 99;
    }

    ASSERT_EQ(value, 99);
}

// ------------------------------------------------------------
// std::condition_variable_any compatibility (C++11 style)
// ------------------------------------------------------------
TEST(FastSpinLock, ConditionVariableAny)
{
    FastSpinLock lock;
    std::condition_variable_any cv;

    bool ready = false;

    std::thread t([&]() {
        std::unique_lock<FastSpinLock> lk(lock);
        ready = true;
        cv.notify_one();
    });

    {
        std::unique_lock<FastSpinLock> lk(lock);
        while (!ready) {
            cv.wait(lk);
        }
    }

    t.join();
    ASSERT_TRUE(ready);
}

// ------------------------------------------------------------
// Multi-thread contention sanity test (C++11)
// ------------------------------------------------------------
TEST(FastSpinLock, MultiThreadContention)
{
    FastSpinLock lock;
    std::atomic<int> counter(0);

    const int threads = 8;
    const int iters = 100000;

    std::vector<std::thread> workers;
    workers.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        workers.push_back(std::thread([&]() {
            for (int j = 0; j < iters; ++j) {
                std::lock_guard<FastSpinLock> guard(lock);
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        }));
    }

    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }

    ASSERT_EQ(counter.load(), threads * iters);
}
