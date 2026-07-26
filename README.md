*This project has been created as part of the 42 curriculum by aguennac.*

# Codexion

## Description

Codexion is a multithreaded concurrency simulation inspired by the dining philosophers problem. A set of coders share a circular pool of dongles and repeatedly cycle through three states: compiling, debugging, and refactoring. To compile, a coder must acquire the two dongles adjacent to it. If a coder does not begin compiling within `time_to_burnout` milliseconds from its last compile start, it burns out and the simulation stops.

The project is built around POSIX threads, per-resource synchronization, and a custom request queue per dongle. Each dongle can serve waiting coders according to one of two scheduling policies:

- `fifo`: requests are served in arrival order.
- `edf`: requests are served by earliest deadline first, using `last_compile + time_to_burnout` as the deadline and coder id as a deterministic tie-breaker.

A dedicated monitor thread watches for burnout and for the success condition where every coder has completed the required number of compiles.

## Instructions

### Compilation

```sh
make
make clean
make fclean
make re
```

The project builds the `codexion` binary with `cc -Wall -Wextra -Werror -pthread`.

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All 8 arguments are required. The first 7 must be non-negative integers, with `number_of_coders >= 1`, and `scheduler` must be either `fifo` or `edf`.

Examples:

```sh
./codexion 5 800 200 200 200 3 50 fifo
./codexion 1 500 200 200 200 3 50 edf
./codexion 4 610 200 100 100 5 20 edf
```

Each state transition is logged in the format `timestamp_in_ms coder_id message`, for example `403 2 is compiling`.

## Blocking cases handled

- Deadlock prevention: the program breaks circular wait by always requesting the lower-indexed dongle first and the higher-indexed one second. This gives a global resource order and prevents a cycle of coders from waiting on each other forever.
- Starvation prevention: under `fifo`, each dongle serves requests in strict arrival order; under `edf`, the coder with the earliest burnout deadline is served first, with coder id used as a stable tie-breaker.
- Dongle cooldown handling: when a dongle is released, it becomes unavailable until `free_at = now + dongle_cooldown`. Waiting coders re-check that deadline with timed waits, so cooldown is respected even without an explicit expiry signal.
- Precise burnout detection: the monitor checks coder state every 0.5 ms and compares the current time with `last_compile`, so burnout is detected quickly and logged before the simulation keeps running too long.
- Log serialization: all output goes through a single print mutex, which keeps log lines from interleaving and prevents messages from appearing after shutdown has been announced.
- Single-coder handling: with one coder and one dongle, the coder can take the only dongle but can never acquire a second one, so burnout is detected correctly instead of deadlocking.

## Thread synchronization mechanisms

- `pthread_mutex_t` per dongle protects each dongle's ownership state, cooldown deadline, and request heap. A coder can only inspect or modify that dongle while holding the mutex, which prevents duplicate ownership and heap corruption.
- `pthread_cond_t` per dongle provides the waiting mechanism for coders blocked on a resource. Coders wait with timed sleeps so they can wake up for cooldown expiry or shutdown, and the dongle broadcasts on release to wake every waiter when the queue may have changed.
- Custom min-heap request queue: each dongle keeps its own heap of pending requests. The heap key is either a FIFO ticket or an EDF deadline, so the head of the queue always reflects the scheduling policy without relying on ad hoc scans.
- `state` mutex for shared simulation data: the global `over` flag, the FIFO ticket counter, and each coder's `last_compile` and `compiles` counters are protected by the same mutex. Coders update their own timestamps and compile counts under that lock, and the monitor reads them under the same lock, which removes data races on shared state.
- `print` mutex for thread-safe communication with the monitor: logging first locks the print mutex and then checks the shutdown flag under the state mutex. That lock order ensures the monitor can safely announce burnout and set `over` without another thread sneaking a log message in afterward.
- Lock ordering discipline: the code keeps a consistent mutex order and never holds two dongle mutexes at once. That prevents lock-order deadlocks while still allowing coders, dongles, and the monitor to coordinate safely.

## Resources

- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/)
- [pthread_mutex_lock manual](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [pthread_cond_timedwait manual](https://man7.org/linux/man-pages/man3/pthread_cond_timedwait.3p.html)
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Coffman conditions](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)
- [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Binary heap](https://en.wikipedia.org/wiki/Binary_heap)

### How AI was used

AI was used to help draft and organize this README, especially the description of the concurrency model, the blocking cases, and the synchronization overview. It was also used to cross-check the wording of the run instructions and to make the documentation easier to read for someone unfamiliar with the project. The implementation itself was reviewed against the source code before this README was finalized.
