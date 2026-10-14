*This project has been created as part of the 42 curriculum by fanilran.*

# Codexion

## Description

**Codexion** is a concurrency simulation written in C using POSIX threads. A group of coders sit around a circular co-working hub, each needing **two USB dongles** (one on the left, one on the right) to use the shared Quantum Compiler. There are exactly as many dongles as coders, so the dongles are a scarce, contended resource.

Each coder loops through three phases:

1. **Compile** (holding both adjacent dongles),
2. **Debug**,
3. **Refactor**,

then immediately tries to compile again. A coder who does not *start* compiling within `time_to_burnout` ms of their last compile start (or of the simulation start) **burns out**, which ends the simulation. The simulation also ends successfully once every coder has compiled at least `number_of_compiles_required` times.

The goal of the project is to design a fair, deadlock-free and starvation-free resource-sharing protocol, with:

- one thread per coder (`pthread_create`),
- one mutex-protected state per dongle, including a mandatory **cooldown** after release,
- pluggable arbitration policies (`fifo` or `edf`) implemented with a custom **priority queue (heap)**,
- a dedicated **monitor thread** that detects burnout within 10 ms,
- fully serialized logging.

### Seating layout

Coder `1` sits next to coder `number_of_coders`; coder `N` sits between `N - 1` and `N + 1`. There is one dongle between each pair of neighbours. With a single coder, there is only one dongle on the table (so that coder can never compile and will burn out).

## Instructions

### Requirements

- A POSIX system (Linux / macOS)
- `cc` with pthread support

### Build

```bash
make          # builds ./codexion
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + all
```

The project is compiled with `cc -Wall -Wextra -Werror -pthread`. Sources and headers live in the `coders/` directory.

### Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Unit | Description |
|---|---|---|
| `number_of_coders` | – | Number of coders (and dongles) |
| `time_to_burnout` | ms | Max delay between the start of two consecutive compiles (or since simulation start) |
| `time_to_compile` | ms | Duration of a compile (two dongles held) |
| `time_to_debug` | ms | Duration of the debug phase |
| `time_to_refactor` | ms | Duration of the refactor phase |
| `number_of_compiles_required` | – | Simulation ends when every coder has compiled at least this many times |
| `dongle_cooldown` | ms | Time a released dongle stays unavailable |
| `scheduler` | – | `fifo` or `edf` (exactly) |

All arguments are mandatory. Invalid input (wrong argument count, negative numbers, non-integers, unknown scheduler) is rejected with an error message and a non-zero exit code.

### Examples

```bash
# 5 coders, EDF scheduling
./codexion 5 800 200 200 200 7 50 edf

# 4 coders, FIFO scheduling
./codexion 4 1000 200 100 100 5 20 fifo

# A single coder: will burn out (only one dongle exists)
./codexion 1 800 200 200 200 3 0 fifo
```

### Log format

Every state change is printed on its own line as `timestamp_in_ms X <event>`:

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
402 1 is refactoring
...
1505 4 burned out
```

Valid events: `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, `burned out`. Timestamps are measured with `gettimeofday()` relative to the start of the simulation.

## Resources

### References

- [POSIX Threads Programming (LLNL)](https://hpc-tutorials.llnl.gov/posix/)
- `man 3 pthread_create`, `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`, `gettimeofday`, `usleep`
- Dijkstra, E. W. — *Hierarchical ordering of sequential processes* (resource ordering for deadlock avoidance)
- Coffman, E. G. et al. — *System Deadlocks* (1971)
- Liu, C. L. & Layland, J. W. — *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (Earliest Deadline First)
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- Binary heap / priority queue — Cormen et al., *Introduction to Algorithms*, chapter 6

### Use of AI

<!-- Fill this in honestly. Describe which tasks AI was used for and which parts of the project. Example below. -->

AI tools were used for:

- *Brainstorming test scenarios* (e.g. edge cases such as a single coder, zero cooldown, tight burnout deadlines) — results were run and reviewed with peers.
- *Explaining* the Coffman conditions and the difference between FIFO and EDF arbitration.
- *Drafting and proofreading this README.*

All code was written, tested and is fully understood by the author(s); AI output was systematically checked and reviewed with peers before being used.

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

A deadlock requires all four Coffman conditions to hold simultaneously. The solution breaks the **circular wait** condition (and avoids hold-and-wait pathologies):

- **Mutual exclusion**: inherent to the problem (a dongle is held by one coder at a time), so it is kept.
- **Hold and wait**: a coder only waits on one dongle at a time, and dongles are always requested in a fixed global order.
- **No preemption**: kept; a dongle is only released voluntarily after compiling.
- **Circular wait**: broken with a **resource ordering** — each coder requests the dongle with the lower index first, then the higher one. The last coder (whose neighbours are dongle `N-1` and dongle `0`) therefore picks up its right dongle first, so no cycle in the wait-for graph can form.

### Starvation prevention

- **FIFO**: each dongle serves requests strictly in arrival order, so no request can be overtaken indefinitely.
- **EDF**: each dongle serves the waiting coder with the earliest deadline (`last_compile_start + time_to_burnout`). A coder that has waited longest since its last compile has the earliest deadline and therefore rises to the top of the queue, which guarantees liveness for feasible parameters.
- Ties are broken deterministically (arrival order, then coder id) so that the heap ordering is stable.

### Cooldown handling

When a dongle is released, it records `available_at = now + dongle_cooldown`. A coder granted the dongle by the scheduler waits (using `pthread_cond_timedwait` on an absolute deadline, never a busy loop) until `available_at` has passed before the dongle is actually taken. Cooldown waits are interruptible: they also wake up when the simulation stops.

### Precise burnout detection

- A separate **monitor thread** continuously checks every coder's `last_compile_start` against the current time, polling at a short interval (≈ 1 ms) so burnout is reported well within the **10 ms** tolerance.
- Coder timestamps (`last_compile_start`) are updated under a per-coder mutex at the *start* of a compile, so the monitor never reads a torn value.
- When burnout is detected, the monitor sets the shared `stop` flag, prints the `burned out` line (the only message allowed after the stop), and wakes all waiting threads.
- The monitor also sets `stop` when every coder has reached `number_of_compiles_required`.

### Log serialization

All output goes through a single **log mutex**. A message is formatted and written while holding it, and it checks the `stop` flag under the same lock, so:

- two messages never interleave on one line,
- no regular state message is printed after the burnout message.

### Other edge cases

- **Single coder**: only one dongle exists; the coder takes it, can never get a second one, and burns out at `time_to_burnout`.
- **Clean shutdown**: after `stop` is set, all threads are woken, joined, and every mutex / condition variable is destroyed. All heap memory is freed (no leaks).
- **Input validation**: invalid numbers or scheduler values are rejected before any thread is created.
- **No global variables**: all shared state lives in a context struct passed to every thread.

## Thread synchronization mechanisms

### Primitives used

| Primitive | Protects / used for |
|---|---|
| `pthread_t` | One thread per coder + one monitor thread |
| `pthread_mutex_t` (per dongle) | The dongle's state: `in_use`, `available_at` (cooldown) and its wait queue (heap) |
| `pthread_cond_t` (per dongle) | Coders waiting for their turn / for the cooldown to expire on that dongle (`pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_broadcast`) |
| `pthread_mutex_t` (log) | Serialized output so lines never interleave |
| `pthread_mutex_t` (per coder) | `last_compile_start` and `compile_count`, shared between the coder and the monitor |
| `pthread_mutex_t` (simulation state) | The shared `stop` flag read by coders and written by the monitor |

### Dongle arbitration

Each dongle owns a **priority queue (binary heap)** of pending requests. Each request carries the coder id, the arrival sequence number and the coder's deadline. The heap comparison depends on the scheduler:

- `fifo`: ordered by arrival sequence number,
- `edf`: ordered by deadline (`last_compile_start + time_to_burnout`), ties broken by arrival order.

A coder acquires a dongle as follows:

1. Lock the dongle mutex and push a request into the heap.
2. `pthread_cond_wait` until the request is at the top of the heap **and** the dongle is not in use.
3. If the cooldown is still running, `pthread_cond_timedwait` until `available_at`.
4. Pop the request, mark the dongle as in use, unlock.

On release, the coder sets `in_use = 0` and `available_at = now + dongle_cooldown` under the dongle mutex, then calls `pthread_cond_broadcast` so the next coder in the queue re-evaluates its predicate.

### Race condition prevention

- **Duplicated dongle**: the `in_use` flag is only read and modified while holding the dongle mutex, and the wait predicate is re-checked in a `while` loop after every wake-up (protecting against spurious wake-ups).
- **Burnout vs. compile start**: the coder updates `last_compile_start` and increments `compile_count` under its own mutex; the monitor reads both under the same mutex, so it can never observe a half-updated state.
- **Stop flag**: the `stop` flag is only accessed under its mutex. Coders check it after every wait and before every state change, so they exit promptly instead of sleeping through the end of the simulation.
- **Sleeping**: long phases (compile / debug / refactor) are implemented with short `usleep` slices that re-check the `stop` flag, so threads terminate quickly after burnout.

### Coder ↔ monitor communication

The coders and monitor never talk directly. They communicate through shared state protected by mutexes:

- coders **write** `last_compile_start` / `compile_count` (per-coder mutex),
- the monitor **reads** them and **writes** the `stop` flag (simulation mutex),
- coders **read** the `stop` flag and leave their loop,
- on stop, the monitor broadcasts on every dongle condition variable so that blocked coders wake up, see `stop`, and return.

## Project structure

```
.
├── Makefile
├── README.md
└── Coders/
    ├── *.c
    └── *.h
```
