# FastSpinLock

FastSpinLock is a minimal, header‑only C++ spinlock implemented using a single
`std::atomic<bool>` and a test‑and‑set acquire loop. It is designed for cases
where extremely low uncontended latency is required and where the user
understands the tradeoffs of a pure TAS spinlock under contention.

FastSpinLock has:

- zero dependencies
- zero kernel involvement
- no futex fallback
- no fairness guarantees
- a fast uncontended path
- predictable behavior for micro‑critical sections
- a BSD 2-clause license

This project includes:

- the header‑only lock implementation (`FastSpinLock.hpp`)
- a benchmark directory (`benchmark`), which contains a benchmark program that uses wrapped std::mutex and this class and scaled
  between 0 and 29 worker threads (1-30 total threads including main).  Gnuplot scripts (and png files) associated with a run on a
  CentOS 7 40-core 3.1 GHZ single-socket CPU and their input CSV files and output PNG files are attached in the repo.
- Unit test directory (`tests`) is also provided with unit tests that will do std::unique_lock, std::lock_guard, and
  std::condition_variable_any are also provided.

---

## Design Rationale

Std::mutex is implemented (on Linux) on top of futex().  It's good, it's stable, it's not particularly performant.  I've ended
up writing high performance replacements at three different companies, from scratch (because they were done on company resources
and time).  For lower thread-count, a much more efficient solution exists, since we don't have to worry about "contention forever"
cases.  This is an open source equivalent that takes into consideration my multi-core, NUMA aware work, up to 4096 cores over
decades of programming.

The idea is, that well designed code should have mutexes around small numbers of threads, with them being held for limited
amounts of time.  This implementation is optimized for that case.  Std::mutex is implemented for the generic "do no harm" case.

## Compatibility

This class is a direct drop in for std::mutex, with one exception - std::condition_variable is not compatible.  If you want to
use this, use std::condition_variable_any instead.

## Where it should be used instead of std::mutex

This class is designed to have the least overhead possible with the basic rule that locks should be held for very short periods
of time, and a thread should never get descheduled in a lock state (such as doing a message to the systemd-journald logs, or to
disk files).

This is designed for typical use cases where there are a relatively small number of threads that share a mutex.  For example, an
MPMC queue with 16 or fewer threads that can own the lock get better performance than std::mutex, at significantly lower CPU
time (user+system).  At 18-threads, that flips.

If more threads exist than cores, std::mutex will probably be the way to go, because this doesn't deschedule to prevent
CPU thrashing.

It also doesn't protect against lock(A) ; lock(B) in one thread, and lock(B) and lock(A) in another.  I don't believe std::mutex
protects against this case either.

### Performance summary

| Threads | FastSpinLock | std::mutex |
| ------- | ------------ | ---------- |
| 1-9     | Faster       | Slower     |
| 10-17   | Comparable   | Comparable |
| 18+     | Slower       | Faster     |

## Header‑Only Lock

```cpp
class FastSpinLock {
public:
    FastSpinLock() {
        m_lock = false;
    }
    void lock() {
        while (std::atomic_exchange_explicit(&m_lock, true,
                                             std::memory_order_acquire))
            ;
    }
    void unlock() {
        std::atomic_store_explicit(&m_lock, false,
                                   std::memory_order_release);
    }
private:
    std::atomic<bool> m_lock;
};
```

## When to Use

FastSpinLock is appropriate when:

- the critical section is only a few instructions;
- the contention is expected to be low;
- the lock is used in tight loops when `std::mutex` overhead dominates;
- you want predictable, minimal overhead in the uncontended case.

## When Not to Use

FastSpinLock should not be used when:

- contention is high;
- fairness is required;
- threads may block for long periods;
- the critical section performs I/O or system calls;
- you need priority inheritance or wakeup ordering guarantees

Under contention, a TAS spinlock will generate cacheline ping-pong and degrade rapidly (which you will see for the larger core
counts in the graphs).  This is expected and inherent to the design.

## Notes, from experience

If you have lots of threads and each locks the same mutex, you should consider fixing your design.

### Example 1

If you use std::lock_guard and do logs or system calls within the lock state, you should consider your code, and narrow the
scope of the locks (with a more limited open/close bracing protection of the lock).

```cpp
{
    std::lock_guard(std::mutex) lock;
    // access protected data
    // then write data to disk
}
```

Is better represented by

```cpp
{
    std::vector<uint8_t> copy;
    {
        std::lock_guard(std::mutex) lock;
        // access protected data
        // copy protected data to "copy"
    }
    // then write data to disk
}
```

This is an example of how to reduce lock state with std::mutex.  FastSpinLock would get the same benefit.

### Example 2

I once worked at a company that did geolocation of cellular data at the eNodeB (LTE) layer - each eNodeB had one thread, so in
typical use cases, a server supported about 800 eNodeB's, so we had 800 threads with data.  Each thread pushed to a single queue
the data it read from cell phones.  The queue had a mutex.  On a 32-core server, you had about a 1/32 chance of getting a lock
(about 3%) to push to the queue.  It worked OK, but was slow.  When we deployed on a 96 core server, you had a 1/96 chance of
getting the lock (about 1%).  Performance was less than acceptible.  Crappy design.  The queue simply aggregated data from the
sources.

We were actually able to get this to be lock-free - we used TLS.  Each thread appended to a buffer, when the buffer filled,
we output to a socket (which was what the old queue did on the receiving side - one buffer, not one buffer per eNodeB).  This
change gave screaming good performance.

So the key here, if you can get rid of a mutex via smart programming, do that before switching to FastSpinLock (note,
FastSpinLock wouldn't haven't worked here (we tried something similar) because of the huge number of threads doing locks).

## Benchmark results

This first plot compares locks/second for FastSpinLock and std::mutex.  The FastSpinLock values correspond to FastSpinLock in the
graphs.  Std::mutex labels correspond to the std::mutex values in the graphs.

The tests run with main always doing lock/unlock, and a number of threads from 0 through 29.  The X axis is the number of
threads, so the values at 0 actually have a main process and zero threads doing locks/unlocks, and the value at 29 has main and
29 other threads doing locks and unlocks.

Each test does 10 tests each for each of FastSpinLock and std::mutex at each number of threads.

In this plot, the larger numbers are best.

![locks per second](benchmark/locksPerSecond.png)

And the second takes the cumulative CPU time of the threads (including the clock_gettime() calls around the lock/unlock calls
and the loop overhead).

In this plot, the smaller numbers are best.

![user + system CPU time](benchmark/user+sysTime.png)

### Benchmarking with alignas(64) and alignas(128)

The same benchmark plotted in the two plots was run with both Mutex classes being set as **alignas(64)** and **alignas(128)**,
since both are valid cache line sizes.  Like above, the test ran 10 iterations for each of FastSpinLock and std::mutex, for 0
through 29 threads (plus the main program).  The following table shows the results for no-align, 64-bit align, and 128-bit
align, for lock rate and sum(user+system) averaged.

The idea for testing with **alignas** came from a Grok query, and from the test, it looks like we get a performance hit from
doing the alignment, possibly because it produces a bigger hit on cacheline access / invalidation.

For the percent values, +/- 5% is left in black.  If it's worse outside that range, it's in red.
If it's better outside that range, it's in green.

<table>
    <thead>
        <!-- Row 1 -->
        <tr>
            <th style="text-align:center; font-weight:bold;">Threads</th>
            <th colspan="6" style="text-align:center; font-weight:bold;">std::mutex</th>
            <th colspan="6" style="text-align:center; font-weight:bold;">FastSpinLock</th>
        </tr>
        <!-- Row 2 -->
        <tr>
            <th></th>
            <th colspan="3" style="text-align:center; font-weight:bold;">Locks/Second</th>
            <th colspan="3" style="text-align:center; font-weight:bold;">User+System Time</th>
            <th colspan="3" style="text-align:center; font-weight:bold;">Locks/Second</th>
            <th colspan="3" style="text-align:center; font-weight:bold;">User+System Time</th>
        </tr>
        <!-- Row 3 -->
        <tr>
            <th></th>
            <th colspan="3" style="text-align:center;">Locks/Second As a Percent (Greater Than 100% is best)</th>
            <th colspan="3" style="text-align:center;">User+System Time As a Percent (Less Than 100% is best)</th>
            <th colspan="3" style="text-align:center;">Locks/Second As a Percent (Greater Than 100% is best)</th>
            <th colspan="3" style="text-align:center;">User+System Time As a Percent (Less Than 100% is best)</th>
        </tr>
        <!-- Row 4 -->
        <tr>
            <th></th>
            <th style="text-align:center; font-weight:bold;">No align</th>
            <th style="text-align:center; font-weight:bold;">Alignas(64)</th>
            <th style="text-align:center; font-weight:bold;">Alignas(128)</th>
            <th style="text-align:center; font-weight:bold;">No align</th>
            <th style="text-align:center; font-weight:bold;">Alignas(64)</th>
            <th style="text-align:center; font-weight:bold;">Alignas(128)</th>
            <th style="text-align:center; font-weight:bold;">No align</th>
            <th style="text-align:center; font-weight:bold;">Alignas(64)</th>
            <th style="text-align:center; font-weight:bold;">Alignas(128)</th>
            <th style="text-align:center; font-weight:bold;">No align</th>
            <th style="text-align:center; font-weight:bold;">Alignas(64)</th>
            <th style="text-align:center; font-weight:bold;">Alignas(128)</th>
        </tr>
        <!-- Row 5 -->
        <tr>
            <th></th>
            <th style="text-align:center;"></th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;"></th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
            <th style="text-align:center;">Percent</th>
        </tr>
    </thead>
    <tbody>
        <!-- Replace starting at the next line -->
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 0</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 35414052.507</td>
	        <td style="text-align right; font-weight:bold;"> 34944135.825</td>
	        <td style="text-align right; font-weight:bold;"> 35183426.950</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 4.396</td>
	        <td style="text-align right; font-weight:bold;"> 4.454</td>
	        <td style="text-align right; font-weight:bold;"> 4.424</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 50239938.956</td>
	        <td style="text-align right; font-weight:bold;"> 49975245.555</td>
	        <td style="text-align right; font-weight:bold;"> 50622071.863</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 3.569</td>
	        <td style="text-align right; font-weight:bold;"> 3.595</td>
	        <td style="text-align right; font-weight:bold;"> 3.550</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.673</td>
	        <td style="text-align right;"> 99.349</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.319</td>
	        <td style="text-align right;"> 100.637</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 141.864</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 141.117</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 142.943</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 81.187</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 81.779</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 80.755</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 1</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 6990012.858</td>
	        <td style="text-align right; font-weight:bold;"> 7219073.283</td>
	        <td style="text-align right; font-weight:bold;"> 7122395.009</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 37.619</td>
	        <td style="text-align right; font-weight:bold;"> 37.552</td>
	        <td style="text-align right; font-weight:bold;"> 37.598</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 11759434.207</td>
	        <td style="text-align right; font-weight:bold;"> 10185062.208</td>
	        <td style="text-align right; font-weight:bold;"> 9919177.389</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 26.474</td>
	        <td style="text-align right; font-weight:bold;"> 28.894</td>
	        <td style="text-align right; font-weight:bold;"> 29.146</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 103.277</td>
	        <td style="text-align right;"> 101.894</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.822</td>
	        <td style="text-align right;"> 99.944</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 168.232</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 145.709</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 141.905</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 70.374</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 76.807</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 77.477</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 2</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 3272492.196</td>
	        <td style="text-align right; font-weight:bold;"> 3315878.576</td>
	        <td style="text-align right; font-weight:bold;"> 3305732.059</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 102.538</td>
	        <td style="text-align right; font-weight:bold;"> 101.096</td>
	        <td style="text-align right; font-weight:bold;"> 101.468</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 7502863.296</td>
	        <td style="text-align right; font-weight:bold;"> 6094786.489</td>
	        <td style="text-align right; font-weight:bold;"> 6635457.825</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 59.608</td>
	        <td style="text-align right; font-weight:bold;"> 70.790</td>
	        <td style="text-align right; font-weight:bold;"> 67.049</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.326</td>
	        <td style="text-align right;"> 101.016</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.594</td>
	        <td style="text-align right;"> 98.956</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 229.271</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 186.243</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 202.765</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 58.133</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 69.038</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 65.389</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 3</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1885010.299</td>
	        <td style="text-align right; font-weight:bold;"> 1933630.078</td>
	        <td style="text-align right; font-weight:bold;"> 1929676.695</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 225.943</td>
	        <td style="text-align right; font-weight:bold;"> 220.752</td>
	        <td style="text-align right; font-weight:bold;"> 220.968</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 5164591.796</td>
	        <td style="text-align right; font-weight:bold;"> 5212193.756</td>
	        <td style="text-align right; font-weight:bold;"> 5125541.513</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 113.780</td>
	        <td style="text-align right; font-weight:bold;"> 117.056</td>
	        <td style="text-align right; font-weight:bold;"> 118.848</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 102.579</td>
	        <td style="text-align right;"> 102.370</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 97.703</td>
	        <td style="text-align right;"> 97.798</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 273.982</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 276.507</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 271.911</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 50.358</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 51.808</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 52.601</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 4</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1417964.998</td>
	        <td style="text-align right; font-weight:bold;"> 1421807.406</td>
	        <td style="text-align right; font-weight:bold;"> 1422307.155</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 366.041</td>
	        <td style="text-align right; font-weight:bold;"> 365.604</td>
	        <td style="text-align right; font-weight:bold;"> 364.856</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 4458619.622</td>
	        <td style="text-align right; font-weight:bold;"> 4635251.434</td>
	        <td style="text-align right; font-weight:bold;"> 4651979.912</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 158.810</td>
	        <td style="text-align right; font-weight:bold;"> 164.454</td>
	        <td style="text-align right; font-weight:bold;"> 164.323</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.271</td>
	        <td style="text-align right;"> 100.306</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.881</td>
	        <td style="text-align right;"> 99.676</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 314.438</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 326.895</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 328.074</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 43.386</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 44.928</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 44.892</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 5</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1138799.643</td>
	        <td style="text-align right; font-weight:bold;"> 1151028.439</td>
	        <td style="text-align right; font-weight:bold;"> 1161514.601</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 537.703</td>
	        <td style="text-align right; font-weight:bold;"> 532.131</td>
	        <td style="text-align right; font-weight:bold;"> 527.655</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 3939322.394</td>
	        <td style="text-align right; font-weight:bold;"> 3856724.154</td>
	        <td style="text-align right; font-weight:bold;"> 3863948.508</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 211.964</td>
	        <td style="text-align right; font-weight:bold;"> 230.961</td>
	        <td style="text-align right; font-weight:bold;"> 230.915</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.074</td>
	        <td style="text-align right;"> 101.995</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.964</td>
	        <td style="text-align right;"> 98.131</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 345.919</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 338.666</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 339.300</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 39.420</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 42.953</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 42.945</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 6</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 945769.780</td>
	        <td style="text-align right; font-weight:bold;"> 961709.199</td>
	        <td style="text-align right; font-weight:bold;"> 961734.839</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 746.572</td>
	        <td style="text-align right; font-weight:bold;"> 735.125</td>
	        <td style="text-align right; font-weight:bold;"> 735.124</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 3325069.884</td>
	        <td style="text-align right; font-weight:bold;"> 3235056.500</td>
	        <td style="text-align right; font-weight:bold;"> 3289890.089</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 290.690</td>
	        <td style="text-align right; font-weight:bold;"> 316.306</td>
	        <td style="text-align right; font-weight:bold;"> 315.232</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.685</td>
	        <td style="text-align right;"> 101.688</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.467</td>
	        <td style="text-align right;"> 98.467</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 351.573</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 342.055</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 347.853</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 38.937</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 42.368</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 42.224</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 7</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 836535.678</td>
	        <td style="text-align right; font-weight:bold;"> 836021.608</td>
	        <td style="text-align right; font-weight:bold;"> 831744.370</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 961.314</td>
	        <td style="text-align right; font-weight:bold;"> 961.557</td>
	        <td style="text-align right; font-weight:bold;"> 967.904</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 3013608.954</td>
	        <td style="text-align right; font-weight:bold;"> 2751836.943</td>
	        <td style="text-align right; font-weight:bold;"> 2755177.849</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 370.999</td>
	        <td style="text-align right; font-weight:bold;"> 421.210</td>
	        <td style="text-align right; font-weight:bold;"> 421.893</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.939</td>
	        <td style="text-align right;"> 99.427</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.025</td>
	        <td style="text-align right;"> 100.686</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 360.249</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 328.956</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 329.356</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 38.593</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 43.816</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 43.887</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 8</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 775009.779</td>
	        <td style="text-align right; font-weight:bold;"> 762672.093</td>
	        <td style="text-align right; font-weight:bold;"> 784770.516</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 1166.861</td>
	        <td style="text-align right; font-weight:bold;"> 1186.760</td>
	        <td style="text-align right; font-weight:bold;"> 1152.523</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 2625843.613</td>
	        <td style="text-align right; font-weight:bold;"> 2505477.630</td>
	        <td style="text-align right; font-weight:bold;"> 2489105.961</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 480.796</td>
	        <td style="text-align right; font-weight:bold;"> 529.221</td>
	        <td style="text-align right; font-weight:bold;"> 528.732</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.408</td>
	        <td style="text-align right;"> 101.259</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.705</td>
	        <td style="text-align right;"> 98.771</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 338.814</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 323.283</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 321.171</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 41.204</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 45.354</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 45.312</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 9</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 723996.882</td>
	        <td style="text-align right; font-weight:bold;"> 714079.891</td>
	        <td style="text-align right; font-weight:bold;"> 725020.843</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 1388.784</td>
	        <td style="text-align right; font-weight:bold;"> 1408.143</td>
	        <td style="text-align right; font-weight:bold;"> 1388.193</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 2372237.816</td>
	        <td style="text-align right; font-weight:bold;"> 2237002.717</td>
	        <td style="text-align right; font-weight:bold;"> 2340544.187</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 595.976</td>
	        <td style="text-align right; font-weight:bold;"> 653.086</td>
	        <td style="text-align right; font-weight:bold;"> 639.715</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.630</td>
	        <td style="text-align right;"> 100.141</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.394</td>
	        <td style="text-align right;"> 99.957</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 327.659</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 308.980</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 323.281</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 42.914</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 47.026</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 46.063</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 10</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 656427.583</td>
	        <td style="text-align right; font-weight:bold;"> 652673.602</td>
	        <td style="text-align right; font-weight:bold;"> 655021.858</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 1670.827</td>
	        <td style="text-align right; font-weight:bold;"> 1680.048</td>
	        <td style="text-align right; font-weight:bold;"> 1672.926</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1690339.496</td>
	        <td style="text-align right; font-weight:bold;"> 2041063.274</td>
	        <td style="text-align right; font-weight:bold;"> 2120844.285</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 810.949</td>
	        <td style="text-align right; font-weight:bold;"> 793.098</td>
	        <td style="text-align right; font-weight:bold;"> 775.193</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.428</td>
	        <td style="text-align right;"> 99.786</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.552</td>
	        <td style="text-align right;"> 100.126</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 257.506</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 310.935</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 323.089</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 48.536</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 47.467</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 46.396</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 11</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 603668.719</td>
	        <td style="text-align right; font-weight:bold;"> 592589.289</td>
	        <td style="text-align right; font-weight:bold;"> 604014.825</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 1981.581</td>
	        <td style="text-align right; font-weight:bold;"> 2019.263</td>
	        <td style="text-align right; font-weight:bold;"> 1982.558</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1569975.837</td>
	        <td style="text-align right; font-weight:bold;"> 1895902.420</td>
	        <td style="text-align right; font-weight:bold;"> 1959041.348</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 957.713</td>
	        <td style="text-align right; font-weight:bold;"> 938.878</td>
	        <td style="text-align right; font-weight:bold;"> 924.718</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.165</td>
	        <td style="text-align right;"> 100.057</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.902</td>
	        <td style="text-align right;"> 100.049</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 260.072</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 314.063</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 324.523</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 48.331</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 47.380</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 46.666</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 12</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 536487.284</td>
	        <td style="text-align right; font-weight:bold;"> 542691.694</td>
	        <td style="text-align right; font-weight:bold;"> 552712.524</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 2412.810</td>
	        <td style="text-align right; font-weight:bold;"> 2386.827</td>
	        <td style="text-align right; font-weight:bold;"> 2344.242</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1369233.273</td>
	        <td style="text-align right; font-weight:bold;"> 1589010.073</td>
	        <td style="text-align right; font-weight:bold;"> 1819588.394</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 1181.085</td>
	        <td style="text-align right; font-weight:bold;"> 1102.722</td>
	        <td style="text-align right; font-weight:bold;"> 1014.447</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.156</td>
	        <td style="text-align right;"> 103.024</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.923</td>
	        <td style="text-align right;"> 97.158</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 255.222</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 296.188</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 339.167</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 48.951</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 45.703</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 42.044</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 13</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 496325.478</td>
	        <td style="text-align right; font-weight:bold;"> 493013.089</td>
	        <td style="text-align right; font-weight:bold;"> 501120.174</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 2809.605</td>
	        <td style="text-align right; font-weight:bold;"> 2831.521</td>
	        <td style="text-align right; font-weight:bold;"> 2783.412</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1193316.741</td>
	        <td style="text-align right; font-weight:bold;"> 1391618.427</td>
	        <td style="text-align right; font-weight:bold;"> 1616809.236</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 1432.677</td>
	        <td style="text-align right; font-weight:bold;"> 1274.402</td>
	        <td style="text-align right; font-weight:bold;"> 1153.948</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.333</td>
	        <td style="text-align right;"> 100.966</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.780</td>
	        <td style="text-align right;"> 99.068</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 240.430</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 280.384</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 325.756</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 50.992</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 45.359</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 41.072</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 14</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 467443.119</td>
	        <td style="text-align right; font-weight:bold;"> 471536.603</td>
	        <td style="text-align right; font-weight:bold;"> 464006.323</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 3206.408</td>
	        <td style="text-align right; font-weight:bold;"> 3179.027</td>
	        <td style="text-align right; font-weight:bold;"> 3227.933</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 1041810.636</td>
	        <td style="text-align right; font-weight:bold;"> 1233206.510</td>
	        <td style="text-align right; font-weight:bold;"> 1435322.688</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 1748.546</td>
	        <td style="text-align right; font-weight:bold;"> 1473.892</td>
	        <td style="text-align right; font-weight:bold;"> 1318.177</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.876</td>
	        <td style="text-align right;"> 99.265</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.146</td>
	        <td style="text-align right;"> 100.671</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 222.874</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 263.820</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 307.058</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 54.533</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 45.967</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 41.111</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 15</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 440130.536</td>
	        <td style="text-align right; font-weight:bold;"> 430036.674</td>
	        <td style="text-align right; font-weight:bold;"> 430570.985</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 3633.573</td>
	        <td style="text-align right; font-weight:bold;"> 3724.507</td>
	        <td style="text-align right; font-weight:bold;"> 3709.711</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 970833.216</td>
	        <td style="text-align right; font-weight:bold;"> 1052552.120</td>
	        <td style="text-align right; font-weight:bold;"> 1198895.524</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 2009.033</td>
	        <td style="text-align right; font-weight:bold;"> 1768.066</td>
	        <td style="text-align right; font-weight:bold;"> 1603.990</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 97.707</td>
	        <td style="text-align right;"> 97.828</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 102.503</td>
	        <td style="text-align right;"> 102.095</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 220.578</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 239.145</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 272.395</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 55.291</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 48.659</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 44.144</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 16</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 414139.710</td>
	        <td style="text-align right; font-weight:bold;"> 417299.382</td>
	        <td style="text-align right; font-weight:bold;"> 414620.827</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 4100.720</td>
	        <td style="text-align right; font-weight:bold;"> 4071.807</td>
	        <td style="text-align right; font-weight:bold;"> 4096.465</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 861772.777</td>
	        <td style="text-align right; font-weight:bold;"> 989857.108</td>
	        <td style="text-align right; font-weight:bold;"> 1098642.366</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 2392.358</td>
	        <td style="text-align right; font-weight:bold;"> 1949.306</td>
	        <td style="text-align right; font-weight:bold;"> 1774.823</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.763</td>
	        <td style="text-align right;"> 100.116</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.295</td>
	        <td style="text-align right;"> 99.896</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 208.087</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 239.015</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 265.283</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 58.340</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 47.536</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 43.281</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 17</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 392847.916</td>
	        <td style="text-align right; font-weight:bold;"> 399327.832</td>
	        <td style="text-align right; font-weight:bold;"> 397063.353</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 4579.473</td>
	        <td style="text-align right; font-weight:bold;"> 4507.548</td>
	        <td style="text-align right; font-weight:bold;"> 4536.793</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 752392.072</td>
	        <td style="text-align right; font-weight:bold;"> 796039.924</td>
	        <td style="text-align right; font-weight:bold;"> 988177.979</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 2869.932</td>
	        <td style="text-align right; font-weight:bold;"> 2563.548</td>
	        <td style="text-align right; font-weight:bold;"> 2058.294</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.649</td>
	        <td style="text-align right;"> 101.073</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.429</td>
	        <td style="text-align right;"> 99.068</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 191.522</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 202.633</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 251.542</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #d8ffd8;"> 62.669</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 55.979</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 44.946</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 18</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 365887.530</td>
	        <td style="text-align right; font-weight:bold;"> 355296.361</td>
	        <td style="text-align right; font-weight:bold;"> 361234.553</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 5172.732</td>
	        <td style="text-align right; font-weight:bold;"> 5323.793</td>
	        <td style="text-align right; font-weight:bold;"> 5244.065</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 399494.439</td>
	        <td style="text-align right; font-weight:bold;"> 467282.336</td>
	        <td style="text-align right; font-weight:bold;"> 795979.271</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 5530.730</td>
	        <td style="text-align right; font-weight:bold;"> 5036.739</td>
	        <td style="text-align right; font-weight:bold;"> 2615.915</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 97.105</td>
	        <td style="text-align right;"> 98.728</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 102.920</td>
	        <td style="text-align right;"> 101.379</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #d8ffd8;"> 109.185</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 127.712</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 217.548</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 106.921</td>
	        <td style="text-align right;"> 97.371</td>
	        <td style="text-align right; background-color: #d8ffd8;"> 50.571</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 19</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 343351.266</td>
	        <td style="text-align right; font-weight:bold;"> 339132.650</td>
	        <td style="text-align right; font-weight:bold;"> 345012.454</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 5812.136</td>
	        <td style="text-align right; font-weight:bold;"> 5873.528</td>
	        <td style="text-align right; font-weight:bold;"> 5777.743</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 243713.205</td>
	        <td style="text-align right; font-weight:bold;"> 233500.182</td>
	        <td style="text-align right; font-weight:bold;"> 302519.050</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 8515.741</td>
	        <td style="text-align right; font-weight:bold;"> 8738.884</td>
	        <td style="text-align right; font-weight:bold;"> 6843.827</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.771</td>
	        <td style="text-align right;"> 100.484</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.056</td>
	        <td style="text-align right;"> 99.408</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 70.981</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 68.006</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 88.108</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 146.517</td>
	        <td style="text-align right;"> 150.356</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 117.751</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 20</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 324448.738</td>
	        <td style="text-align right; font-weight:bold;"> 325592.373</td>
	        <td style="text-align right; font-weight:bold;"> 331685.659</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 6450.764</td>
	        <td style="text-align right; font-weight:bold;"> 6424.408</td>
	        <td style="text-align right; font-weight:bold;"> 6308.247</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 178067.169</td>
	        <td style="text-align right; font-weight:bold;"> 171432.868</td>
	        <td style="text-align right; font-weight:bold;"> 195419.298</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 12022.858</td>
	        <td style="text-align right; font-weight:bold;"> 12414.315</td>
	        <td style="text-align right; font-weight:bold;"> 10884.229</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.352</td>
	        <td style="text-align right;"> 102.231</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.591</td>
	        <td style="text-align right;"> 97.791</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 54.883</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 52.838</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 60.231</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 186.379</td>
	        <td style="text-align right;"> 192.447</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 168.728</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 21</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 312572.668</td>
	        <td style="text-align right; font-weight:bold;"> 310245.070</td>
	        <td style="text-align right; font-weight:bold;"> 311547.506</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 7011.467</td>
	        <td style="text-align right; font-weight:bold;"> 7051.405</td>
	        <td style="text-align right; font-weight:bold;"> 7038.458</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 152096.561</td>
	        <td style="text-align right; font-weight:bold;"> 140699.656</td>
	        <td style="text-align right; font-weight:bold;"> 151209.127</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 14738.142</td>
	        <td style="text-align right; font-weight:bold;"> 15771.588</td>
	        <td style="text-align right; font-weight:bold;"> 14653.400</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.255</td>
	        <td style="text-align right;"> 99.672</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.570</td>
	        <td style="text-align right;"> 100.385</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 48.660</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 45.013</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 48.376</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 210.201</td>
	        <td style="text-align right;"> 224.940</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 208.992</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 22</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 302066.350</td>
	        <td style="text-align right; font-weight:bold;"> 298716.522</td>
	        <td style="text-align right; font-weight:bold;"> 302760.342</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 7586.841</td>
	        <td style="text-align right; font-weight:bold;"> 7678.768</td>
	        <td style="text-align right; font-weight:bold;"> 7571.574</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 158692.741</td>
	        <td style="text-align right; font-weight:bold;"> 150996.955</td>
	        <td style="text-align right; font-weight:bold;"> 159257.686</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 14793.030</td>
	        <td style="text-align right; font-weight:bold;"> 15378.472</td>
	        <td style="text-align right; font-weight:bold;"> 14575.306</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.891</td>
	        <td style="text-align right;"> 100.230</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.212</td>
	        <td style="text-align right;"> 99.799</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 52.536</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 49.988</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 52.723</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 194.983</td>
	        <td style="text-align right;"> 202.699</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 192.113</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 23</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 286355.688</td>
	        <td style="text-align right; font-weight:bold;"> 287899.943</td>
	        <td style="text-align right; font-weight:bold;"> 280891.603</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 8347.043</td>
	        <td style="text-align right; font-weight:bold;"> 8307.219</td>
	        <td style="text-align right; font-weight:bold;"> 8517.369</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 115680.672</td>
	        <td style="text-align right; font-weight:bold;"> 110601.514</td>
	        <td style="text-align right; font-weight:bold;"> 112951.623</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 21079.547</td>
	        <td style="text-align right; font-weight:bold;"> 21814.093</td>
	        <td style="text-align right; font-weight:bold;"> 21391.694</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.539</td>
	        <td style="text-align right;"> 98.092</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.523</td>
	        <td style="text-align right;"> 102.041</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 40.398</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 38.624</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 39.445</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 252.539</td>
	        <td style="text-align right;"> 261.339</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 256.279</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 24</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 276649.441</td>
	        <td style="text-align right; font-weight:bold;"> 271166.609</td>
	        <td style="text-align right; font-weight:bold;"> 274828.361</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 9002.962</td>
	        <td style="text-align right; font-weight:bold;"> 9189.583</td>
	        <td style="text-align right; font-weight:bold;"> 9066.322</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 126209.119</td>
	        <td style="text-align right; font-weight:bold;"> 118392.849</td>
	        <td style="text-align right; font-weight:bold;"> 119593.461</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 20201.617</td>
	        <td style="text-align right; font-weight:bold;"> 21207.217</td>
	        <td style="text-align right; font-weight:bold;"> 21037.566</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.018</td>
	        <td style="text-align right;"> 99.342</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 102.073</td>
	        <td style="text-align right;"> 100.704</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 45.621</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 42.795</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 43.229</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 224.389</td>
	        <td style="text-align right;"> 235.558</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 233.674</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 25</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 266688.795</td>
	        <td style="text-align right; font-weight:bold;"> 263970.190</td>
	        <td style="text-align right; font-weight:bold;"> 261646.284</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 9719.040</td>
	        <td style="text-align right; font-weight:bold;"> 9812.419</td>
	        <td style="text-align right; font-weight:bold;"> 9903.635</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 98653.874</td>
	        <td style="text-align right; font-weight:bold;"> 89630.032</td>
	        <td style="text-align right; font-weight:bold;"> 89092.723</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 26817.789</td>
	        <td style="text-align right; font-weight:bold;"> 29102.813</td>
	        <td style="text-align right; font-weight:bold;"> 29289.250</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.981</td>
	        <td style="text-align right;"> 98.109</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.961</td>
	        <td style="text-align right;"> 101.899</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 36.992</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 33.608</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 33.407</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 275.930</td>
	        <td style="text-align right;"> 299.441</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 301.359</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 26</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 256041.460</td>
	        <td style="text-align right; font-weight:bold;"> 251502.369</td>
	        <td style="text-align right; font-weight:bold;"> 256703.285</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 10489.049</td>
	        <td style="text-align right; font-weight:bold;"> 10675.836</td>
	        <td style="text-align right; font-weight:bold;"> 10463.097</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 107740.816</td>
	        <td style="text-align right; font-weight:bold;"> 96198.417</td>
	        <td style="text-align right; font-weight:bold;"> 99628.339</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 25221.200</td>
	        <td style="text-align right; font-weight:bold;"> 28146.670</td>
	        <td style="text-align right; font-weight:bold;"> 27226.012</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 98.227</td>
	        <td style="text-align right;"> 100.258</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 101.781</td>
	        <td style="text-align right;"> 99.753</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 42.079</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 37.571</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 38.911</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 240.453</td>
	        <td style="text-align right;"> 268.343</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 259.566</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 27</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 241578.440</td>
	        <td style="text-align right; font-weight:bold;"> 242582.087</td>
	        <td style="text-align right; font-weight:bold;"> 240120.272</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 11533.465</td>
	        <td style="text-align right; font-weight:bold;"> 11481.770</td>
	        <td style="text-align right; font-weight:bold;"> 11599.614</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 83281.079</td>
	        <td style="text-align right; font-weight:bold;"> 77487.484</td>
	        <td style="text-align right; font-weight:bold;"> 75825.521</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 33711.036</td>
	        <td style="text-align right; font-weight:bold;"> 36238.591</td>
	        <td style="text-align right; font-weight:bold;"> 37016.156</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.415</td>
	        <td style="text-align right;"> 99.396</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.552</td>
	        <td style="text-align right;"> 100.574</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 34.474</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 32.075</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 31.388</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 292.289</td>
	        <td style="text-align right;"> 314.204</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 320.946</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 28</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 231696.014</td>
	        <td style="text-align right; font-weight:bold;"> 231753.529</td>
	        <td style="text-align right; font-weight:bold;"> 235399.801</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 12452.959</td>
	        <td style="text-align right; font-weight:bold;"> 12454.059</td>
	        <td style="text-align right; font-weight:bold;"> 12250.922</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 92065.063</td>
	        <td style="text-align right; font-weight:bold;"> 85634.278</td>
	        <td style="text-align right; font-weight:bold;"> 81121.257</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 31689.874</td>
	        <td style="text-align right; font-weight:bold;"> 34006.420</td>
	        <td style="text-align right; font-weight:bold;"> 35856.360</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.025</td>
	        <td style="text-align right;"> 101.599</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.009</td>
	        <td style="text-align right;"> 98.378</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 39.735</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 36.960</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 35.012</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 254.477</td>
	        <td style="text-align right;"> 273.079</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 287.934</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right; font-weight:bold;"> 29</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 223745.574</td>
	        <td style="text-align right; font-weight:bold;"> 222607.111</td>
	        <td style="text-align right; font-weight:bold;"> 225560.779</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right; font-weight:bold;"> 13342.744</td>
	        <td style="text-align right; font-weight:bold;"> 13407.378</td>
	        <td style="text-align right; font-weight:bold;"> 13231.294</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; font-weight:bold;"> 73367.133</td>
	        <td style="text-align right; font-weight:bold;"> 66676.362</td>
	        <td style="text-align right; font-weight:bold;"> 68334.204</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; font-weight:bold;"> 41130.458</td>
	        <td style="text-align right; font-weight:bold;"> 45078.236</td>
	        <td style="text-align right; font-weight:bold;"> 43970.624</td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;">pct</td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 99.491</td>
	        <td style="text-align right;"> 100.811</td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"></td>
	        <td style="text-align right;"> 100.484</td>
	        <td style="text-align right;"> 99.165</td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right; background-color: #ffd8d8;"> 32.790</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 29.800</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 30.541</td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right; background-color: #ffd8d8;"> 308.261</td>
	        <td style="text-align right;"> 337.848</td>
	        <td style="text-align right; background-color: #ffd8d8;"> 329.547</td>
	</tr>
        <!-- Replace through the line above this one-->
    </tbody>
</table>

### Build and Run Benchmarks

```sh
cd benchmark
bash run2.sh
gnuplot results.gp
eog *.png
```

### Alignment benchmarks

The alignment (alignas) tests are done in two directories that are modified from benchmark - **bench64** and **bench128**.

To run the benchmarks there, do the same procedure as above, simply replacing the directory name.

The alignment table above is built by running the 3 benchmarks, and then running
```sh
bash combineResults.sh
```

and then taking the output of **readmeHtml.txt** and putting it inside the **tbody** block in the README.md table.
