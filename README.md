# Dining Philosophers Problem

A C implementation of the classic Dining Philosophers synchronisation problem.

It does not represent a real-world system, but it shows a core lesson of process
synchronisation: sharing a limited set of resources among concurrent processes with
mutual exclusion, without deadlock and without starvation.

## How it works

- Each philosopher is a POSIX thread; each chopstick is a `pthread_mutex_t`.
- A philosopher needs the chopsticks on both sides to eat.
- **Deadlock avoidance – resource ordering:** every philosopher picks up the
  lower-numbered chopstick first. A circular wait cannot form, so the program
  always finishes.
- Each philosopher thinks, eats a fixed number of meals, then exits; the main
  thread joins every thread and releases all resources.

## Requirements

- A C11 compiler (gcc or clang) and POSIX threads – Linux, macOS, or WSL on Windows
- `make`

## Getting started

Clone the repository:

```bash
git clone https://github.com/ezeadiele/DiningPhilosopher.git
```

```bash
cd DiningPhilosopher
```

## Build and run

Linux, macOS or WSL:

```bash
make
./dining_philosophers 5 3
```

Windows with MinGW-w64 (for example WinLibs, POSIX-threads build):

```bash
mingw32-make CC=gcc
./dining_philosophers.exe 5 3
```

Without make:

```bash
gcc -std=c11 -Wall -Wextra -pthread dining_philosophers.c -o dining_philosophers
```

The program prints each philosopher's actions and finishes with
`All N philosophers ate M time(s). No deadlock.`

Arguments:

| Argument | Meaning | Valid range |
|----------|---------|-------------|
| 1 | Number of philosophers | 2 – 1000 |
| 2 | Number of times each philosopher eats | ≥ 1 |

Other targets:

```bash
make run    # build and run with 5 philosophers, 3 meals each
make tsan   # build with ThreadSanitizer to check for data races
make clean
```
