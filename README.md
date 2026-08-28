*This project has been created as part of the 42 curriculum by dsutormi.*

# codexion

## Description

**codexion** is a multithreaded simulation project inspired by the classic dining philosophers problem,
reskinned around a software team: instead of philosophers sharing forks, **coders** share **dongles**
(hardware license keys) to compile code.

Each coder repeatedly cycles through four phases — take dongles, compile, debug,
refactor — for a configurable number of iterations (`number_of_compiles_required`),
and "burns out" (fails the simulation) if too much time passes without compiling.
The goal of the project is to correctly model concurrent access to a shared,
limited resource (the dongles) using POSIX threads, while avoiding deadlock,
starvation, and race conditions, and enforcing a cooldown period before a dongle can be reused.

On top of the core simulation, this project adds a **queueing/scheduling layer**: coders waiting for their turn
are placed into a shared queue (implemented as a binary search tree ordered by timestamp),
and a dedicated scheduler thread dispatches them — by default in FIFO order 
with an alternate EDF (earliest deadline first) mode available.

## Instructions

### Compilation

```bash
make
```

This produces the `codexion` executable.

To remove build artifacts:

```bash
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + full rebuild
```

### Execution

```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```

- `number_of_coders` — number of coder threads (and dongles) in the simulation.
- `time_to_burnout` — time in ms without compiling before a coder burns out.
- `time_to_compile` / `time_to_debug` / `time_to_refactor` — duration in ms of each phase.
- `number_of_compiles_required` — number of full cycles each coder must complete.
- `dongle_cooldown` — minimum time in ms a dongle must sit idle before it can be reused.
- `scheduler` — edf or fifo.

Example:

```bash
./codexion 5 800 200 100 100 7 50
```

### Output

Each coder logs a timestamped line to stdout for every phase transition (dongle taken,
compiling, debugging, refactoring, finished/burned out),
serialized through a logging mutex so lines are never interleaved or corrupted.

## Blocking cases handled

- **Deadlock prevention.** Taking dongles action is protected by a single
shared by all threads mutex, so only one pair is taken in a quant of time. 
A coder always releases one dongle a little later, then another,
so if these dongles are being waited by another coder the priority will be given to a newer coder.
- **Starvation prevention.** Coders wanting to eat/compile don't spin-fight for shared state directly;
instead they register into a shared, timestamp-ordered queue and are dispatched by a dedicated scheduler
thread in FIFO order (by default), guaranteeing that a coder that has been waiting longest
is served first rather than being repeatedly outraced by neighbors.
- **Cooldown handling.** Each dongle tracks `cld_count` (the timestamp it was released)
and `ms_cld` (the required cooldown). `ft_validate()` refuses to hand out a dongle
whose cooldown window hasn't elapsed yet, under `validate_mtx`, so a dongle can't
be reused faster than the configured cooldown even under contention.
- **Precise burnout detection.** A monitor thread periodically compares each coder's
`bur_c_ms` (last compile-start timestamp) against `ms_burn` under the shared state mutex,
so burnout is detected against a consistent snapshot rather than a value that could be
mid-update by the coder thread itself. Once a coder is found to have burned out,
`prog_status` is dropped and all coder threads are woken via `pthread_cond_broadcast()`
so none of them block indefinitely waiting on a condition that will never again be signaled for their own benefit.
- **Log serialization.** All log lines for a given event are written while holding a single shared `log_mtx`,
so concurrent coders never interleave partial lines, and log output is always attributable to one complete state transition at a time.

## Thread synchronization mechanisms

- **`pthread_mutex_t etx`** — guards a coder's wait for its turn to run (`ready` flag) and the very short window in which it takes hold
of its dongles at the start of a cycle. Combined with the condition variable below, this is the coder's "wait to be scheduled" gate.
- **`pthread_mutex_t validate_mtx`** — protects the `is_busy` and `cld_count` fields of each dongle.
Any check-and-set on dongle availability (`ft_validate()`) happens entirely under this lock,
so two coders can never both observe a dongle as free and grab it simultaneously — this is
the mutex that actually prevents the classic "double-eating" race on a shared fork/dongle.
- **`pthread_mutex_t line_mtx`** — protects the shared scheduling queue
(the BST rooted at `settings->fifo_head`). Every insertion (`ft_append_tree`),
search, and removal (`ft_delrestruct_tree`) of a node happens under this single lock,
taken once for the whole tree traversal rather than per-node, so the tree's shape
is never observed or mutated inconsistently by two threads at once.
- **`pthread_mutex_t status_mtx`** (per coder) — protects a single coder's own `ready` state,
which is read by that coder's thread, written by the queue producer, and written again by the scheduler
when dispatching it. Keeping this per-coder (rather than global) avoids serializing
unrelated coders against each other for a field only they and the scheduler touch.
- **`pthread_mutex_t log_mtx`** — serializes all `printf` calls in `ft_log()` so log lines
from different coder threads are never torn or interleaved mid-line.
- **`pthread_cond_t sig`** — the condition variable coders wait on (under `etx`) until the
scheduler marks them `ready`. The scheduler and the monitor both call `pthread_cond_broadcast()` after
updating shared state (dispatching a coder, or shutting the simulation down), so every
waiting coder re-checks its condition instead of relying on being individually targeted —
this is what lets a coder block efficiently (no busy-waiting) while still being guaranteed to wake up promptly, including on shutdown.

**Example — race condition avoided:** two coders sharing dongle `d`.
Both call `ft_validate()` around the same time. Because `validate_mtx` is held for the entire
check-and-set (`is_busy` check, cooldown check, then `is_busy = 1`), only one of them
can observe `is_busy == 0` and cooldown-elapsed and proceed; the other blocks on the mutex and,
once it acquires it, correctly observes `is_busy == 1` and backs off.
Without this lock, both threads could read `is_busy == 0` before either writes `is_busy = 1`,
and both would proceed to compile with the same dongle simultaneously.

**Example — thread-safe coder/monitor communication:** the monitor thread never writes
directly into a coder's fields; it only reads `bur_c_ms` and `coms_left` (written
by the coder thread itself under its own critical sections) and, on burnout or completion,
flips the shared `prog_status` flag and broadcasts `sig`. Coder threads periodically
re-check `prog_status` and their own `running` flag after waking from `pthread_cond_wait()`, so
shutdown is communicated safely without the monitor ever needing to lock or mutate a coder's internal state directly.

## Resources

-Grokking Algorithms - Aditya Y. Bhargava - general information about BST and algorithms to work with it.
- Edsger W. Dijkstra, *Hierarchical ordering of sequential processes* (origin of the dining philosophers problem).
- [The Linux `pthread` man pages](https://man7.org/linux/man-pages/man7/pthreads.7.html) — `pthread_mutex_init(3)`, `pthread_cond_wait(3)`, `pthread_create(3)`.
- [POSIX Threads Programming, LLNL tutorial](https://hpc-tutorials.llnl.gov/posix/) — general reference on mutexes, condition variables, and common concurrency pitfalls.
- 42 School internal subject for this project (philosophers-family project).

### AI usage

An AI assistant (Claude, Anthropic) was used during development strictly as a **debugging and code-review aid**, not for original design or bulk code generation:

- Interpreting `valgrind` output (uninitialized-value and use-after-free reports) and tracing them back to specific lines of code.
- Reviewing hand-written mutex-locking logic in `run()` / `ft_handler()` for lock/unlock imbalance and identifying a resulting undefined-behavior bug (double-unlock of `line_mtx`).
- Reviewing a hand-written BST insertion/rebalancing routine (`ft_iter_tree`) for a double-insertion bug and proposing a corrected version.
- Identifying a double-free of the scheduling tree caused by `ft_free_tree()` being called from two different places.
- Drafting this README from a description of the already-implemented synchronization design.

All core algorithmic and synchronization design (mutex granularity, dongle acquisition order, queue/scheduler architecture) was authored by 
the project's team members; AI assistance was limited to reviewing and debugging.