#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <sys/syscall.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <atomic>
#include <array>
#include <string>

static const uint64_t c_usecPerSec = 1000*1000;

#if !defined(THREADCOUNT)
#    define THREADCOUNT 7 // 1 - 10
#endif // threadcount

#if defined(RDTSCP)
static inline uint64_t rdtscp( uint32_t & aux )
{
    uint64_t rax,rdx;
    asm volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : : );
    return (rdx << 32) + rax;
}
#elif defined(RDTSC)
static inline uint64_t rdtsc(void)
{
    uint64_t rax,rdx;
    asm volatile ( "rdtsc\n" : "=a" (rax), "=d" (rdx) : : );
    return (rdx << 32) + rax;
}
#endif // defined(RDTSC)

#if !defined(MYLOCK)
# if defined(TIMING) || defined(RDTSCP) || defined(RDTSC)
    class Mtx: public std::mutex {
    public:
        void lock(void) {
#  if defined(TIMING)
            clock_gettime(CLOCK_MONOTONIC, &m_lockStartTime);
#  elif defined(RDTSCP)
            uint32_t aux;
            m_lockStartTimeTicks = rdtscp(aux);
#  elif defined(RDTSC)
            m_lockStartTimeTicks = rdtsc();
#  endif
            std::mutex::lock();
#  if defined(TIMING)
            clock_gettime(CLOCK_MONOTONIC, &m_lockLockedTime);
#  elif defined(RDTSCP)
            m_lockLockedTimeTicks = rdtscp(aux);
#  elif defined(RDTSC)
            m_lockLockedTimeTicks = rdtsc();
#  endif
        }
        void unlock(void) {
            std::mutex::unlock();
#  if defined(TIMING)
            clock_gettime(CLOCK_MONOTONIC, &m_lockUnlockTime);
#  elif defined(RDTSCP)
            uint32_t aux;
            m_lockUnlockTimeTicks = rdtscp(aux);
#  elif defined(RDTSC)
            m_lockUnlockTimeTicks = rdtsc();
#  endif
        }
    private:
#  if defined(TIMING)
        struct timespec m_lockStartTime;
        struct timespec m_lockLockedTime;
        struct timespec m_lockUnlockTime;
#  elif defined(RDTSCP) || defined(RDTSC)
        uint64_t m_lockStartTimeTicks;
        uint64_t m_lockLockedTimeTicks;
        uint64_t m_lockUnlockTimeTicks;
#  endif // defined(RDTSCP) || defined(RDTSC)
    };
#  if defined(TIMING)
    Mtx mtx;
    std::string label("StdMutex-timing");
#  elif defined(RDTSCP)
    Mtx mtx;
    std::string label("StdMutex-rdtscp");
#  elif defined(RDTSC)
    Mtx mtx;
    std::string label("StdMutex-rdtsc");
#  endif
# else
  std::mutex mtx;
  std::string label("StdMutex");
# endif
#else
class MyLock {
public:
    MyLock() {
        m_lock = false;
    }
    void lock() {
#if defined(TIMING)
        clock_gettime(CLOCK_MONOTONIC, &m_lockStartTime);
#elif defined(RDTSCP)
        uint32_t aux;
        m_lockStartTimeTicks = rdtscp(aux);
#elif defined(RDTSC)
        m_lockStartTimeTicks = rdtsc();
#endif // defined(RDTSC)
        while (std::atomic_exchange_explicit(&m_lock, true, std::memory_order_acquire))
            ;
#if defined(TIMING)
        clock_gettime(CLOCK_MONOTONIC, &m_lockLockedTime);
#elif defined(RDTSCP)
        m_lockLockedTimeTicks = rdtscp(aux);
#elif defined(RDTSC)
        m_lockLockedTimeTicks = rdtsc();
#endif // defined(RDTSC)
    }
    void unlock() {
        std::atomic_store_explicit(&m_lock, false, std::memory_order_release);
#if defined(TIMING)
        clock_gettime(CLOCK_MONOTONIC, &m_lockUnlockTime);
#endif // defined(TIMING)
#if defined(RDTSCP)
        uint32_t aux;
        m_lockUnlockTimeTicks = rdtscp(aux);
#endif // defined(RDTSCP)
#if defined(RDTSC)
        m_lockUnlockTimeTicks = rdtsc();
#endif // defined(RDTSC)
    }
private:
    std::atomic<bool> m_lock;
#if defined(TIMING)
    struct timespec m_lockStartTime;
    struct timespec m_lockLockedTime;
    struct timespec m_lockUnlockTime;
#endif // defined(TIMING)
#if defined(RDTSCP) || defined(RDTSC)
    uint64_t m_lockStartTimeTicks;
    uint64_t m_lockLockedTimeTicks;
    uint64_t m_lockUnlockTimeTicks;
#endif // defined(RDTSCP) || defined(RDTSC)
};
MyLock mtx;
#if defined(TIMING)
std::string label("MyLock-timing");
#elif defined(RDTSCP)
std::string label("MyLock-rdtscp");
#elif defined(RDTSC)
std::string label("MyLock-rdtsc");
#else
std::string label("MyLock");
#endif
#endif

uint64_t threadAccumNanos[THREADCOUNT+1];
uint64_t progAccumNanos;
static const int threadIterations = 100000000;
static const int progIterations = 100000000;
static const long nanosPerSec = 1000*1000*1000;

void fcn(int item)
{
    if (item > THREADCOUNT) {
        return;
    }
    //printf("xxx %d: Thread = %d\n", item, syscall(SYS_gettid));
    //printf("xxx %d: Child locking...\n", item);
    struct timespec start, end;
    threadAccumNanos[item] = 0;
    for (int i = 0; i < threadIterations; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        mtx.lock();
        mtx.unlock();
        clock_gettime(CLOCK_MONOTONIC, &end);
        uint64_t nanos = (end.tv_sec - start.tv_sec);
        nanos *= nanosPerSec;
        nanos += (end.tv_nsec - start.tv_nsec);
        threadAccumNanos[item] += nanos;
    }
    printf("xxx %d: Child done\n", item);
}

int main(int ac, char **av)
{
    for (int i = 0; i < THREADCOUNT+1; i++) {
        threadAccumNanos[i] = 0;
    }
    printf("BEGIN TEST\n"); // for awk processing
    fflush(stdout);
    int type = -1;
    int threads = -1;
    if (av[1] != NULL) {
        type = atoi(av[1]);
        if (av[2] != NULL) {
            threads = atoi(av[2]);
        }
    }
    printf("Running %s type %d threads %d (arg2), THREADCOUNT %d :\n", av[0], type, threads, THREADCOUNT);
    fflush(stdout);
    struct timeval startTime, endTime;
    struct tms startTms, endTms;
    int clocksPerSec = sysconf(_SC_CLK_TCK);
    printf("%d clocks/sec\n", clocksPerSec);
    fflush(stdout);
    gettimeofday(&startTime, NULL);
    times(&startTms);
    progAccumNanos = 0;
    //printf("xxx Pid %d\n", getpid());
#if 0
    std::thread t1(fcn, 1);
    std::thread t2(fcn, 2);
    std::thread t3(fcn, 3);
    std::thread t4(fcn, 4);
    std::thread t5(fcn, 5);
    std::thread t6(fcn, 6);
    std::thread t7(fcn, 7);
    std::thread t8(fcn, 8);
    std::thread t9(fcn, 9);
    std::thread t10(fcn, 10);
#else
    std::array<std::thread, THREADCOUNT+1> threadArray;
    for (int i = 1; i <= THREADCOUNT; i++) {
        threadArray[i] = std::thread(fcn, i);
    }
#endif
    printf("xxx Created %d threads, array size %lu\n", THREADCOUNT, threadArray.size());
    printf("xxx Parent locking...\n");
    fflush(stdout);
    struct timespec start, end;
    for (int i = 0; i < progIterations; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        mtx.lock();
        mtx.unlock();
        clock_gettime(CLOCK_MONOTONIC, &end);
        uint64_t nanos = (end.tv_sec - start.tv_sec);
        nanos *= nanosPerSec;
        nanos += (end.tv_nsec - start.tv_nsec);
        progAccumNanos += nanos;
    }
    printf("xxx Parent done\n");
    printf("xxx Final %d threads, array size %lu\n", THREADCOUNT, threadArray.size());
#if 0
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
#else
    printf("xxx Waiting for clients to complete\n");
    for (auto &t : threadArray) {
        if (t.joinable()) {
            t.join();
        }
    }
    printf("xxx Childs completed\n");
#endif
    uint64_t locks = progIterations;
    uint64_t nanos = progAccumNanos;
    printf("\n%d threads:\n", THREADCOUNT);
    printf("\tmain %d iterations: %lu sec, %lu nsec\n",
        progIterations, progAccumNanos / nanosPerSec, progAccumNanos % nanosPerSec);
    for (int i = 1; i <= THREADCOUNT; i++) {
        locks += threadIterations;
        nanos += threadAccumNanos[i];
        printf("\tthread %d: %d iterations: %lu sec, %lu nsec\n",
            i, threadIterations, threadAccumNanos[i] / nanosPerSec, threadAccumNanos[i] % nanosPerSec);
    }
    double rate = (double) locks / ((double) nanos / (double) nanosPerSec);
    printf("\t%s Lock rate = %f locks / second\n", label.c_str(), rate);
    fflush(stdout);
    gettimeofday(&endTime, NULL);
    times(&endTms);
    {
        __uint128_t tmp = endTime.tv_sec - startTime.tv_sec;
        tmp *= c_usecPerSec;
        tmp += endTime.tv_usec;
        tmp -= startTime.tv_usec;
        printf("real %12.3f seconds\n", ((double) tmp) / c_usecPerSec);
    }
    {
        double tmp = ((double) (endTms.tms_utime - startTms.tms_utime));
        printf("user %12.3f seconds\n", ((double) tmp) / ((double) clocksPerSec));

        tmp = ((double) (endTms.tms_stime - startTms.tms_stime));
        printf("sys  %12.3f seconds\n", ((double) tmp) / ((double) clocksPerSec));
    }
    printf("END TEST\n"); // for awk processing
    fflush(stdout);
    return 0;
}
