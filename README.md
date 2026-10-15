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
	        <td style="text-align right;"><span style="font-weight:bold;"> 0</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 35414052.507</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 34944135.825</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 35183426.950</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 4.396</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4.454</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4.424</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 50239938.956</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 49975245.555</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 50622071.863</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3.569</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3.595</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3.550</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.673</span></td>
	        <td > 99.349</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.319</span></td>
	        <td > 100.637</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 141.864</span></td>
	        <td style="background-color: #d8ffd8;"> 141.117</span></td>
	        <td style="background-color: #d8ffd8;"> 142.943</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 81.187</span></td>
	        <td style="background-color: #d8ffd8;"> 81.779</span></td>
	        <td style="background-color: #d8ffd8;"> 80.755</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 6990012.858</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7219073.283</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7122395.009</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 37.619</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 37.552</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 37.598</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 11759434.207</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 10185062.208</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 9919177.389</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 26.474</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 28.894</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 29.146</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 103.277</span></td>
	        <td > 101.894</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.822</span></td>
	        <td > 99.944</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 168.232</span></td>
	        <td style="background-color: #d8ffd8;"> 145.709</span></td>
	        <td style="background-color: #d8ffd8;"> 141.905</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 70.374</span></td>
	        <td style="background-color: #d8ffd8;"> 76.807</span></td>
	        <td style="background-color: #d8ffd8;"> 77.477</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3272492.196</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3315878.576</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3305732.059</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 102.538</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 101.096</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 101.468</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 7502863.296</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 6094786.489</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 6635457.825</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 59.608</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 70.790</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 67.049</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 101.326</span></td>
	        <td > 101.016</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 98.594</span></td>
	        <td > 98.956</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 229.271</span></td>
	        <td style="background-color: #d8ffd8;"> 186.243</span></td>
	        <td style="background-color: #d8ffd8;"> 202.765</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 58.133</span></td>
	        <td style="background-color: #d8ffd8;"> 69.038</span></td>
	        <td style="background-color: #d8ffd8;"> 65.389</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1885010.299</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1933630.078</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1929676.695</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 225.943</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 220.752</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 220.968</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 5164591.796</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5212193.756</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5125541.513</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 113.780</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 117.056</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 118.848</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 102.579</span></td>
	        <td > 102.370</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 97.703</span></td>
	        <td > 97.798</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 273.982</span></td>
	        <td style="background-color: #d8ffd8;"> 276.507</span></td>
	        <td style="background-color: #d8ffd8;"> 271.911</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 50.358</span></td>
	        <td style="background-color: #d8ffd8;"> 51.808</span></td>
	        <td style="background-color: #d8ffd8;"> 52.601</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 4</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1417964.998</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1421807.406</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1422307.155</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 366.041</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 365.604</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 364.856</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 4458619.622</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4635251.434</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4651979.912</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 158.810</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 164.454</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 164.323</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.271</span></td>
	        <td > 100.306</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.881</span></td>
	        <td > 99.676</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 314.438</span></td>
	        <td style="background-color: #d8ffd8;"> 326.895</span></td>
	        <td style="background-color: #d8ffd8;"> 328.074</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 43.386</span></td>
	        <td style="background-color: #d8ffd8;"> 44.928</span></td>
	        <td style="background-color: #d8ffd8;"> 44.892</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 5</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1138799.643</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1151028.439</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1161514.601</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 537.703</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 532.131</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 527.655</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3939322.394</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3856724.154</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3863948.508</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 211.964</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 230.961</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 230.915</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 101.074</span></td>
	        <td > 101.995</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 98.964</span></td>
	        <td > 98.131</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 345.919</span></td>
	        <td style="background-color: #d8ffd8;"> 338.666</span></td>
	        <td style="background-color: #d8ffd8;"> 339.300</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 39.420</span></td>
	        <td style="background-color: #d8ffd8;"> 42.953</span></td>
	        <td style="background-color: #d8ffd8;"> 42.945</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 6</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 945769.780</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 961709.199</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 961734.839</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 746.572</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 735.125</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 735.124</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3325069.884</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3235056.500</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3289890.089</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 290.690</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 316.306</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 315.232</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 101.685</span></td>
	        <td > 101.688</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 98.467</span></td>
	        <td > 98.467</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 351.573</span></td>
	        <td style="background-color: #d8ffd8;"> 342.055</span></td>
	        <td style="background-color: #d8ffd8;"> 347.853</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 38.937</span></td>
	        <td style="background-color: #d8ffd8;"> 42.368</span></td>
	        <td style="background-color: #d8ffd8;"> 42.224</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 7</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 836535.678</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 836021.608</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 831744.370</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 961.314</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 961.557</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 967.904</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3013608.954</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2751836.943</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2755177.849</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 370.999</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 421.210</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 421.893</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 99.939</span></td>
	        <td > 99.427</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.025</span></td>
	        <td > 100.686</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 360.249</span></td>
	        <td style="background-color: #d8ffd8;"> 328.956</span></td>
	        <td style="background-color: #d8ffd8;"> 329.356</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 38.593</span></td>
	        <td style="background-color: #d8ffd8;"> 43.816</span></td>
	        <td style="background-color: #d8ffd8;"> 43.887</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 8</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 775009.779</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 762672.093</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 784770.516</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1166.861</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1186.760</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1152.523</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2625843.613</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2505477.630</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2489105.961</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 480.796</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 529.221</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 528.732</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.408</span></td>
	        <td > 101.259</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.705</span></td>
	        <td > 98.771</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 338.814</span></td>
	        <td style="background-color: #d8ffd8;"> 323.283</span></td>
	        <td style="background-color: #d8ffd8;"> 321.171</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 41.204</span></td>
	        <td style="background-color: #d8ffd8;"> 45.354</span></td>
	        <td style="background-color: #d8ffd8;"> 45.312</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 9</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 723996.882</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 714079.891</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 725020.843</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1388.784</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1408.143</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1388.193</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2372237.816</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2237002.717</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2340544.187</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 595.976</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 653.086</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 639.715</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.630</span></td>
	        <td > 100.141</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.394</span></td>
	        <td > 99.957</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 327.659</span></td>
	        <td style="background-color: #d8ffd8;"> 308.980</span></td>
	        <td style="background-color: #d8ffd8;"> 323.281</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 42.914</span></td>
	        <td style="background-color: #d8ffd8;"> 47.026</span></td>
	        <td style="background-color: #d8ffd8;"> 46.063</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 10</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 656427.583</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 652673.602</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 655021.858</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1670.827</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1680.048</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1672.926</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1690339.496</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2041063.274</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2120844.285</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 810.949</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 793.098</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 775.193</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 99.428</span></td>
	        <td > 99.786</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.552</span></td>
	        <td > 100.126</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 257.506</span></td>
	        <td style="background-color: #d8ffd8;"> 310.935</span></td>
	        <td style="background-color: #d8ffd8;"> 323.089</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 48.536</span></td>
	        <td style="background-color: #d8ffd8;"> 47.467</span></td>
	        <td style="background-color: #d8ffd8;"> 46.396</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 11</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 603668.719</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 592589.289</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 604014.825</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1981.581</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2019.263</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1982.558</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1569975.837</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1895902.420</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1959041.348</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 957.713</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 938.878</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 924.718</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.165</span></td>
	        <td > 100.057</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.902</span></td>
	        <td > 100.049</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 260.072</span></td>
	        <td style="background-color: #d8ffd8;"> 314.063</span></td>
	        <td style="background-color: #d8ffd8;"> 324.523</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 48.331</span></td>
	        <td style="background-color: #d8ffd8;"> 47.380</span></td>
	        <td style="background-color: #d8ffd8;"> 46.666</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 12</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 536487.284</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 542691.694</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 552712.524</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2412.810</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2386.827</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2344.242</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1369233.273</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1589010.073</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1819588.394</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1181.085</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1102.722</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1014.447</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 101.156</span></td>
	        <td > 103.024</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 98.923</span></td>
	        <td > 97.158</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 255.222</span></td>
	        <td style="background-color: #d8ffd8;"> 296.188</span></td>
	        <td style="background-color: #d8ffd8;"> 339.167</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 48.951</span></td>
	        <td style="background-color: #d8ffd8;"> 45.703</span></td>
	        <td style="background-color: #d8ffd8;"> 42.044</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 13</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 496325.478</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 493013.089</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 501120.174</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2809.605</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2831.521</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2783.412</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1193316.741</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1391618.427</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1616809.236</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1432.677</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1274.402</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1153.948</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 99.333</span></td>
	        <td > 100.966</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.780</span></td>
	        <td > 99.068</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 240.430</span></td>
	        <td style="background-color: #d8ffd8;"> 280.384</span></td>
	        <td style="background-color: #d8ffd8;"> 325.756</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 50.992</span></td>
	        <td style="background-color: #d8ffd8;"> 45.359</span></td>
	        <td style="background-color: #d8ffd8;"> 41.072</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 14</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 467443.119</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 471536.603</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 464006.323</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3206.408</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3179.027</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3227.933</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1041810.636</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1233206.510</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1435322.688</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 1748.546</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1473.892</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1318.177</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.876</span></td>
	        <td > 99.265</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.146</span></td>
	        <td > 100.671</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 222.874</span></td>
	        <td style="background-color: #d8ffd8;"> 263.820</span></td>
	        <td style="background-color: #d8ffd8;"> 307.058</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 54.533</span></td>
	        <td style="background-color: #d8ffd8;"> 45.967</span></td>
	        <td style="background-color: #d8ffd8;"> 41.111</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 15</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 440130.536</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 430036.674</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 430570.985</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 3633.573</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3724.507</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 3709.711</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 970833.216</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1052552.120</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1198895.524</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2009.033</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1768.066</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1603.990</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 97.707</span></td>
	        <td > 97.828</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 102.503</span></td>
	        <td > 102.095</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 220.578</span></td>
	        <td style="background-color: #d8ffd8;"> 239.145</span></td>
	        <td style="background-color: #d8ffd8;"> 272.395</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 55.291</span></td>
	        <td style="background-color: #d8ffd8;"> 48.659</span></td>
	        <td style="background-color: #d8ffd8;"> 44.144</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 16</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 414139.710</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 417299.382</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 414620.827</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 4100.720</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4071.807</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4096.465</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 861772.777</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 989857.108</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1098642.366</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2392.358</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1949.306</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 1774.823</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.763</span></td>
	        <td > 100.116</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.295</span></td>
	        <td > 99.896</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 208.087</span></td>
	        <td style="background-color: #d8ffd8;"> 239.015</span></td>
	        <td style="background-color: #d8ffd8;"> 265.283</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 58.340</span></td>
	        <td style="background-color: #d8ffd8;"> 47.536</span></td>
	        <td style="background-color: #d8ffd8;"> 43.281</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 17</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 392847.916</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 399327.832</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 397063.353</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 4579.473</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4507.548</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 4536.793</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 752392.072</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 796039.924</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 988177.979</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 2869.932</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2563.548</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2058.294</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 101.649</span></td>
	        <td > 101.073</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 98.429</span></td>
	        <td > 99.068</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 191.522</span></td>
	        <td style="background-color: #d8ffd8;"> 202.633</span></td>
	        <td style="background-color: #d8ffd8;"> 251.542</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #d8ffd8;"> 62.669</span></td>
	        <td style="background-color: #d8ffd8;"> 55.979</span></td>
	        <td style="background-color: #d8ffd8;"> 44.946</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 18</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 365887.530</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 355296.361</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 361234.553</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 5172.732</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5323.793</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5244.065</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 399494.439</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 467282.336</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 795979.271</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 5530.730</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5036.739</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 2615.915</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 97.105</span></td>
	        <td > 98.728</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 102.920</span></td>
	        <td > 101.379</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #d8ffd8;"> 109.185</span></td>
	        <td style="background-color: #d8ffd8;"> 127.712</span></td>
	        <td style="background-color: #d8ffd8;"> 217.548</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 106.921</span></td>
	        <td > 97.371</span></td>
	        <td style="background-color: #d8ffd8;"> 50.571</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 19</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 343351.266</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 339132.650</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 345012.454</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 5812.136</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5873.528</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 5777.743</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 243713.205</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 233500.182</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 302519.050</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 8515.741</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 8738.884</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 6843.827</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.771</span></td>
	        <td > 100.484</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.056</span></td>
	        <td > 99.408</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 70.981</span></td>
	        <td style="background-color: #ffd8d8;"> 68.006</span></td>
	        <td style="background-color: #ffd8d8;"> 88.108</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 146.517</span></td>
	        <td > 150.356</span></td>
	        <td style="background-color: #ffd8d8;"> 117.751</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 20</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 324448.738</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 325592.373</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 331685.659</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 6450.764</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 6424.408</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 6308.247</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 178067.169</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 171432.868</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 195419.298</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 12022.858</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 12414.315</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 10884.229</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.352</span></td>
	        <td > 102.231</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.591</span></td>
	        <td > 97.791</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 54.883</span></td>
	        <td style="background-color: #ffd8d8;"> 52.838</span></td>
	        <td style="background-color: #ffd8d8;"> 60.231</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 186.379</span></td>
	        <td > 192.447</span></td>
	        <td style="background-color: #ffd8d8;"> 168.728</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 21</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 312572.668</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 310245.070</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 311547.506</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 7011.467</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7051.405</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7038.458</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 152096.561</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 140699.656</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 151209.127</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 14738.142</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 15771.588</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 14653.400</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 99.255</span></td>
	        <td > 99.672</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.570</span></td>
	        <td > 100.385</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 48.660</span></td>
	        <td style="background-color: #ffd8d8;"> 45.013</span></td>
	        <td style="background-color: #ffd8d8;"> 48.376</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 210.201</span></td>
	        <td > 224.940</span></td>
	        <td style="background-color: #ffd8d8;"> 208.992</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 22</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 302066.350</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 298716.522</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 302760.342</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 7586.841</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7678.768</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 7571.574</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 158692.741</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 150996.955</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 159257.686</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 14793.030</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 15378.472</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 14575.306</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.891</span></td>
	        <td > 100.230</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.212</span></td>
	        <td > 99.799</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 52.536</span></td>
	        <td style="background-color: #ffd8d8;"> 49.988</span></td>
	        <td style="background-color: #ffd8d8;"> 52.723</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 194.983</span></td>
	        <td > 202.699</span></td>
	        <td style="background-color: #ffd8d8;"> 192.113</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 23</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 286355.688</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 287899.943</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 280891.603</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 8347.043</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 8307.219</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 8517.369</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 115680.672</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 110601.514</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 112951.623</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 21079.547</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 21814.093</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 21391.694</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.539</span></td>
	        <td > 98.092</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.523</span></td>
	        <td > 102.041</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 40.398</span></td>
	        <td style="background-color: #ffd8d8;"> 38.624</span></td>
	        <td style="background-color: #ffd8d8;"> 39.445</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 252.539</span></td>
	        <td > 261.339</span></td>
	        <td style="background-color: #ffd8d8;"> 256.279</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 24</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 276649.441</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 271166.609</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 274828.361</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 9002.962</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 9189.583</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 9066.322</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 126209.119</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 118392.849</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 119593.461</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 20201.617</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 21207.217</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 21037.566</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.018</span></td>
	        <td > 99.342</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 102.073</span></td>
	        <td > 100.704</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 45.621</span></td>
	        <td style="background-color: #ffd8d8;"> 42.795</span></td>
	        <td style="background-color: #ffd8d8;"> 43.229</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 224.389</span></td>
	        <td > 235.558</span></td>
	        <td style="background-color: #ffd8d8;"> 233.674</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 25</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 266688.795</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 263970.190</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 261646.284</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 9719.040</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 9812.419</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 9903.635</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 98653.874</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 89630.032</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 89092.723</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 26817.789</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 29102.813</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 29289.250</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.981</span></td>
	        <td > 98.109</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.961</span></td>
	        <td > 101.899</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 36.992</span></td>
	        <td style="background-color: #ffd8d8;"> 33.608</span></td>
	        <td style="background-color: #ffd8d8;"> 33.407</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 275.930</span></td>
	        <td > 299.441</span></td>
	        <td style="background-color: #ffd8d8;"> 301.359</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 26</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 256041.460</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 251502.369</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 256703.285</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 10489.049</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 10675.836</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 10463.097</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 107740.816</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 96198.417</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 99628.339</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 25221.200</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 28146.670</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 27226.012</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 98.227</span></td>
	        <td > 100.258</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 101.781</span></td>
	        <td > 99.753</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 42.079</span></td>
	        <td style="background-color: #ffd8d8;"> 37.571</span></td>
	        <td style="background-color: #ffd8d8;"> 38.911</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 240.453</span></td>
	        <td > 268.343</span></td>
	        <td style="background-color: #ffd8d8;"> 259.566</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 27</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 241578.440</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 242582.087</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 240120.272</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 11533.465</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 11481.770</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 11599.614</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 83281.079</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 77487.484</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 75825.521</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 33711.036</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 36238.591</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 37016.156</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.415</span></td>
	        <td > 99.396</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 99.552</span></td>
	        <td > 100.574</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 34.474</span></td>
	        <td style="background-color: #ffd8d8;"> 32.075</span></td>
	        <td style="background-color: #ffd8d8;"> 31.388</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 292.289</span></td>
	        <td > 314.204</span></td>
	        <td style="background-color: #ffd8d8;"> 320.946</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 28</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 231696.014</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 231753.529</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 235399.801</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 12452.959</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 12454.059</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 12250.922</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 92065.063</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 85634.278</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 81121.257</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 31689.874</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 34006.420</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 35856.360</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 100.025</span></td>
	        <td > 101.599</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.009</span></td>
	        <td > 98.378</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 39.735</span></td>
	        <td style="background-color: #ffd8d8;"> 36.960</span></td>
	        <td style="background-color: #ffd8d8;"> 35.012</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 254.477</span></td>
	        <td > 273.079</span></td>
	        <td style="background-color: #ffd8d8;"> 287.934</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 29</span></td>
	    <!-- Mutex Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 223745.574</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 222607.111</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 225560.779</span></td>
	    <!-- Mutex User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 13342.744</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 13407.378</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 13231.294</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 73367.133</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 66676.362</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 68334.204</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="text-align right;"><span style="font-weight:bold;"> 41130.458</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 45078.236</span></td>
	        <td style="text-align right;"><span style="font-weight:bold;"> 43970.624</span></td>
	</tr>
	<tr>
	    <!-- Threads -->
	        <td >pct</span></td>
	    <!-- Mutex Locks / Second -->
	        <td ></span></td>
	        <td > 99.491</span></td>
	        <td > 100.811</span></td>
	    <!-- Mutex User+System -->
	        <td ></span></td>
	        <td > 100.484</span></td>
	        <td > 99.165</span></td>
	    <!-- FastSpinLock Locks / Second -->
	        <td style="background-color: #ffd8d8;"> 32.790</span></td>
	        <td style="background-color: #ffd8d8;"> 29.800</span></td>
	        <td style="background-color: #ffd8d8;"> 30.541</span></td>
	    <!-- FastSpinLock User+System -->
	        <td style="background-color: #ffd8d8;"> 308.261</span></td>
	        <td > 337.848</span></td>
	        <td style="background-color: #ffd8d8;"> 329.547</span></td>
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
