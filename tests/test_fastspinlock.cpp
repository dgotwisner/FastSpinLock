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
// try_lock: succeeds when unlocked, fails when held
// ------------------------------------------------------------
TEST(FastSpinLock, TryLockSucceedsWhenUnlocked)
{
    FastSpinLock lock;

    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(FastSpinLock, TryLockFailsWhenHeld)
{
    FastSpinLock lock;

    lock.lock();
    ASSERT_FALSE(lock.try_lock());
    lock.unlock();
}

TEST(FastSpinLock, TryLockSucceedsAfteUnlock)
{
    FastSpinLock lock;

    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(FastSpinLock, TryLockAfteBlockingLock)
{
    FastSpinLock lock;

    lock.lock();
    ASSERT_FALSE(lock.try_lock());
    lock.unlock();
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

// ------------------------------------------------------------
// std::unique_lock with try_to_lock
// ------------------------------------------------------------
TEST(FastSpinLock, UniqueLockTyToLock)
{
    FastSpinLock lock;

    lock.lock();
    {
        std::unique_lock<FastSpinLock> ulock(lock, std::try_to_lock);
        ASSERT_FALSE(ulock.owns_lock());
    }
    lock.unlock();

    {
        std::unique_lock<FastSpinLock> ulock(lock, std::try_to_lock);
        ASSERT_TRUE(ulock.owns_lock());
    }
}

// ------------------------------------------------------------
// Multi-thread try_lock contention
// ------------------------------------------------------------
TEST(FastSpinLock, TryLockMultiThreadContention)
{
    FastSpinLock lock;
    std::atomic<int> acquied(0);
    std::atomic<int> failed(0);

    const int threads = 8;
    const int iters = 100000;

    std::vector<std::thread> workers;
    workers.reserve(threads);

    for (int i = 0; i < threads; ++i) {
        workers.push_back(std::thread([&]() {
            for (int j = 0; j < iters; ++j) {
                if (lock.try_lock()) {
                    acquied.fetch_add(1, std::memory_order_relaxed);
                    lock.unlock();
                } else {
                    failed.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }));
    }

    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }

    ASSERT_EQ(acquied.load() + failed.load(), threads * iters);
    ASSERT_GT(acquied.load(), 0);
    ASSERT_GT(failed.load(), 0);
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
