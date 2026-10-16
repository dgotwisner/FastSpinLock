#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <random>
#include <new>
#include <cstdint>
#include <type_traits>

#include "../FastSpinLock.hpp"

namespace {

volatile int g_sink = 0;

void cpu_burn(int iters)
{
    int x = 0;
    for (int i = 0; i < iters; ++i) {
        x += i * 17;
    }
    g_sink = x;
}

template <typename WorkFn>
void run_contention_stress(int threads, int iters, WorkFn work)
{
    FastSpinLock lock;
    std::atomic<int> counter(0);

    std::vector<std::thread> workers;
    workers.reserve(static_cast<size_t>(threads));

    for (int i = 0; i < threads; ++i) {
        workers.push_back(std::thread([&, i]() {
            std::mt19937 rng(static_cast<unsigned>(i * 2654435761u + 1u));
            for (int j = 0; j < iters; ++j) {
                lock.lock();
                counter.fetch_add(1, std::memory_order_relaxed);
                work(rng);
                lock.unlock();
            }
        }));
    }

    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }

    ASSERT_EQ(counter.load(), threads * iters);
}

} // namespace

// ------------------------------------------------------------
// Non-copyable
// ------------------------------------------------------------
TEST(FastSpinLock, IsNotCopyConstructibleOrAssignable)
{
    static_assert(!std::is_copy_constructible<FastSpinLock>::value,
                  "FastSpinLock must not be copy constructible");
    static_assert(!std::is_copy_assignable<FastSpinLock>::value,
                  "FastSpinLock must not be copy assignable");
    SUCCEED();
}

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
// Negative: unlock without a matching lock is a NOOP
// ------------------------------------------------------------
TEST(FastSpinLock, UnlockWithoutLockIsNoop)
{
    FastSpinLock lock;

    // No prior lock(); unlock must leave the lock available.
    lock.unlock();
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(FastSpinLock, UnlockWhenNotLockedThenTryLockSucceeds)
{
    FastSpinLock lock;

    lock.unlock();
    lock.unlock();
    ASSERT_TRUE(lock.try_lock());
    ASSERT_FALSE(lock.try_lock());
    lock.unlock();
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
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

// ------------------------------------------------------------
// Stress: N threads, one workload style per test
// ------------------------------------------------------------
TEST(FastSpinLock, StressShortCriticalSections)
{
    run_contention_stress(8, 20000, [](std::mt19937&) {
        g_sink = g_sink + 1;
    });
}

TEST(FastSpinLock, StressLongCriticalSections)
{
    run_contention_stress(6, 2000, [](std::mt19937&) {
        cpu_burn(5000);
    });
}

TEST(FastSpinLock, StressRandomSleeps)
{
    run_contention_stress(4, 200, [](std::mt19937& rng) {
        std::uniform_int_distribution<int> us(0, 50);
        std::this_thread::sleep_for(std::chrono::microseconds(us(rng)));
    });
}

TEST(FastSpinLock, StressRandomYields)
{
    run_contention_stress(8, 5000, [](std::mt19937& rng) {
        std::uniform_int_distribution<int> n(1, 4);
        const int yields = n(rng);
        for (int i = 0; i < yields; ++i) {
            std::this_thread::yield();
        }
    });
}

TEST(FastSpinLock, StressRandomCpuBurn)
{
    run_contention_stress(6, 3000, [](std::mt19937& rng) {
        std::uniform_int_distribution<int> n(100, 2000);
        cpu_burn(n(rng));
    });
}

// ------------------------------------------------------------
// API misuse: no owner tracking — lock and unlock may be on
// different threads. Still expected to release the lock.
// ------------------------------------------------------------
TEST(FastSpinLock, LockUnlockFromDifferentThreads)
{
    FastSpinLock lock;
    std::atomic<bool> locked(false);
    std::atomic<bool> unlocked(false);

    std::thread locker([&]() {
        lock.lock();
        locked.store(true, std::memory_order_release);
        while (!unlocked.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    });

    while (!locked.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    // Misuse relative to std::mutex: unlock from a different thread.
    lock.unlock();
    unlocked.store(true, std::memory_order_release);
    locker.join();

    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

// ------------------------------------------------------------
// API misuse / lifetime
// ------------------------------------------------------------
// Use-after-destroy without reconstruction is undefined behavior and
// is expected to trip ASan/UBSan; keep it out of sanitizer builds.
#ifndef FASTSPINLOCK_SANITIZER
TEST(FastSpinLock, LockUnlockAfterDestructedObject)
{
    alignas(FastSpinLock) unsigned char storage[sizeof(FastSpinLock)];

    FastSpinLock* lock = new (storage) FastSpinLock();
    lock->lock();
    lock->unlock();
    lock->~FastSpinLock();

    // Misuse: lock/unlock after the object's lifetime has ended.
    lock->lock();
    lock->unlock();
}
#endif // !FASTSPINLOCK_SANITIZER

// Valid recovery path: destroy, placement-new, then use again.
TEST(FastSpinLock, LockUnlockAfterPlacementNew)
{
    alignas(FastSpinLock) unsigned char storage[sizeof(FastSpinLock)];

    FastSpinLock* lock = new (storage) FastSpinLock();
    lock->lock();
    lock->unlock();
    lock->~FastSpinLock();

    lock = new (storage) FastSpinLock();
    lock->lock();
    ASSERT_FALSE(lock->try_lock());
    lock->unlock();
    ASSERT_TRUE(lock->try_lock());
    lock->unlock();
    lock->~FastSpinLock();
}

// Valid recovery path: reuse storage for another type, then as a lock.
TEST(FastSpinLock, LockUnlockAfterMemoryReuse)
{
    constexpr size_t kStorageSize =
        sizeof(FastSpinLock) > sizeof(std::uint64_t) ? sizeof(FastSpinLock)
                                                     : sizeof(std::uint64_t);
    alignas(FastSpinLock) alignas(std::uint64_t) unsigned char storage[kStorageSize];

    {
        FastSpinLock* lock = new (storage) FastSpinLock();
        lock->lock();
        lock->unlock();
        lock->~FastSpinLock();
    }

    {
        std::uint64_t* junk = new (storage) std::uint64_t(0xDEADBEEFCAFEBABEull);
        ASSERT_EQ(*junk, 0xDEADBEEFCAFEBABEull);
        junk->~uint64_t();
    }

    FastSpinLock* lock = new (storage) FastSpinLock();
    ASSERT_TRUE(lock->try_lock());
    lock->unlock();
    lock->lock();
    lock->unlock();
    lock->~FastSpinLock();
}
